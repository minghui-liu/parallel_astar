from graph_utils import knn_graph, rdisc_graph, save_graph
import time

seed = 12
canvas_dim = 400

K_list = [4, 8, 16, 32, 64, 75]
for k in K_list:
    tic = time.perf_counter()
    G, pos = knn_graph(1000000, seed=seed, canvas_dim=canvas_dim, K=k)
    toc = time.perf_counter()
    print("Graph generation time - ", 1000 * (toc - tic), " ms")
    save_graph(G, "./test_graphs-" + str(k))
