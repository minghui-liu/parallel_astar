CC = g++
SRCS = vec2i.cpp node.cpp graph.cpp
OBJS = vec2i.o node.o graph.o
HPPS = astar.hpp
CCFLAGS = -std=c++17

all: $(HPPS) $(OBJS)
	$(CC) $(CCFLAGS) astar.cpp $(OBJS)

$(OBJS) : %.o : %.cpp $(HPPS)
	$(CC) $(CCFLAGS) -c $< -o $@

clean:
	rm *.o a.out