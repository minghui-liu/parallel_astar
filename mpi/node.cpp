#include "astar.hpp"
#include <algorithm>

using namespace std::placeholders;
using namespace AStar;

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

