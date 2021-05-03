#include "astar.hpp"
#include <algorithm>


using namespace std::placeholders;
using namespace AStar;


int main()
{
    Graph G = Graph((char*)"../test_graphs/nodes.out", (char*)"../test_graphs/edges.out");

    G.astar(2263,4613);
    G.show_path(4613);
    //G.print_info();


    return 0;
}
