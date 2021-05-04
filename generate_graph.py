from graph_utils import knn_graph, rdisc_graph, save_graph

seed=42
canvas_dim=400
G,_ = knn_graph(100000, seed=seed, canvas_dim=canvas_dim, K=4)
save_graph(G, "./test_graphs")