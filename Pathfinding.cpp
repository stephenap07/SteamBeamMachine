#include <list>
#include <vector>
#include <cmath>
#include <iostream>

#include "Pathfinding.hpp"

template <typename T> static T clamp(T value, T min, T max) {
  if (value < min) {
    return min;
  } else if (value > max) {
    return max;
  }
  return value;
}

void Pathfinder::initSides() {
  sides = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1},
           {0, 1}, {0, -1}, {1, 0},  {-1, 0}};
}

int Pathfinder::manhattanDistance(Node start, Node goal) {
  return std::abs(goal.x - start.x) + std::abs(goal.y - start.y);
}

bool Pathfinder::canWalkBetween(Node &current, Node &other) {
  const int distanceX = current.x - other.x;
  const int distanceY = current.y - other.y;

  return (std::abs(distanceX) == 1 && distanceY == 0);
}

bool Pathfinder::canJumpBetween(Node &current, Node &other) {
  const int distanceX = current.x - other.x;
  const int distanceY = current.y - other.y;
  const int deltaX = clamp(distanceX, -1, 1);
  const int deltaY = clamp(distanceY, -1, 1);

  return false;
}

float Pathfinder::getJumpCost(Node &current, Node &other) { return 100.0f; }

void Pathfinder::preprocessGrid(Node goal) {
  setNodes(goal);
  for (Node &current : m_nodes) {
    for (Node &other : m_nodes) {
      if (other == current)
        continue;
      Edge e(&current, &other);

      if (canWalkBetween(current, other)) {
        e.edgeType = EdgeType::WALK;
      } else if (canJumpBetween(current, other)) {
        e.edgeType = EdgeType::JUMP;
      } else {
        continue;
      }

      auto foundEdge = std::find(m_edges.begin(), m_edges.end(), e);
      if (foundEdge != m_edges.end())
        continue;
      m_edges.push_back(e);
    }
  }
}

std::vector<Node *> Pathfinder::nodeNeighbors(const Node *node) {
  std::vector<Node *> result;
  if (!node->parent) {
    for (int i = 0; i < 8; i++) {
      int x = node->x + sides[i].x;
      int y = node->y + sides[i].y;
      if (!m_tileMap->isBlocked(x, y)) {
        result.push_back(getPoolNode(x, y));
      }
    }
    return result;
  }

  int dX = std::min(std::max(-1, node->x - node->parent->x), 1);
  int dY = std::min(std::max(-1, node->y - node->parent->y), 1);

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
  } else if (dX == 0) {
    if (!m_tileMap->isBlocked(node->x, node->y + dY)) {
      result.push_back(getPoolNode(node->x, node->y + dY));
      if (m_tileMap->isBlocked(node->x + 1, node->y)) {
        result.push_back(getPoolNode(node->x + 1, node->y + dY));
      }
      if (m_tileMap->isBlocked(node->x - 1, node->y)) {
        result.push_back(getPoolNode(node->x - 1, node->y + dY));
      }
    }
  } else {
    if (!m_tileMap->isBlocked(node->x + dX, node->y)) {
      result.push_back(getPoolNode(node->x + dX, node->y));
      if (m_tileMap->isBlocked(node->x, node->y + 1)) {
        result.push_back(getPoolNode(node->x + dX, node->y + 1));
      }
      if (m_tileMap->isBlocked(node->x, node->y - 1)) {
        result.push_back(getPoolNode(node->x + dX, node->y - 1));
      }
    }
  }

  return result;
}

