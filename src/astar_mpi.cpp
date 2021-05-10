#include "astar.hpp"
#include <algorithm>
#include <chrono>
#include "mpi.h"

using namespace std::placeholders;
using namespace AStar;
using namespace std::chrono;

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    Graph G = Graph(argv[1], argv[2]);

    FILE *fp = fopen(argv[3],"r");
    int src, dst;
    while(fscanf(fp, "%d %d", &src, &dst) != EOF){};
    fclose(fp);
    
    auto start = high_resolution_clock::now();
    G.astar_mpi(src, dst);
    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(stop - start);
    //G.show_path(4613);

    
    std::cout << "Time taken:- " << duration.count() << " ms" << std::endl;

    //G.print_info();
    MPI_Finalize();
    return 0;
}
