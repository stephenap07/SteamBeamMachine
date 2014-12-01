#ifndef PATHFINDING_H_
#define PATHFINDING_H_

#include "TileMap.hpp"

struct Node {
    int x, y;
    float g, h;

    Node() :x(-1), y(-1), g(0), h(0) {}
    Node(int _x, int _y) :x(_x), y(_y), g(0), h(0) {}

    bool isValid() const { return (x != -1 && y != -1); }
    void set(int _x, int _y) { x = _x; y = _y; }

    float f() const { return g + h; }
    const bool operator==(const Node &other) { return (x == other.x && y == other.y); }
    const bool operator>(const Node &other) { return f() > other.f(); }
    const bool operator<(const Node &other) { return f() < other.f(); }
};


class Pathfinder {

typedef std::list<Node*>::iterator NodePtrIter;

public:
    Pathfinder() :m_tileMap(nullptr) {}
    Pathfinder(TileMap *tileMap) :m_tileMap(tileMap) {}

    std::vector<Node> getPath(Node start, Node goal);
    void setMap(TileMap *tileMap);

private:
    int distance(Node start, Node goal)
    {
        return std::abs(goal.x - start.x) + std::abs(goal.y - start.y);
    }

    std::vector<Node> identifySuccessors(Node current, Node start, Node goal);
    Node jump(int x, int y, int dX, int dY, const Node &start, const Node &goal);

private:
    const TileMap *m_tileMap;
};

#endif // PATHFINDING_H_
