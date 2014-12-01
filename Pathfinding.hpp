#ifndef PATHFINDING_H_
#define PATHFINDING_H_

#include "TileMap.hpp"

struct Node {
    int x, y;
    float g, h;
    Node *parent;

    Node() :x(-1), y(-1), g(0), h(0), parent(nullptr) {}
    Node(int _x, int _y) :x(_x), y(_y), g(0), h(0), parent(nullptr) {}

    bool isValid() const { return (x != -1 && y != -1); }
    void set(int _x, int _y) { x = _x; y = _y; }

    float f() const { return g + h; }
    const bool operator==(const Node &other) { return (x == other.x && y == other.y); }
    const bool operator>(const Node &other) { return f() > other.f(); }
    const bool operator<(const Node &other) { return f() < other.f(); }

    int distanceTo(const Node &other) const
    {
        return std::sqrt((other.x - x) * (other.x - x) + (other.y - y) * (other.y - y));   
    }
};


class Pathfinder {

    typedef std::list<Node*>::iterator NodeIter;

    struct Point {
        int x, y;
    };

public:
    Pathfinder() :m_tileMap(nullptr) {initSides();}
    Pathfinder(TileMap *tileMap) :m_tileMap(tileMap) {initSides();}

    std::vector<Node> getPath(Node start, Node goal);
    void setMap(TileMap *tileMap);

private:
    int manhattanDistance(Node start, Node goal)
    {
        return std::abs(goal.x - start.x) + std::abs(goal.y - start.y);
    }

    void setNodes(Node goal);

    std::vector<Node*> identifySuccessors(const Node *current, Node *start, Node *goal);
    std::vector<Node*> nodeNeighbors(const Node *node);
    Node *jump(int x, int y, int dX, int dY, Node *start, Node *goal);
    Node *nextNode(const std::list<Node*> &nodeList);
    Node *getPoolNode(int x, int y);
    void initSides();

private:
    const TileMap *m_tileMap;
    std::vector<Node> m_nodes;
    std::vector<Point> sides;
};

#endif // PATHFINDING_H_
