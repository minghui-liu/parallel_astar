#include "astar.hpp"
#include <algorithm>
#include <chrono>

using namespace std::placeholders;
using namespace AStar;
using namespace std::chrono;

int main()
{
    Graph G = Graph((char*)"../test_graphs/nodes.out", (char*)"../test_graphs/edges.out");
    
    auto start = high_resolution_clock::now();
    G.astar_seq(102577, 51670);
    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(stop - start);
    G.show_path(51670);

    
    std::cout << "Time taken:- " << duration.count() << " ms" << std::endl;

    //G.print_info();

    return 0;
}
