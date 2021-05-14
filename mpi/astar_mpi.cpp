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

    std::cout << "Reading Graph..." << std::endl;
    Graph G = Graph(argv[1], argv[2]);

    FILE *fp = fopen(argv[3],"r");
    int src, dst;
    while(fscanf(fp, "%d %d", &src, &dst) != EOF){};
    fclose(fp);
  
    std::cout << "Read Graph..." << std::endl;

    

    std::cout << "Starting Astar..." << std::endl;
    auto start = high_resolution_clock::now();
    G.astar_mpi(src, dst);
    auto stop = high_resolution_clock::now();
    std::cout << "Ending Astar..." << std::endl;

    auto duration = duration_cast<milliseconds>(stop - start);
    //G.show_path(4613);

    
    std::cout << "Time taken:- " << duration.count() << " ms" << std::endl;

    //G.print_info();
    MPI_Finalize();
    return 0;
}
