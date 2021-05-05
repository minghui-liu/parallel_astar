import random
import math
import heapq
import networkx as nx
import numpy as np
import matplotlib.pyplot as plt
import matplotlib
import os
from sklearn.neighbors import NearestNeighbors 

matplotlib.use("Agg")

def l2(a, b):
    """
    l2 norm function
        a, b: a coordinate 2-tuple of the form (x, y)
    """
    return math.sqrt((a[0]-b[0])**2 + (a[1]-b[1])**2)


def make_l2_heuristic(G):
    def l2_heuristic(n1, n2):
        return l2((G.nodes[n1]['x'], G.nodes[n1]['y']), (G.nodes[n2]['x'], G.nodes[n2]['y']))
    return l2_heuristic

def make_sorting_fn(G):
    def sort_fn(node):
        return G.nodes[node]['x'] + G.nodes[node]['y']
    return sort_fn 
    
def knn_graph(N, K=None, seed=None, canvas_dim=40):
    """
    construct an undirected graph using kNN
    Parameters:
        N: number of nodes
        K: number of nearest neighbors, default = 2*e*log(N)
        seed: a random seed integer
    Return:
        An undirected networkx graph
    """
    if seed:
        np.random.seed(seed)
    
    if not K:
        K = math.ceil(2 * math.e * math.log(N))
    
    coordinates = np.random.uniform(low=-canvas_dim/2, high=canvas_dim/2, size=(N,2))
    # kNN = []
    # for i in range(N):
    #     pq = [] # priority queue
    #     for j in range(N):
    #         if i == j: # skip myself
    #             continue
    #         # note heapq is a min heap implementation (always pops smallest element)
    #         heapq.heappush(pq, (-l2(coordinates[i], coordinates[j]), j)) # use l2 norm
    #         if len(pq) > K:
    #             heapq.heappop(pq)
    #     kNN.append(pq)
    # assert(K == len(kNN[0]))
    nbrs = NearestNeighbors(n_neighbors=K+1, algorithm='kd_tree').fit(coordinates)
    distances, indices = nbrs.kneighbors(coordinates)

    # construct graph acoording to kNN
    G = nx.Graph()
    # add nodes
    for i in range(N):
        G.add_node(i, x=coordinates[i][0], y=coordinates[i][1])
    assert(G.number_of_nodes() == N)
    # add edges
    for i in range(N):
        for neighbor,dist in zip(indices[i][1:], distances[i][1:]):
            G.add_edge(i, neighbor, weight=dist) # neighbor[0] is l2 distance, neighbor[1] is node number
    pos = {}
    for i, coord in enumerate(coordinates):
        pos[i] = tuple(coord) 
    return G, pos

def rdisc_graph(N, R=None, seed=None, canvas_dim=40, unit_vol=4):
    """
    construct an undirected graph using kNN
    Parameters:
        N: number of nodes
        R: Radius of disc, default = 2*e*log(N)
        seed: a random seed integer
    Return:
        An undirected networkx graph
    """
    if seed:
        np.random.seed(seed)
    
    if not R:
        gamma = 2*np.sqrt(1.5*canvas_dim*canvas_dim/unit_vol)
        R = gamma * np.sqrt(math.log(N)/N)
    
    coordinates = np.random.uniform(low=-canvas_dim/2, high=canvas_dim/2, size=(N,2))
    neighbours= []
    for i in range(N):
        qualified_nodes = [] # priority queue
        for j in range(N):
            if j==i: # avoid duplicate edges and self loops
                continue
            # note heapq is a min heap implementation (always pops smallest element)
            dist = l2(coordinates[i], coordinates[j]) 
            if dist <= R:
                qualified_nodes.append((dist, j))
        
        neighbours.append(qualified_nodes)

    # construct graph acoording to kNN
    G = nx.Graph()
    # add nodes
    for i in range(N):
        G.add_node(i, x=coordinates[i][0], y=coordinates[i][1])
    assert(G.number_of_nodes() == N)
    # add edges
    for i in range(N):
        for neighbor in neighbours[i]:
            G.add_edge(i, neighbor[1], weight=1.0*neighbor[0]) # neighbor[0] is l2 distance, neighbor[1] is node number
    pos = {}
    for i, coord in enumerate(coordinates):
        pos[i] = tuple(coord) 
    return G, pos


def visualize(G, pos, savefile):
    ### make visualizations better
    f = plt.figure()
    nx.draw_networkx_nodes(G, pos, node_size=20)
    nx.draw_networkx_edges(G, pos, width=0.2)
    # nx.draw_spring(G, ax=f.add_subplot(111))
    f.savefig(savefile)

def visualize_path(G, pos, path, savefile):
    ### make visualizations better
    f = plt.figure()
    node_colours = ['#1f78b4'] * G.number_of_nodes() #default node colour
    node_sizes = [20] * G.number_of_nodes() #default node colour
    for node in G.nodes():
        if node in path:
            node_colours[node] = (1,1,0)
            node_sizes[node] = 100

    node_colours[path[0]] = (1,0,0)
    node_colours[path[-1]] = (0,1,0)

    nx.draw_networkx_nodes(G, pos, node_size=node_sizes, node_color=node_colours)
    
    shortest_path_edges = [(path[i], path[i+1]) for i in range(len(path)-1)] + [(path[i+1], path[i]) for i in range(len(path)-1)]
    edge_colours = ['k'] * G.number_of_edges()
    widths = [0.2] * G.number_of_edges()
    for i,edge in enumerate(G.edges()):
        if edge in shortest_path_edges:
            edge_colours[i] = (0,0,0) 
            widths[i] = 1

    nx.draw_networkx_edges(G, pos, width=widths, edge_color=edge_colours)
    # nx.draw_spring(G, ax=f.add_subplot(111))
    f.savefig(savefile)

def path_length(G, path):
    length = 0
    for i in range(len(path)-1):
        u, v = path[i], path[i+1]
        length += G.edges[u,v]['weight']
    return length

def save_graph(G, path):
    os.makedirs(path, exist_ok=True)

    with open(os.path.join(path,"nodes.out"), "w") as f:
        number_of_nodes = G.number_of_nodes()
        for i in range(number_of_nodes):
            f.write(f"{i} {G.nodes[i]['x']} {G.nodes[i]['y']}")
            f.write("\n")

    with open(os.path.join(path,"edges.out"), "w") as f:
        for edge in G.edges():
            weight = G.edges[edge]['weight']
            f.write(f"{edge[0]} {edge[1]} {weight}")
            f.write("\n")



