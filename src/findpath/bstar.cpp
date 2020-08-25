#include "bstar.h"
#include "../utils/utils.h"
#include <iostream>
#include <chrono>


std::list<Coordinate> BStar::findPath(const Coordinate &src, const Coordinate &dest) {
    auto start = std::chrono::system_clock::now();
    this->directionsCount = 4;
    stopFindPath = false;

    book = util::New2D<int>(boundary.width(), boundary.height(), INT_MAX);
    book[src.y][src.x] = 0;

    Node* srcNode = new Node(src, nullptr);
    srcNode->directions = new Coordinate[4];
    srcNode->idx = 3;
    Coordinate* dirs = getDirections(src, dest);
    for (int i = 0; i < 4; ++i) {
        srcNode->directions[3-i] = dirs[i];
    }
    nodes.push_back(srcNode);
    freeNodes.push_front(srcNode);

    Node* current = nullptr;

    while (true) {
        if (freeNodes.empty()) {
            for (auto iter = nodes.rbegin(); iter != nodes.rend(); ++iter) {
                Node* node = *iter;
                if (node->directions) {
                    continue;
                }
                else {
                    Coordinate* dirs = getDirections(node->coord, dest);
                    bool flags[4];
                    int count = 0;
                    for (int i = 0; i < 4; ++i) {
                        Coordinate newCoord = node->coord + dirs[i];
                        if (detectObstacle(newCoord)) {
                            flags[i] = false;
                        }
                        else if (book[newCoord.y][newCoord.x] == INT_MAX) {
                            count++;
                            flags[i] = true;
                        }
                        else {
                            flags[i] = false;
                        }
                    }
                    if (count <= 0) {
                        continue;
                    }
                    else {
                        node->directions = new Coordinate[count];
                        for (int i = 3; i >= 0; --i) {
                            if (flags[i]) {
                                node->idx++;
                                node->directions[node->idx] = dirs[i];
                            }
                        }
                        freeNodes.push_front(node);
                        break;
                    }
                }
            }
            if (freeNodes.empty()) {
                break;
            }
        }

        Node* freeNode = freeNodes.front();
        if (freeNode->idx < 0) {
            freeNodes.pop_front();
            continue;
        }
        const Coordinate& direcction = freeNode->currentDirection();
        freeNode->idx--;

        current = freeNode;

        while (1) {
            if (stopFindPath) {
                cleanup();
                return {};
            }
            Coordinate newCoord = current->coord + direcction;
            if (detectObstacle(newCoord) || book[newCoord.y][newCoord.x] != INT_MAX) {
                Node* freeNode = convertToFreeNode(current, direcction, dest);
                if (freeNode) {
                    freeNodes.push_front(freeNode);
                }
                break;
            }

            notifyVisitedCoordinate(newCoord);

            if (newCoord == dest) {
                // found path
                std::list<Coordinate> path;
                path.push_front(dest);
                while (current) {
                    path.push_front(current->coord);
                    current = current->parent;
                }

                auto end = std::chrono::system_clock::now();
                auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                emit sigTimeUsed(dur.count());

                cleanup();
                return path;
            }
            else {
                book[newCoord.y][newCoord.x] = 1;
                Node* node = new Node(newCoord, current);
                nodes.push_back(node);
                current = node;
            }
        }
    }

    cleanup();
    return {};
}


void BStar::cleanup() {
    util::Delete2D(book, boundary.width(), boundary.height());
    for (auto iter = nodes.begin(); iter != nodes.end(); ) {
        delete *iter;
        iter = nodes.erase(iter);
    }
    for (auto iter = freeNodes.begin(); iter != freeNodes.end(); ) {
        iter = freeNodes.erase(iter);
    }
}


BStar::Node* BStar::findNodeInList(const std::list<Node*> &nodes, const Coordinate &coord) {
    for (auto& node : nodes) {
        if (node->coord == coord) {
            return node;
        }
    }
    return nullptr;
}


BStar::Node* BStar::convertToFreeNode(Node* node, const Coordinate &currDir, const Coordinate &dest)
{
    if (node->directions) {
        return nullptr;
    }

    const Coordinate& currCoord = node->coord;
    Coordinate nextDir, nextNextDir;

    if (currDir.x == 0) {
        if (currCoord.x < dest.x) {
            nextDir = { 1, 0 };
            nextNextDir = { -1, 0 };
        }
        else {
            nextDir = { -1, 0 };
            nextNextDir = { 1, 0 };
        }
    }
    else {
        if (currCoord.y < dest.y) {
            nextDir = { 0, 1 };
            nextNextDir = { 0, -1 };
        }
        else {
            nextDir = { 0, -1 };
            nextNextDir = { 0, 1 };
        }
    }

    Coordinate nextCoord = currCoord + nextDir;
    Coordinate nextNextCoord = currCoord + nextNextDir;

    bool next = detectObstacle(nextCoord) || book[nextCoord.y][nextCoord.x] != INT_MAX;
    bool nextNext = detectObstacle(nextNextCoord) || book[nextNextCoord.y][nextNextCoord.x] != INT_MAX;

    if (nextNext) {
        if (next) {
            return nullptr;
        }
        else {
            node->directions = new Coordinate(nextDir);
            node->idx = 0;
        }
    }
    else {
        if (next) {
            node->directions = new Coordinate(nextNextDir);
            node->idx = 0;
        }
        else {
            node->directions = new Coordinate[2];
            node->directions[1] = nextDir;
            node->directions[0] = nextNextDir;
            node->idx = 1;
        }
    }

    return node;
}
