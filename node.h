#ifndef _SP_NODE_H_
#define _SP_NODE_H_
#include <vector>

struct Node {
    int x;
    int y;
    float g;
    float h;
    
    Node *parent;
    std::vector<Node*> neighbors;

    Node() :x(0), y(0), g(0), h(0), parent(nullptr) {}
    Node(int ax, int ay) : x(ax), y(ay), g(0), h(0) {} 
    float f() const { return g + h; }
    const bool operator==(const Node &other) { return (x == other.x && y == other.y); }
    const bool operator>(const Node &other) { return f() > other.f(); }
    const bool operator<(const Node &other) { return f() < other.f(); }
};

#endif
