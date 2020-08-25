#include "astar.h"
#include <algorithm>
#include <math.h>
#include <thread>
#include <iostream>


/*  Heuristic Function
***/
void AStar::setHeuristicFunction(HeuristicFunction heurFunc) {
    heuristicFunc = heurFunc;
}

void AStar::setManhattanHeuristicFunction() {
    heuristicFunc = Heuristic::manhattan;
}

void AStar::setEuclideanHeuristicFunction() {
    heuristicFunc = Heuristic::euclidean;
}


/*  Find path
***/
std::list<Coordinate> AStar::findPath(const Coordinate& src, const Coordinate& dest) {
    auto start = std::chrono::system_clock::now();
    stopFindPath = false;

    Node* current = nullptr;

    std::list<Node*> openList, closedList;
    Node* node = new Node(src, nullptr);
    node->directions = getDirections(src, dest);
    openList.push_back(node);

    while (!openList.empty()) {
        auto current_iter = openList.begin();
        current = *current_iter;

        for (auto iter = openList.begin(); iter != openList.end(); ++iter) {
            if (stopFindPath) {
                stopFindPath = false;
                return std::list<Coordinate>();
            }
            auto node = *iter;
            if (node->getScore() <= current->getScore()) {
                current = node;
                current_iter = iter;
            }
        }

        if (current->coord == dest) {
            break;
        }

        closedList.push_back(current);
        openList.erase(current_iter);

        for (int i = 0; i < directionsCount; ++i) {
            if (stopFindPath) {
                stopFindPath = false;
                return std::list<Coordinate>();
            }
            Coordinate newCoord(current->coord + current->directions[i]);
            if (detectObstacle(newCoord) || findNodeInList(closedList, newCoord)) {
                continue;
            }

            // notify GUI thread to draw visited path
            notifyVisitedCoordinate(newCoord);

            unsigned int totalCost = current->G + (i < 4 ? 10 : 14);

            Node* node = findNodeInList(openList, newCoord);
            if (!node) {
                node = new Node(newCoord, current);
                node->directions = getDirections(newCoord, dest);
                node->G = totalCost;
                node->H = heuristicFunc(node->coord, dest);
                openList.push_back(node);
            }
            else if (totalCost < node->G) {
                node->parent = current;
                node->G = totalCost;
            }
        }
    }

    std::list<Coordinate> path;
    if (current->coord == dest) {
        while (current) {
            path.push_front(current->coord);
            current = current->parent;
        }
    }

    destroyList(openList);
    destroyList(closedList);

    auto end = std::chrono::system_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    emit sigTimeUsed(dur.count());

    return path;
}


/*  Helper function
***/
void AStar::destroyList(std::list<Node*>& list) const {
    for (auto iter = list.begin(); iter != list.end(); ) {
        delete *iter;
        iter = list.erase(iter);
    }
}

AStar::Node* AStar::findNodeInList(const std::list<Node*>& list, const Coordinate& coord) const {
    for (auto node : list) {
        if (node->coord == coord) {
            return node;
        }
    }
    return nullptr;
}


/*  Heuristic Function
***/
unsigned int Heuristic::manhattan(const Coordinate& src, const Coordinate& dest) {
    unsigned int deltaX = abs(src.x - dest.x);
    unsigned int deltaY = abs(src.y - dest.y);
    return 10 * (deltaX + deltaY);
}

unsigned int Heuristic::euclidean(const Coordinate& src, const Coordinate& dest) {
    unsigned int deltaX = abs(src.x - dest.x);
    unsigned int deltaY = abs(src.y - dest.y);
    return 10 * sqrt(deltaX * deltaX + deltaY * deltaY);
}
