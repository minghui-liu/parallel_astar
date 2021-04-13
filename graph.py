import random
import math
import heapq
import networkx as nx


def l2(a, b):
    """
    l2 norm function
        a, b: a coordinate 2-tuple of the form (x, y)
    """
    return math.sqrt((a[0]-b[0])**2 + (a[1]-b[1])**2)


def knn_graph(N, K=-1, seed=None):
    """
    construct an undirected graph using kNN
    Parameters:
        N: number of nodes
        K: number of nearest neighbors, default = 2*e*log(N)
        seed: a random seed integer
    Return:
        An undirected networkx graph
    """
    if seed == None:
        random.seed()
    else:
        random.seed(seed)
    
    if K < 0:
        K = math.ceil(2 * math.e * math.log(N))
    
    coordinates = [(random.random(), random.random()) for i in range(N)]
    kNN = []
    for i in range(N):
        pq = [] # priority queue
        for j in range(N):
            if i == j: # skip myself
                continue
            # note heapq is a min heap implementation (always pops smallest element)
            heapq.heappush(pq, (-1.0 * l2(coordinates[i], coordinates[j]), j)) # use l2 norm
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
    
    return G
