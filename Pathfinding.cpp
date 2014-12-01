#include <list>
#include <vector>
#include <cmath>
#include <iostream>

#include "Pathfinding.hpp"

template<typename T>
static T clamp(T value, T min, T max)
{
    if(value < min) {
        return min;
    } else if(value > max) {
        return max;
    }
    return value;
}

std::vector<Node> Pathfinder::getPath(Node start, Node goal)
{
    if (m_tileMap == nullptr) {
        std::cerr << "Error, map must not be null\n";
        return std::vector<Node>();
    }

    std::list<Node*> openSet;
    std::list<Node*> closedSet;
    setNodes(goal);

    auto startIt= std::find(m_nodes.begin(), m_nodes.end(), start);
    Node *pStart = nullptr;
    if (startIt != m_nodes.end())
        pStart = &(*startIt);
    else {
        std::cerr << "Error, start node not found\n";
        return std::vector<Node>();
    }

    Node *pGoal = nullptr;

    openSet.push_back(&(*pStart));

    while (!openSet.empty()) {
        Node *current = nextNode(openSet);
        openSet.remove(current);

        if (*current == goal) {
            pGoal = current;
            break;
        }

        auto successors = identifySuccessors(current, pStart, pGoal);

        for (Node *neighbor : successors) {
            NodeIter find_closed = std::find(closedSet.begin(), closedSet.end(), neighbor);
            if (find_closed != closedSet.end()) {
                continue; 
            }
            float tentative_g = current->g + current->distanceTo(*neighbor);

            NodeIter find_open = std::find(openSet.begin(), openSet.end(), neighbor);
            if (find_open == openSet.end() || tentative_g < neighbor->g) {
                neighbor->parent = current;
                neighbor->g = tentative_g;
                neighbor->h = manhattanDistance(*neighbor, goal);
                openSet.push_back(neighbor);
            } else if ((*find_open)->f() > (tentative_g + neighbor->h)) {
                neighbor->parent = current;
                neighbor->g = tentative_g;
                neighbor->h = manhattanDistance(*neighbor, goal);
                openSet.push_back(neighbor);
            }
        }
        closedSet.push_back(current); 
    }

    std::vector<Node> result;
    Node *p = pGoal->parent;
    while (p) {
        result.push_back(*p);
        p = p->parent;
    }

    return result;
}

std::vector<Node*> Pathfinder::identifySuccessors(const Node *current, Node *start, Node *goal)
{
    std::vector<Node*> successors;
    std::vector<Node*> neighbors;
    
    for (auto neighbor : neighbors) {
        // Direction from current node to neighbor:
        int dX = clamp(neighbor->x - current->x, -1, 1);
        int dY = clamp(neighbor->y - current->y, -1, 1);
 
        // Try to find a node to jump to:
        Node *jumpPoint = jump(current->x, current->y, dX, dY, start, goal);
 
        // If found add it to the list:
        if (jumpPoint)
            successors.push_back(jumpPoint);
    }
    
    return successors;
}


// cX, cY - Current Node Position,  dX, dY - Direction
Node *Pathfinder::jump(int cX, int cY, int dX, int dY, Node *start, Node *goal)
{
    // Position of new node we are going to consider:
    int nextX = cX + dX;
    int nextY = cY + dY;

    Node *result = nullptr;
    
    // If it's blocked we can't jump here
    if (m_tileMap->isBlocked(nextX, nextY))
        return result;
 
    // If the node is the goal return it
    if (nextX == goal->x && nextY == goal->y) {
        result = goal;
        return result;
    }
 
    // Diagonal Case  
    if (dX != 0 && dY != 0) {
        if (true/*... Diagonal Forced Neighbor Check ...*/) {
            //return Node.pooledNode(nextX, nextY);
        }
        
        // Check in horizontal and vertical directions for forced neighbors
        // This is a special case for diagonal direction
        if (jump(nextX, nextY, dX, 0, start, goal) ||
            jump(nextX, nextY, 0, dY, start, goal))
        {
            //return Node.pooledNode(nextX, nextY);
        }
    } else {
        // Horizontal case
        if (dX != 0) {
            if (true/*... Horizontal Forced Neighbor Check ...*/) {
                //return Node.pooledNode(nextX, nextY);
            }
        /// Vertical case
        } else {
            if (true/*... Vertical Forced Neighbor Check ...*/) {
                //return Node.pooledNode(nextX, nextY);
            }
        }
    }
    
    /// If forced neighbor was not found try next jump point
    return jump(nextX, nextY, dX, dY, start, goal);
}

Node *Pathfinder::nextNode(const std::list<Node*> &nodeList)
{
    if (nodeList.empty())
        return nullptr;

    Node *lowest = nullptr;
    for (auto node : nodeList) {
        if (lowest == nullptr) {
            lowest = node;
            continue;
        } else if (node->f() < lowest->f()) {
            lowest = node;
        }
    }

    return lowest;
}

std::vector<Node*> Pathfinder::nodeNeighbors(const Node *node)
{
    std::vector<Node*> result;
    if (!node->parent) {
        for (int i = 0; i < 8; i++) {
            int x = node->x + sides[i].x;
            int y = node->y + sides[i].y;
            if (!m_tileMap->isBlocked(x, y)) {
                result.push_back(&m_nodes[y*m_tileMap->getWidth() + x]);
            }
        }
        return result;
    }

    int dX = clamp(node->x - node->parent->x, -1, 1);
    int dY = clamp(node->y - node->parent->y, -1, 1);
    if (dX != 0 && dY != 0) {
        if (!m_tileMap->isBlocked(node->x, node->y + dY)) {
            result.push_back(getPoolNode(node->x, node->y + dY));
        }
        if (!m_tileMap->isBlocked(node->x + dX, node->y)) {
            result.push_back(getPoolNode(node->x + dX, node->y));
        }
        if (!m_tileMap->isBlocked(node->x, node->y + dY) ||
            !m_tileMap->isBlocked(node->x + dX, node->y)) {
            result.push_back(getPoolNode(node->x + dX, node->y + dY));
        }
        if (m_tileMap->isBlocked(node->x - dX, node->y) &&
            !m_tileMap->isBlocked(node->x, node->y + dY)) {
            result.push_back(getPoolNode(node->x - dX, node->y + dY));
        }
        if (m_tileMap->isBlocked(node->x, node->y - dY) &&
            !m_tileMap->isBlocked(node->x + dX, node->y)) {
            result.push_back(getPoolNode(node->x + dX, node->y - dY));
        }
    }

    return result;
}

void Pathfinder::initSides()
{
    sides = {
        {  1,  1 },
        {  1, -1 },
        { -1,  1 },
        { -1, -1 },
        {  0,  1 },
        {  0, -1 },
        {  1,  0 },
        { -1,  0 }
    };
}

void Pathfinder::setNodes(Node goal)
{
    m_nodes.clear();
    for (int y = 0; y < m_tileMap->getHeight(); y++) {
        for (int x = 0; x < m_tileMap->getWidth(); x++) {
            Node node(x, y);
            node.h = manhattanDistance(node, goal);
            m_nodes.push_back(node);
        }
    }
}

Node *Pathfinder::getPoolNode(int x, int y)
{
    return &m_nodes[y*m_tileMap->getWidth() + x];
}
