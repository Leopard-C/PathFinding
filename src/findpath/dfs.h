#ifndef DFS_H
#define DFS_H

#include "findpath/findpath_base.h"
#include <stack>


class DFS : public FindPathBase
{
public:
    struct Node {
        Node(const Coordinate& coord, Node* parent = nullptr) :
            coord(coord), parent(parent) {}
        Coordinate* directions;
        int count = 0;
        Coordinate coord;
        Node* parent;
    };

public:
    std::list<Coordinate> findPath(const Coordinate &src, const Coordinate &dest) override;

private:
    void destroyStack(std::stack<Node*>& stack, bool del) const;
    void cleanup();

private:
    int** book = nullptr;

    std::stack<Node*> s;
    std::stack<Node*> sAll;
};

#endif // DFS_H
