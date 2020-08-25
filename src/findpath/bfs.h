#ifndef BFS_H
#define BFS_H

#include "findpath_base.h"
#include <deque>


class BFS : public FindPathBase
{
public:
    struct Node {
        Node(const Coordinate& coord, Node* parent = nullptr) :
            coord(coord), parent(parent) {}
        Coordinate* directions;
        Coordinate coord;
        Node* parent;
    };

public:
    std::list<Coordinate> findPath(const Coordinate &src, const Coordinate &dest) override;

private:
    void cleanup();
    void destroyQueue(std::deque<Node*>& list, bool del) const;

private:
    int** book = nullptr;

    std::deque<Node*> q;
    std::deque<Node*> qAll;
};

#endif // BFS_H
