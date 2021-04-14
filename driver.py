import graph
import dijkstra
import sequential
import random

src = random.randrange(1000)
dest = random.randrange(1000)
print("Source node: ", src)
print("Destination node: ", dest)

G = graph.knn_graph(1000)
d1, p1 = dijkstra.dijkstra(G, 1000, src, dest)
d2, p2 = sequential.sequential_astar(G, 1000, src, dest)
print("Dijkstra's: ", d1, p1)
print("A* Search: ", d2, p2)
assert(d1 == d2)
assert(p1 == p2)