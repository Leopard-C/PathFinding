#ifndef ASTAR_H
#define ASTAR_H

#include <list>
#include <vector>
#include <functional>
#include "findpath_base.h"

using HeuristicFunction = std::function<unsigned int(const Coordinate&, const Coordinate&)>;

class Heuristic {
public:
    static unsigned int manhattan(const Coordinate& src, const Coordinate& dest);
    static unsigned int euclidean(const Coordinate& src, const Coordinate& dest);
};


/******************************************
 *
 *   Class: AStar
 *
******************************************/

class AStar : public FindPathBase
{
    Q_OBJECT
public:
    struct Node {
        Node(const Coordinate& coord, Node* parent = nullptr) :
            coord(coord), parent(parent) {}
        Coordinate* directions;
        unsigned int getScore() const { return G + H; }
        unsigned int G = 0;
        unsigned int H = 0;
        Coordinate coord;
        Node* parent;
    };

public:
    std::list<Coordinate> findPath(const Coordinate& src, const Coordinate& dest) override;

    /* Heuristic Function */
public:
    void setHeuristicFunction(HeuristicFunction heurFunc);
    void setManhattanHeuristicFunction();
    void setEuclideanHeuristicFunction();

    /* Helper function */
private:
    void destroyList(std::list<Node*>& list) const;
    Node* findNodeInList(const std::list<Node*>& list, const Coordinate& coord) const;

private:
    HeuristicFunction heuristicFunc = Heuristic::manhattan;  // default: manhattan
};

#endif // ASTAR_H
