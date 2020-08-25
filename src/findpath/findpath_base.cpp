#include "findpath_base.h"
#include <thread>


//Coordinate FindPathBase::directions[8] = {
//    {  0,  1 }, { 1, 0 }, {  0, -1 }, { -1,  0 },
//    { -1, -1 }, { 1, 1 }, { -1,  1 }, {  1, -1 }
//};

Coordinate FindPathBase::directions4[8][4] = {
    { {  1,  0 }, {  0,  1 }, { -1,  0 }, {  0, -1 } },
    { {  0,  1 }, {  1,  0 }, {  0, -1 }, { -1,  0 } },
    { {  1,  0 }, {  0, -1 }, { -1,  0 }, {  0,  1 } },
    { {  0, -1 }, {  1,  0 }, {  0,  1 }, { -1,  0 } },
    { { -1,  0 }, {  0,  1 }, {  1,  0 }, {  0, -1 } },
    { {  0,  1 }, { -1,  0 }, {  0, -1 }, {  1,  0 } },
    { { -1,  0 }, {  0, -1 }, {  1,  0 }, {  0,  1 } },
    { {  0, -1 }, { -1,  0 }, {  0,  1 }, {  1,  0 } }
};
Coordinate FindPathBase::directions8[8][8] = {
    { {  1,  0 }, {  1,  1 }, {  0,  1 }, {  1, -1 }, {  0, -1 }, { -1,  1 }, { -1,  0 }, { -1, -1 } },
    { {  0,  1 }, {  1,  1 }, {  1,  0 }, {  1, -1 }, {  0, -1 }, { -1,  1 }, { -1,  0 }, { -1, -1 } },
    { {  1,  0 }, {  1, -1 }, {  0, -1 }, {  1,  1 }, {  0,  1 }, { -1, -1 }, { -1,  0 }, { -1,  1 } },
    { {  0, -1 }, {  1, -1 }, {  1,  0 }, {  1,  1 }, {  0,  1 }, { -1, -1 }, { -1,  0 }, { -1,  1 } },
    { { -1,  0 }, { -1,  1 }, {  0,  1 }, { -1, -1 }, {  0, -1 }, {  1,  1 }, {  1,  0 }, {  1, -1 } },
    { {  0,  1 }, { -1,  1 }, { -1,  0 }, {  1,  1 }, {  1,  0 }, { -1, -1 }, {  0, -1 }, {  1, -1 } },
    { { -1,  0 }, { -1, -1 }, {  0, -1 }, { -1,  1 }, {  0,  1 }, {  1, -1 }, {  1,  0 }, {  1,  1 } },
    { {  0, -1 }, { -1, -1 }, { -1,  0 }, { -1,  1 }, {  0,  1 }, {  1, -1 }, {  1,  0 }, {  1,  1 } },
};


Coordinate operator+(const Coordinate& left, const Coordinate& right) {
    return { left.x + right.x, left.y + right.y };
}

/*   Copy members from exist object
***/
void FindPathBase::copyFrom(FindPathBase *ori) {
    if (ori) {
        boundary = ori->boundary;
        obstacles = ori->obstacles;
        stopFindPath = ori->stopFindPath;
        showVisitedPath = ori->showVisitedPath;
        showVisitedPathInterval = ori->showVisitedPathInterval;
    }
}

/*  diagonal move
***/
void FindPathBase::enableDiagonalMove(bool enable/* = true*/) {
    directionsCount = (enable ? 8 : 4);
}

void FindPathBase::disableDiagonalMove() {
    enableDiagonalMove(false);
}


/*  Boundary & Obstacle
***/
void FindPathBase::setBoundary(int minX, int maxX, int minY, int maxY) {
    boundary.minX = minX;
    boundary.maxX = maxX;
    boundary.minY = minY;
    boundary.maxY = maxY;
}

void FindPathBase::setBoundary(const Boundary& bound) {
    boundary = bound;
}

void FindPathBase::addObstacles(const std::vector<Coordinate>& obs) {
    obstacles.insert(obstacles.end(), obs.begin(), obs.end());
}

void FindPathBase::addObstacle(const Coordinate& coord) {
    obstacles.push_back(coord);
}

void FindPathBase::removeObstacle(const Coordinate &coord) {
    auto iter = std::find(obstacles.begin(), obstacles.end(), coord);
    if (iter != obstacles.end()) {
        obstacles.erase(iter);
    }
}

void FindPathBase::clearObstacles() {
    std::vector<Coordinate>().swap(obstacles);
}

bool FindPathBase::detectObstacle(const Coordinate& coord) const {
    if (coord.x < boundary.minX || coord.x > boundary.maxX ||
        coord.y < boundary.minY || coord.y > boundary.maxY ||
        std::find(obstacles.begin(), obstacles.end(), coord) != obstacles.end())
    {
        return true;
    }
    return false;
}


void FindPathBase::notifyVisitedCoordinate(const Coordinate& coord) {
    if (showVisitedPath) {
        emit sigVisited(coord.x, coord.y);
        std::this_thread::sleep_for(std::chrono::milliseconds(showVisitedPathInterval));
    }
}


Coordinate* FindPathBase::getDirections(const Coordinate& src, const Coordinate& dest) {
    int width  = abs(dest.x - src.x);
    int height = abs(dest.y - src.y);
    if (directionsCount == 4) {
        if (src.x < dest.x) {
            if (src.y < dest.y) {
                if (width > height) {
                    return directions4[0];
                }
                else {
                    return directions4[1];
                }
            }
            else {
                if (width > height) {
                    return directions4[2];
                }
                else {
                    return directions4[3];
                }
            }
        }
        else {
            if (src.y < dest.y) {
                if (width > height) {
                    return directions4[4];
                }
                else {
                    return directions4[5];
                }
            }
            else {
                if (width > height) {
                    return directions4[6];
                }
                else {
                    return directions4[7];
                }
            }
        }
    }
    // enable Diagonal Move
    else {
        if (src.x < dest.x) {
            if (src.y < dest.y) {
                if (width > height) {
                    return directions8[0];
                }
                else {
                    return directions8[1];
                }
            }
            else {
                if (width > height) {
                    return directions8[2];
                }
                else {
                    return directions8[3];
                }
            }
        }
        else {
            if (src.y < dest.y) {
                if (width > height) {
                    return directions8[4];
                }
                else {
                    return directions8[5];
                }
            }
            else {
                if (width > height) {
                    return directions8[6];
                }
                else {
                    return directions8[7];
                }
            }
        }
    }
}
