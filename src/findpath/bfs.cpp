#include "bfs.h"
#include "utils/utils.h"
#include <iostream>


std::list<Coordinate> BFS::findPath(const Coordinate &src, const Coordinate &dest) {
    auto start = std::chrono::system_clock::now();
    stopFindPath = false;

    book = util::New2D<int>(boundary.width(), boundary.height(), INT_MAX);
    book[src.y][src.x] = 0;

    Node* current = nullptr;

    Node* node = new Node(src);
    node->directions = getDirections(src, dest);
    q.push_back(node);
    qAll.push_back(node);

    while (!q.empty()) {
        current = q.front();
        q.pop_front();
        for (int k = 0; k < directionsCount; ++k) {
            Coordinate newCoord = current->coord + current->directions[k];
            if (detectObstacle(newCoord)) {
                continue;
            }

            if (stopFindPath) {
                cleanup();
                return {};
            }

            if (book[newCoord.y][newCoord.x] == INT_MAX) {
                notifyVisitedCoordinate(newCoord);
                Node* node = new Node(newCoord, current);
                node->directions = getDirections(newCoord, dest);
                q.push_back(node);
                qAll.push_back(node);
                book[newCoord.y][newCoord.x] = book[current->coord.y][current->coord.x] + 1;
            }
            else {
                book[newCoord.y][newCoord.x] =
                    std::min(book[newCoord.y][newCoord.x], book[current->coord.y][current->coord.x] + 1);
            }

            // Found path
            if (newCoord == dest) {
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
        } // for k
    } // while q is not empty

    cleanup();
    return {};
}


/*  Helper function
***/
void BFS::destroyQueue(std::deque<Node*>& q, bool del) const {
    if (del) {
        for (auto iter = q.begin(); iter != q.end(); ) {
            delete *iter;
            iter = q.erase(iter);
        }
    }
    else {
        for (auto iter = q.begin(); iter != q.end(); ) {
            iter = q.erase(iter);
        }
    }
}

void BFS::cleanup() {
    destroyQueue(q, false);
    destroyQueue(qAll, true);
    util::Delete2D(book, boundary.width(), boundary.height());
    book = nullptr;
}
