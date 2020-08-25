#ifndef BSTAR_H
#define BSTAR_H

#include "findpath_base.h"


/*
 *  TEST ing
 *  There are bugs in the class!
 */

class BStar : public FindPathBase
{
public:
    struct Node {
        ~Node() {
            if (directions) {
                delete directions;
            }
        }
        Node(const Coordinate& coord, Node* parent = nullptr) :
            parent(parent), coord(coord) {}
        const Coordinate& currentDirection() const { return directions[idx]; }
        int idx = -1;
        Coordinate* directions = nullptr;
        Node* parent;
        Coordinate coord;
    };

public:
    std::list<Coordinate> findPath(const Coordinate &src, const Coordinate &dest) override;

private:
    Node* convertToFreeNode(Node* node, const Coordinate& currDir, const Coordinate& dest);
    Node* findNodeInList(const std::list<Node*>& nodes, const Coordinate& coord);
    void cleanup();

private:
    int** book = nullptr;
    std::list<Node*> nodes;
    std::list<Node*> freeNodes;
};

#endif // BSTAR_H
