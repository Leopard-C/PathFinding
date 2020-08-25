#ifndef FINDPATH_BASE_H
#define FINDPATH_BASE_H

#include <list>
#include <vector>
#include <functional>
#include <QObject>

// Coordinate for 2D point
//
struct Coordinate {
    Coordinate() : x(0), y(0) {}
    Coordinate(int x, int y) : x(x), y(y) {}
    void setXY(int x_, int y_) { x = x_; y= y_; }
    bool operator==(const Coordinate& rhs) const {
        return (x == rhs.x && y == rhs.y);
    }
    friend Coordinate operator+(const Coordinate& left, const Coordinate& right);
    int x;
    int y;
};

struct Boundary {
    int width() const { return maxX - minX + 1; }
    int height() const { return maxY - minY + 1; }
    int minX = 0;
    int maxX = 0;
    int minY = 0;
    int maxY = 0;
};


/******************************************
 *
 *   Base class for PathFinding
 *
******************************************/

class FindPathBase : public QObject
{
    Q_OBJECT
public:
    FindPathBase() = default;
    virtual ~FindPathBase() {}

signals:
    void sigVisited(int x, int y) const;
    void sigTimeUsed(long us);

public:
    /*
     *     REQUIRE Override ! ! !
     */
    virtual std::list<Coordinate> findPath(const Coordinate& src, const Coordinate& dest) = 0;

public:
    void stop() { stopFindPath = true; }
    void copyFrom(FindPathBase* ori);

    /* diagonal move */
    void enableDiagonalMove(bool enable = true);
    void disableDiagonalMove();

    /* Visited Path */
    void setShowVisitedPath(bool show) { showVisitedPath = show; }
    void setShowVisitedPathInterval(int interval) { showVisitedPathInterval = interval; }
    void notifyVisitedCoordinate(const Coordinate& coord);

    /* Boundary & Obstacle */
    bool detectObstacle(const Coordinate& coord) const;
    void setBoundary(int minX, int maxX, int minY, int maxY);
    void setBoundary(const Boundary& bound);
    void addObstacles(const std::vector<Coordinate>& obs);
    void addObstacle(const Coordinate& coord);
    void removeObstacle(const Coordinate& coord);
    void clearObstacles();

    Coordinate* getDirections(const Coordinate& src, const Coordinate& dest);

protected:
    Boundary boundary;
    std::vector<Coordinate> obstacles;

    bool stopFindPath = false;
    bool showVisitedPath = false;
    int showVisitedPathInterval = 10;  // 10 milliseconds

    //static Coordinate directions[8];
    static Coordinate directions4[8][4];
    static Coordinate directions8[8][8];
    int directionsCount = 4;
};

#endif // FINDPATH_BASE_H
