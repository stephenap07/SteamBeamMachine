#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>
#include <queue>
#include <algorithm>
#include "node.h"
#include "geometry.h"

struct Polygon {
    std::vector<Node> nodes;
};

struct Edge {
    Node *p1;
    Node *p2;
};

Node g_start;
Node g_goal;

struct Map {
    std::vector<Node*> nodes;
    std::vector<Polygon> polygons;
    std::vector<Edge> edges;
};

float Distance(const Node &node_a, const Node &node_b);
void CalculateEdges(std::vector<Polygon> &polygons, std::vector<Edge> &edges);
void NeighborCalculation(Map &map);
void Heuristic(Map &map);
bool NodeCompare(const Node &lhs, const Node &rhs)
{
    return lhs.f() > rhs.f();
}

Map ReadFile(const char *filename)
{
    std::string line;
    std::ifstream myfile;
    myfile.open(filename);
    Map map;

    if (myfile.is_open()) {
        int total_lines = 0;

        while (getline(myfile, line)) {
            total_lines++;
            std::stringstream ss(line);
            bool is_shape = false;

            if (total_lines > 2) {
                is_shape = true;
            }

            int x = 0;
            int y = 0;

            int value = 0;
            int total = 0;
            Polygon shape;
            bool x_done = false;
            bool y_done = false;

            while (ss >> value) {
                if (ss.peek() == ',' || ss.peek() == ';') {
                    ss.ignore();
                }

                total++;
                if (total % 2 == 1) {
                    x = value;
                    x_done = true;
                } else if (total % 2 == 0) {
                    y = value;
                    y_done = true;
                } 

                if (x_done && y_done) {
                    if (!is_shape) {
                        if (total_lines == 1) {
                            g_start = Node(x, y);
                        } else if (total_lines == 2) {
                            g_goal = Node(x, y);
                        }
                    } else {
                        shape.nodes.push_back(Node(x, y));
                    }

                    x = 0;
                    y = 0;
                    x_done = false;
                    y_done = false;
                }
            }

            if (is_shape) {
                map.polygons.push_back(shape);
            }
        }
        myfile.close();
    }

    map.nodes.push_back(&g_start);
    map.nodes.push_back(&g_goal);

    for (Polygon &polygon : map.polygons) {
        for (Node &node : polygon.nodes) {
            map.nodes.push_back(&node);
        }
    }

    CalculateEdges(map.polygons, map.edges);
    NeighborCalculation(map);
    Heuristic(map);

    return map;
}

float Distance(const Node &node_a, const Node &node_b) 
{
   return sqrt((float)pow(node_a.y - node_b.y, 2) + (float)pow(node_a.x - node_b.x, 2)); 
}

void CalculateEdges(std::vector<Polygon> &polygons, std::vector<Edge> &edges)
{
     for (Polygon &p : polygons) {
        // Assuming that a polygon has at least 3 vertices
        for (unsigned i = 1; i < p.nodes.size(); i++) {
            Node *p1 = &p.nodes[i - 1];
            Node *p2 = &p.nodes[i];
            edges.push_back(Edge{p1, p2});

            if (i != p.nodes.size() - 1) {
                Node *q1 = &p.nodes[i - 1];
                Node *q2 = &p.nodes[i + 1];
                edges.push_back(Edge{q1, q2});
            }
        }
    }
}

void NeighborCalculation(Map &map)
{
    // N^2
    for (Node *current_node : map.nodes) {
        for (Node *other_node : map.nodes) {
            if (*other_node == *current_node) {
                continue;
            }

            bool has_intersection = false;
            for (const Edge &e : map.edges) {
                if (*e.p1 == *current_node || *e.p2 == *current_node ||
                    *e.p1 == *other_node || *e.p2 == *other_node) {
                    continue;
                }

                if (doIntersect(*current_node, *other_node, *e.p1, *e.p2)) {
                    has_intersection = true;
                    break;
                }
            }

            if (!has_intersection) {
                current_node->neighbors.push_back(other_node);
            }
        }
    }
}

void Heuristic(Map &map)
{
    for (Node *n : map.nodes) {
        n->g = Distance(*n, g_goal);
    }
}

void A_Star(Node *start, Node *goal)
{
    
}

Node *NextNode(std::list<Node*> &node_list)
{
    Node *lowest;
    bool is_set = false;
    for (std::list<Node*>::iterator it = node_list.begin(); it != node_list.end(); ++it) {
        if (is_set) {
            if ((*it)->f() < lowest->f()) {
                lowest = *it;
            }
        } else {
            lowest = *it;
            is_set = true;
        }
    }
    return lowest;
}

int DoSolve(int argc, char**argv)
{
    Map map;
    if (argc < 2) {
        std::cerr << "Missing map file\n";
        exit(1);
    } else {
        map = ReadFile(argv[1]);
        if (map.polygons.empty() && map.polygons.size() >= 2) {
            std::cerr << "Error reading file\n";
            exit(1);
        }
    }

    typedef std::list<Node*>::iterator node_iter;

    for (Node *n : map.nodes) {
        n->parent = nullptr;
    }

    std::cout << "Shortest path from (" << g_start.x << ", " << g_start.y << ") to (" << g_goal.x << ", " << g_goal.y << ")" << std::endl;

    std::list<Node*> open_set;
    std::list<Node*> closed_set;

    open_set.push_back(&g_start);
    while (!open_set.empty()) {
        Node *current = NextNode(open_set);
        open_set.remove(current);
        if (*current == g_goal) {
            break;
        }
        for (Node *neighbor : current->neighbors) {
            node_iter find_closed = std::find(closed_set.begin(), closed_set.end(), neighbor);
            if (find_closed != closed_set.end()) {
                continue; 
            }
            float tentative_g = current->g + Distance(*current, *neighbor);

            node_iter find_open = std::find(open_set.begin(), open_set.end(), neighbor);
            if (find_open == open_set.end()) {
                neighbor->parent = current;
                neighbor->g = tentative_g;
                open_set.push_back(neighbor);
            } else if ((*find_open)->f() > (tentative_g + neighbor->h)) {
                neighbor->parent = current;
                neighbor->g = tentative_g;
                open_set.push_back(neighbor);
            }
        }
        closed_set.push_back(current); 
    }

    Node *p = g_goal.parent;
    std::vector<Node> solution;
    while (p) {
        solution.push_back(*p);
        p = p->parent;
    }

    for(std::vector<Node>::reverse_iterator it = solution.rbegin(); it != solution.rend(); ++it)
    {
        std::cout << "(" << it->x << ", " << it->y << ") ";
    }
    std::cout << std::endl;

    return 0;
}
