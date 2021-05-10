# Parallel Hash Distributed A* Algorithm
Course Project for CMSC714 : High Performance Computing Systems at University of Maryland, College Park

By Minghui Liu, Onur Cankur and Siddharth Singh

## Build

### Dependencies
First, the dependencies should be installed to build the project. In this project, `gcc` is required for all implementations, `openmpi/3.1.5/gcc/` is required for MPI and Charm++ implementations, and `charmpp` is required only for Charm++ implementation.


## Makefiles
After having all the required dependencies explained above, you can compile the code using makefiles.

Now, you should go to the directory that you can find the corresponding makefile: you should `cd` into `/src` directory for sequential and/or MPI implementation and you should `cd` into `/charmpp` directory for Charm++ implementation. Then, type

```make```

This should create `astar_seq` for sequential implementation, `astar_mpi` for MPI implementation, and `astar` for Charm++ implementation. If you want to remove these executables you can run

```make clean```

## Run
Before running the code, you need to generate a graph using `generate_graph.py` file. That program will generate a graph and store the graph in `test_graphs-<K>` directory (`test_graphs-4` if K=4) that stores `edges.out`, `nodes.out`, and `srd_dst.out` files.

For the sequential and MPI programs, you need to give `nodes.out`, `edges.out`, and `src_dst.out` files to the program as arguments.

### Submit
You can create your submit script (```sbatch submit.sh```) and you can run the following commands to run the programs:

MPI: ```mpirun -np <number_of_procs> ./astar_mpi <nodes_file> <edges_file> <src_dst_file>```

Sequential: ```./astar_seq <nodes_file> <edges_file> <src_dst_file>```

For Charm++ implementation, you can just generate the graph and run the code without arguments:

```mpirun -np <number_of_procs> ./astar```


### Mass Submit
You can have your own `.sh` scripts or use our mass submit scripts to run the code. `mass-mpi.sh` and `mass-charmpp.sh` create submit scripts to run the program on 1, 2, 4, 8, 16, 32 processes. `mass-seq.sh` is implemented to the run program on graphs created with different K values. We used that script for our experiments but it can be adjusted by updating the for loop in line 33 in `mass-seg.sh`. For example, you can change that line to `for K in 4 8` if you generated two graphs: `test_graphs-4` and `test_graphs-8`.


# Plots
