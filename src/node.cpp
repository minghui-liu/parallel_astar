#include "astar.hpp"
#include <algorithm>


using namespace std::placeholders;
using namespace AStar;

Node::Node(Vec2i coord_, int id_)
{
    id = id_;
    coordinates.x  = coord_.x;
    coordinates.y  = coord_.y;
}

void Node::print_info()
{
    std::cout << "Node id " << id << " x: " << coordinates.x << " y: " << coordinates.y << std::endl;
}

void Node::add_neighbours(std::vector<Node> &neighbours_)
{
    neighbours = neighbours_;
}

void Node::print_neighbours()
{
    std::cout << "Neighbours of " << id << " : ";
    for (Node node : neighbours)
        std::cout << node.id << " ";
    std::cout << std::endl;
}