import random
import math
import heapq
import networkx as nx
import numpy as np
import matplotlib.pyplot as plt
import matplotlib
matplotlib.use("Agg")

def l2(a, b):
    """
    l2 norm function
        a, b: a coordinate 2-tuple of the form (x, y)
    """
    return math.sqrt((a[0]-b[0])**2 + (a[1]-b[1])**2)


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
    kNN = []
    for i in range(N):
        pq = [] # priority queue
        for j in range(N):
            if i == j: # skip myself
                continue
            # note heapq is a min heap implementation (always pops smallest element)
            heapq.heappush(pq, (-l2(coordinates[i], coordinates[j]), j)) # use l2 norm
            if len(pq) > K:
                heapq.heappop(pq)
        kNN.append(pq)
    assert(K == len(kNN[0]))

    # construct graph acoording to kNN
    G = nx.Graph()
    # add nodes
    for i in range(N):
        G.add_node(i, x=coordinates[i][0], y=coordinates[i][1])
    assert(G.number_of_nodes() == N)
    # add edges
    for i in range(N):
        for neighbor in kNN[i]:
            G.add_edge(i, neighbor[1], weight=-1.0*neighbor[0]) # neighbor[0] is l2 distance, neighbor[1] is node number
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
            if i == j: # skip myself
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
            G.add_edge(i, neighbor[1], weight=-1.0*neighbor[0]) # neighbor[0] is l2 distance, neighbor[1] is node number
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

