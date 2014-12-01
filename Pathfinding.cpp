#include <list>
#include <vector>
#include <cmath>

#include "Pathfinding.hpp"

static bool NodeCompare(const Node &lhs, const Node &rhs)
{
    return lhs.f() > rhs.f();
}

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
    std::vector<Node> result;

    return result;
}

std::vector<Node> Pathfinder::identifySuccessors(Node current, Node start, Node goal)
{
    std::vector<Node> successors;
    std::vector<Node> neighbors;
    
    for (auto &neighbor : neighbors) {
        // Direction from current node to neighbor:
        int dX = clamp(neighbor.x - current.x, -1, 1);
        int dY = clamp(neighbor.y - current.y, -1, 1);
 
        // Try to find a node to jump to:
        Node jumpPoint = jump(current.x, current.y, dX, dY, start, goal);
 
        // If found add it to the list:
        if (jumpPoint.isValid())
            successors.push_back(jumpPoint);
    }
    
    return successors;
}


// cX, cY - Current Node Position,  dX, dY - Direction
Node Pathfinder::jump(int cX, int cY, int dX, int dY, const Node &start, const Node &goal)
{
    // Position of new node we are going to consider:
    int nextX = cX + dX;
    int nextY = cY + dY;

    Node result;
    
    // If it's blocked we can't jump here
    if (m_tileMap->isBlocked(nextX, nextY))
        return result;
 
    // If the node is the goal return it
    if (nextX == goal.x && nextY == goal.y) {
        result.set(nextX, nextY);
        return result;
    }
 
    // Diagonal Case  
    if (dX != 0 && dY != 0) {
        if (true/*... Diagonal Forced Neighbor Check ...*/) {
            //return Node.pooledNode(nextX, nextY);
        }
        
        // Check in horizontal and vertical directions for forced neighbors
        // This is a special case for diagonal direction
        if (jump(nextX, nextY, dX, 0, start, goal).isValid() ||
            jump(nextX, nextY, 0, dY, start, goal).isValid())
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

