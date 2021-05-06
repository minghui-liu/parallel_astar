#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <algorithm>
#include <map>
#include <math.h>
#include <stack>
#include <vector>
#include <functional>
#include <set>
#include <iostream>
#include <queue>
#include <tuple>

using namespace std::placeholders;


class Vec2i
{
public:
    float x, y;
    Vec2i(float x_, float y_);
    Vec2i();

};

Vec2i::Vec2i(float x_, float y_)
{
    x = x_;
    y = y_;
}

Vec2i::Vec2i()
{
    x = 0;
    y = 0;
}


struct Node
{
    Vec2i coordinates;
    std::vector<std::pair<Node*,float> > neighbours;
    int id;
    float g; // heuristic
    float f; //distance from source
    float latest_shortest_distance_in_open_list; //to filter outdated entries in open list
    
    bool closed; // if its in open list
    bool open; // if its in closed list
    Node * parent; // who is the parent
    

    Node(Vec2i coord_, int id_);
    void print_info();
    //void add_neighbours(std::vector<Node> &neighbours_);
    //void add_weights(std::vector<float> &edge_weights_);
    void add_neighbour(Node &neighbour, float edge_weight);
    void print_neighbours();
    uint getScore();
};

Node::Node(Vec2i coord_, int id_)
{
    id = id_;
    coordinates.x  = coord_.x;
    coordinates.y  = coord_.y;
    closed = false;
    open = false;
    parent = NULL;
    
}

void Node::print_info()
{
    std::cout << "Node id " << id << " x: " << coordinates.x << " y: " << coordinates.y << std::endl;
}

void Node::add_neighbour(Node &neighbour, float edge_weight)
{
    neighbours.push_back(std::make_pair(&neighbour, edge_weight));
}

void Node::print_neighbours()
{
    std::cout << "Number of Neighbours of " << id << " are : ";
    std::cout << neighbours.size() << std::endl;
    for (auto node : neighbours)
        std::cout << node.first->id << " " << node.second << std::endl;
    std::cout << std::endl;
}


using HeuristicFunction = std::function<uint(Vec2i, Vec2i)>;
using OpenListMember = std::tuple<float, float, Node*, Node*>; // h, proposed_shortest_distance, ThisNode, ProposedParent

class myComparator
{
    public:
        int operator() (const OpenListMember& p1, const OpenListMember& p2)
        {
            return std::get<0>(p1) > std::get<0>(p2);
        }
};

struct Graph
{
    std::vector<Node> nodes;
    Graph(char* node_addr, char* edges_addr);
    void print_info();
    void show_path(int dst);
    float heuristic(Node &n, Node &dst);
};

Graph::Graph(char* node_addr, char* edges_addr)
{
    FILE *fp = fopen(node_addr,"r");
    int node_id;
    float x,y;
    while(fscanf(fp, "%d %f %f", &node_id, &x, &y) != EOF){
        //std::cout << node_id << " " << x << " " << y << std::endl;
        //nodes_to_coord[node_id] = Vec2i(x,y);
        nodes.emplace_back(Vec2i(x,y), node_id);
    }
    fclose(fp);

    fp = fopen(edges_addr, "r");
    int n1,n2;
    float edge_weight;

    while(fscanf(fp, "%d %d %f", &n1, &n2, &edge_weight) != EOF){
        nodes[n1].add_neighbour(nodes[n2], edge_weight);
        nodes[n2].add_neighbour(nodes[n1], edge_weight);
    }

    fclose(fp);
}

void Graph::print_info()
{
    for (auto node : nodes)
    {
        node.print_neighbours();
        std::cout << "====" << std::endl;
    }
}

float Graph::heuristic(Node &n, Node &dst)
{
    float x1 = n.coordinates.x, y1 = n.coordinates.y;
    float x2 = dst.coordinates.x, y2 = dst.coordinates.y;

    return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}

void Graph::show_path(int dst)
{
    std::stack<int> path;
    path.push(dst);
    Node * curr = &nodes[dst];
    std::cout << "Shortest Path Length is " << curr->f << std::endl;
    while(curr->parent){
        curr = curr->parent;
        path.push(curr->id);
    }
    while (!path.empty()) {
        std::cout << " " << path.top();
        path.pop();
    }
    std::cout << std::endl;
}

struct msg {
    float h;
    float dist;
    int node;
    int parent;
};


#endif //__ASTAR_H__
