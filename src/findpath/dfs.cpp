#include "dfs.h"
#include "../utils/utils.h"


std::list<Coordinate> DFS::findPath(const Coordinate &src, const Coordinate &dest) {
    auto start = std::chrono::system_clock::now();
    stopFindPath = false;

    book = util::New2D<int>(boundary.width(), boundary.height(), INT_MAX);
    book[src.y][src.x] = 0;

    Node* current = nullptr;

    Node* node = new Node(src);
    node->directions = getDirections(src, dest);
    s.push(node);
    sAll.push(node);

    while (!s.empty()) {
        current = s.top();

        if (stopFindPath) {
            cleanup();
            return {};
        }

        if (current->count >= directionsCount) {
            s.pop();
        }
        else {
            Coordinate newCoord = current->coord + current->directions[current->count];
            current->count++;
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
                s.push(node);
                sAll.push(node);
                book[newCoord.y][newCoord.x] = 1;
            }
            else {
                book[newCoord.y][newCoord.x] = 1;
            }

            // Found dest
            // Return path
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
        }
    }

    cleanup();
    return {};
}


/*  Helper function
***/
void DFS::destroyStack(std::stack<Node *> &s, bool del) const {
    if (del) {
        while (!s.empty()) {
            delete s.top();
            s.pop();
        }
    }
    else {
        while (!s.empty()) {
            s.pop();
        }
    }
}

void DFS::cleanup() {
    destroyStack(s, false);
    destroyStack(sAll, true);
    util::Delete2D(book, boundary.width(), boundary.height());
    book = nullptr;
}
