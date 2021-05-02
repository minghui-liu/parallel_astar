#include "astar.hpp"
#include <algorithm>


using namespace std::placeholders;
using namespace AStar;


int main()
{
    //struct Vec2i coord = {.x=0.25, .y=0.35};
    //std::cout << coord.x << " " << coord.y << std::endl;

    Node node1 = Node(Vec2i(10.0, 100.0), 5);
    Node node2 = Node(Vec2i(10.0, 100.0), 50);
    node1.print_info();
    node2.print_info();

    std::vector<Node> neighbours_1 = {node2};
    std::vector<Node> neighbours_2 = {node1};

    node1.add_neighbours(neighbours_1);
    node2.add_neighbours(neighbours_2);

    node1.print_neighbours();
    node2.print_neighbours();

    return 0;
}
