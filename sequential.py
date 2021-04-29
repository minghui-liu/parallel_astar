import random
import heapq
import math
from graph_utils import knn_graph, rdisc_graph, visualize, make_l2_heuristic, l2, visualize_path, make_sorting_fn
import networkx as nx
# import networkx.algorithms.shortest_paths.astar.astar_path as astar_path

def reconstruct_path(prev, curr):
    path = [curr]
    while prev[curr]:
        path.insert(0, prev[curr])
        curr = prev[curr]
    return path

# Sequential A* Search
def sequential_astar(G, N, src, dest, heur_func=l2):
    """
    A sequential implementation of A* search algorithm
    Parameters:
        G: the networkx graph
        N: number of nodes
        src: source node id
        dest: destination node id
        H: heuristic function, parameters (a, b) where a and b are coordinates in 2-tuple form
    Return:
        (cost, path)
    """
    openSet = [(0, src)]
    heapq.heapify(openSet)
    closedSet = set()
    costToCome = [float('inf') if i != src else 0 for i in range(N)]
    prev = [None for i in range(N)]

    while len(openSet) > 0:
        _, curr = heapq.heappop(openSet)
        if curr == dest:
            return (costToCome[curr], reconstruct_path(prev, curr))
            break
        if curr in closedSet:
            continue
        closedSet.add(curr)
        for nbr, attr in G.adj[curr].items():
            if nbr in closedSet:
                continue
            gcost = costToCome[curr] + attr['weight']
            fcost = gcost + heur_func((G.nodes[nbr]['x'],G.nodes[nbr]['y']), (G.nodes[dest]['x'],G.nodes[dest]['y']))
            if gcost > costToCome[nbr]:
                continue
            costToCome[nbr] = gcost
            prev[nbr] = curr
            heapq.heappush(openSet, (fcost, nbr))

if __name__ == "__main__":
    #G, pos = knn_graph(100, seed=42)
    G, pos = rdisc_graph(100, R=8, seed=35)
    visualize(G, pos, "sample_graph.png")
    nodes = list(G.nodes())
    nodes.sort(key=make_sorting_fn(G))
    path = nx.algorithms.shortest_paths.astar.astar_path(G, nodes[0], nodes[-1], heuristic=make_l2_heuristic(G))
    visualize_path(G, pos, path, "sample_path.png")
    print(path)