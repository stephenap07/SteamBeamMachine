#ifndef PATHFINDING_H_
#define PATHFINDING_H_

#include <list>
#include "TileMap.hpp"
#include "SFMLDebugDraw.h"

struct Node {
    int x, y;
    float g, h;
    Node *parent;

    Node() :x(-1), y(-1), g(0), h(0), parent(nullptr) {}
    Node(int _x, int _y) :x(_x), y(_y), g(0), h(0), parent(nullptr) {}

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

enum class EdgeType {
    WALK,
    JUMP
};

struct Edge {
    Edge() :n1(nullptr), n2(nullptr), edgeType(EdgeType::WALK), jumpPower(0.0f) {}
    Edge(Node *a, Node *b) :n1(a), n2(b), edgeType(EdgeType::WALK), jumpPower(0.0f) {}
    Node *n1;
    Node *n2;
    EdgeType edgeType;
    float jumpPower;

    const bool operator==(const Edge &other) const
    {
        return (*other.n1 == *n1 && *other.n2 == *n2) || (*other.n1 == *n2 && *other.n2 == *n1);
    }
};

class Pathfinder {
    typedef std::list<Node*>::iterator NodeIter;
    struct Point {
        int x, y;
    };

public:
    Pathfinder() :m_tileMap(nullptr)
    {
        init();
    }
    Pathfinder(TileMap *tileMap) :m_tileMap(tileMap)
    {
        init();
    }
    void setMap(TileMap *tileMap){
        m_tileMap = tileMap;
    }
    std::vector<Node> getPath(Node start, Node goal);

private:
    void init()
    {
        m_mass = 0.476074f;
        m_velocity = 5.0f;
        m_gravity = 40.0f;
        m_jump = 3.0f*m_gravity;

        initSides();
    }
    int manhattanDistance(Node start, Node goal);
    void setNodes(Node goal);
    std::vector<Node*> identifySuccessors(const Node *current, Node *start, Node *goal);
    std::vector<Node*> nodeNeighbors(const Node *node);
    Node *jump(int x, int y, int dX, int dY, Node *start, Node *goal);
    Node *nextNode(const std::list<Node*> &nodeList);
    Node *getPoolNode(int x, int y);
    void initSides();
    void preprocessGrid(Node goal);

    bool canWalkBetween(Node &current, Node &other);
    bool canJumpBetween(Node &current, Node &other);
    float getJumpCost(Node &current, Node &other);

private:
    const TileMap *m_tileMap;
    std::vector<Node> m_nodes;
    std::vector<Edge> m_edges;
    std::vector<Point> sides;

    float m_mass;
    float m_velocity;
    float m_gravity;
    float m_jump;
};

#endif // PATHFINDING_H_
