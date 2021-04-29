import random
import heapq
import math
from graph_utils import knn_graph, rdisc_graph, visualize
import networkx as nx


def reconstruct_path(prev, curr):
    path = [curr]
    while prev[curr]:
        path.insert(0, prev[curr])
        curr = prev[curr]
    return path

def l2_heuristic(curr, dest):
    return math.sqrt((curr[0]-dest[0])**2 + (curr[1]-dest[1])**2)

# Sequential A* Search
def sequential_astar(G, N, src, dest, H):
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
            fcost = gcost + l2_heuristic((G.nodes[nbr]['x'],G.nodes[nbr]['y']), (G.nodes[dest]['x'],G.nodes[dest]['y']))
            if gcost > costToCome[nbr]:
                continue
            costToCome[nbr] = gcost
            prev[nbr] = curr
            heapq.heappush(openSet, (fcost, nbr))

if __name__ == "__main__":
    #G, pos = knn_graph(100, seed=42)
    G, pos = rdisc_graph(100)
    visualize(G,pos,"sample.png")