std::vector<Node> Pathfinder::getPath(Node start, Node goal) {
  if (m_tileMap == nullptr) {
    std::cerr << "Error, map must not be null\n";
    return std::vector<Node>();
  }

  std::list<Node *> openSet;
  std::list<Node *> closedSet;
  setNodes(goal);

  Node *pStart = getPoolNode(start.x, start.y);
  Node *pGoal = getPoolNode(goal.x, goal.y);

  openSet.push_back(&(*pStart));

  while (!openSet.empty()) {
    Node *current = nextNode(openSet);
    openSet.remove(current);

    if (*current == goal) {
      break;
    }

    auto successors = identifySuccessors(current, pStart, pGoal);

    for (Node *neighbor : successors) {
      NodeIter find_closed =
          std::find(closedSet.begin(), closedSet.end(), neighbor);
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
  result.push_back(*pGoal);
  Node *p = pGoal->parent;
  while (p) {
    result.push_back(*p);
    p = p->parent;
  }

  std::reverse(result.begin(), result.end());
  return result;
}

std::vector<Node *> Pathfinder::identifySuccessors(const Node *current,
                                                   Node *start, Node *goal) {
  std::vector<Node *> successors;
  std::vector<Node *> neighbors = nodeNeighbors(current);

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
Node *Pathfinder::jump(int cX, int cY, int dX, int dY, Node *start,
                       Node *goal) {
  // Position of new node we are going to consider:
  int nextX = cX + dX;
  int nextY = cY + dY;

  Node *result = nullptr;

  // If it's blocked we can't jump here
  if (m_tileMap->isBlocked(nextX, nextY))
    return result;

  // If the node is the goal return it
  if (nextX == goal->x && nextY == goal->y) {
    return getPoolNode(goal->x, goal->y);
  }

  int offsetX = nextX;
  int offsetY = nextY;

  // Diagonal Case
  if (dX != 0 && dY != 0) {
    while (true) {
      if ((!m_tileMap->isBlocked(offsetX - dX, offsetY + dY) &&
           m_tileMap->isBlocked(offsetX - dX, offsetY)) ||
          (!m_tileMap->isBlocked(offsetX + dX, offsetY - dY) &&
           m_tileMap->isBlocked(offsetX, offsetY - dY))) {
        return getPoolNode(offsetX, offsetY);
      }

      // Check in horizontal and vertical directions for forced neighbors
      // This is a special case for diagonal direction
      if (jump(offsetX, offsetY, dX, 0, start, goal) ||
          jump(offsetX, offsetY, 0, dY, start, goal)) {
        return getPoolNode(nextX, nextY);
      }
      offsetX += dX;
      offsetY += dY;
      if (m_tileMap->isBlocked(offsetX, offsetY)) {
        return nullptr;
      }
      if (offsetX == goal->x && offsetY == goal->y) {
        return getPoolNode(offsetX, offsetY);
      }
    }
  } else {
    if (dX != 0) {
      while (true) {
        if ((!m_tileMap->isBlocked(offsetX + dX, nextY + 1) &&
             m_tileMap->isBlocked(offsetX, nextY + 1)) ||
            (!m_tileMap->isBlocked(offsetX + dX, nextY - 1) &&
             m_tileMap->isBlocked(offsetX, offsetY - 1))) {
          return getPoolNode(offsetX, offsetY);
        }

        offsetX += dX;
        if (m_tileMap->isBlocked(offsetX, offsetY)) {
          return nullptr;
        }
        if (offsetX == goal->x && nextY == goal->y) {
          return getPoolNode(offsetX, nextY);
        }
      }
    } else {
      while (true) {
        if ((!m_tileMap->isBlocked(nextX + 1, offsetY + dY) &&
             m_tileMap->isBlocked(nextX + 1, offsetY)) ||
            (!m_tileMap->isBlocked(nextX - 1, offsetY + dY) &&
             m_tileMap->isBlocked(nextX - 1, offsetY))) {
          return getPoolNode(nextX, offsetY);
        }
        offsetY += dY;
        if (m_tileMap->isBlocked(nextX, offsetY)) {
          return nullptr;
        }
        if (nextX == goal->x && offsetY == goal->y) {
          return getPoolNode(nextX, offsetY);
        }
      }
    }
  }

  // If forced neighbor was not found try next jump point
  return jump(nextX, nextY, dX, dY, start, goal);
}

Node *Pathfinder::nextNode(const std::list<Node *> &nodeList) {
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

void Pathfinder::setNodes(Node goal) {
  m_nodes.clear();
  for (int y = 0; y < m_tileMap->getHeight(); y++) {
    for (int x = 0; x < m_tileMap->getWidth(); x++) {
      Node node(x, y);
      node.h = manhattanDistance(node, goal);
      m_nodes.push_back(node);
    }
  }
}

Node *Pathfinder::getPoolNode(int x, int y) {
  return &m_nodes[y * m_tileMap->getWidth() + x];
}
