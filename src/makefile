CC = mpic++
SRCS = vec2i.cpp node.cpp graph.cpp
OBJS = vec2i.o node.o graph.o
HPPS = astar.hpp
CCFLAGS = -std=c++17

all: seq mpi

mpi: $(HPPS) $(OBJS)
	$(CC) $(CCFLAGS) astar_mpi.cpp $(OBJS) -o astar_mpi

seq: $(HPPS) $(OBJS)
	$(CC) $(CCFLAGS) astar_seq.cpp $(OBJS) -o astar_seq

$(OBJS) : %.o : %.cpp $(HPPS)
	$(CC) $(CCFLAGS) -c $< -o $@

clean:
	rm *.o astar_mpi astar_seq