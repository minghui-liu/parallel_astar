
# Dijkstra's algorithm for comparison and correctness check
def dijkstra(G, N, src, dest):
    """
    Dijkstra's shortest path algorithm
    Parameters:
        G: networkx graph
        N: number of nodes
        src: source node id
        dest: destination id
    Return:
        (cost, path)
    """
    unvisited = set(range(N))
    dist = [float('inf') for i in range(N)]
    dist[src] = 0
    prev = [None for i in range(N)]

    while len(unvisited) > 0:
        _, curr = min([(dist[i], i) for i in unvisited])
        unvisited.remove(curr)
        if curr == dest:
            path = []
            while curr:
                path.insert(0, curr)
                curr = prev[curr]
            return (dist[dest], path)
        for nbr, attr in G.adj[curr].items():
            alt = dist[curr] + attr['weight']
            if alt < dist[nbr]:
                dist[nbr] = alt
                prev[nbr] = curr
    return (-1, [])