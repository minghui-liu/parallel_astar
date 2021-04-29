from mpi4py import MPI
from heapq import heappush, heappop
from itertools import count
from collections import defaultdict
import networkx as nx
import numpy as np

from graph_utils import knn_graph, rdisc_graph, visualize, make_l2_heuristic, l2, visualize_path, make_sorting_fn, path_length

from external.networkx_astar import astar_path

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
world_size = comm.Get_size()

push = heappush
pop = heappop

def hash_fn(x):
    # we should look at more hash functions
    return x % world_size 


class ParallelAStar:
    def __init__(self, G, source, target, heuristic=None, weight='weight'):
        self.G = G
        self.source = source 
        self.target = target 
        self.heuristic = heuristic 
        self.weight = weight 
        
        if source not in G or target not in G:
            msg = 'Either source {} or target {} is not in G'
            raise nx.NodeNotFound(msg.format(source, target))
        
        if heuristic is None:
            # The default heuristic is h=0 - same as Dijkstra's algorithm
            def heuristic(u, v):
                return 0
            self.heuristic = heuristic 

        self.enqueued = {} # closed list - (distance from source, heuristic) 
        self.explored = {} # closed list - (parent)
        self.queue = [] # open list
        self.c = count()

        if rank == hash_fn(source):
            self.queue = [(0, next(self.c), source, 0, None)]

        self.iter = 0
        self.running = True
        self.termination_future = None  

    def single_iteration(self):
        assert self.queue, "there should be something in the open list"        
        
        messages = defaultdict(list)

        # Pop the smallest item from queue.
        _, __, curnode, dist, parent = pop(self.queue)


        if curnode in self.explored:
            # Do not override the parent of starting node
            if self.explored[curnode] is None:
                return messages 

            # Skip bad paths that were enqueued before finding a better one
            qcost, h = self.enqueued[curnode]
            if qcost < dist:
                return messages

        self.explored[curnode] = parent
        
        if curnode == self.target: 
            print(f"rank {rank} target found")
            return messages


        for neighbor, w in self.G[curnode].items():
            ncost = dist + w[self.weight]
            owner = hash_fn(neighbor)
            if rank == owner: 
                if neighbor in self.enqueued:
                    qcost, h = self.enqueued[neighbor]
                    # if qcost <= ncost, a less costly path from the
                    # neighbor to the source was already determined.
                    # Therefore, we won't attempt to push this neighbor
                    # to the queue
                    if qcost <= ncost:
                        continue
                else:
                    h = self.heuristic(neighbor, self.target)
                self.enqueued[neighbor] = ncost, h
                push(self.queue, (ncost + h, next(self.c), neighbor, ncost, curnode))

            else:
                h = self.heuristic(neighbor, self.target)
                messages[owner].append([neighbor, ncost, h, curnode])

        return messages 

    def exchange_messages(self, messages):
        rcvd_msgs = []
        for sender in range(world_size):
            if sender == rank:
                # send your stuff
                for rcvr in range(world_size):
                    if rcvr == rank:
                        continue
                    comm.send(messages[rcvr], dest=rcvr, tag=self.iter)
            else:
                rcvd_msgs += comm.recv(source=sender, tag=self.iter)
        return rcvd_msgs 

    def update_queues(self, messages):
        for (neighbor, ncost, h, curnode) in messages:
            if neighbor in self.enqueued:
                qcost, h = self.enqueued[neighbor]
                # if qcost <= ncost, a less costly path from the
                # neighbor to the source was already determined.
                # Therefore, we won't attempt to push this neighbor
                # to the queue
                if qcost <= ncost:
                    continue
            else:
                h = self.heuristic(neighbor, self.target)
            self.enqueued[neighbor] = ncost, h
            push(self.queue, (ncost + h, next(self.c), neighbor, ncost, curnode))            

    def check_termination(self):
        queue_len_send = np.array([len(self.queue)], dtype=np.int)
        queue_len_recv = np.zeros(1, dtype=np.int)
        comm.Allreduce([queue_len_send, MPI.INT], [queue_len_recv, MPI.INT])
        return queue_len_recv[0] == 0

    def run_astar(self):
        while True:
            # if rank==0:
            #     print(f"Iter number {self.iter}")
            if self.running:
                if self.queue:
                    # if rank==0:
                    #     print("here-1")
                    msgs = self.single_iteration()
                else:
                    # if rank==0:
                    #     print("here-2")
                    self.running = False 
                    self.termination_future = comm.Ibarrier()
                    msgs = defaultdict(list)
            else:
                if self.termination_future.Test():
                    print(f"rank {rank} - testing termination")
                    if self.check_termination():
                        if rank==0:
                            print("here-3")
                        print(f"rank{rank} Termination condition reached")
                        return  
                    else:
                        # if rank==0:
                        #     print("here-4")
                        self.running = True 
                        self.termination_future = None 
                        msgs = defaultdict(list)
                #     if rank==0:
                #         print("here-5")
                # if rank==0:
                #     print("here-6")

            rcvd_msgs = self.exchange_messages(msgs)
            self.update_queues(rcvd_msgs)
            self.iter += 1

    def retrace_path_fragment(self, node):
        assert node in self.explored, "there's some annoying bug"
        path_fragment = []
        while (node is not None) and (node in self.explored):
            parent = self.explored[node]
            if (parent is not None):
                path_fragment.append(parent)
            node = parent
        return path_fragment

    def retrace_path(self):
        total_path = [self.target]
        curr_node = self.target
        bcast_buffer = np.zeros((1), dtype=np.int)

        while True:
            #CHECK TERMINATION
            if curr_node == self.source:
                break 
            owner = hash_fn(curr_node)
            
            if rank == owner: 
                path_fragment = self.retrace_path_fragment(curr_node)
                print(f"Retracing path  for node {curr_node} in rank {rank}, fragment {path_fragment}")
                #print(self.explored)
                if rank == 0:
                    total_path += path_fragment
                else:
                    comm.send(path_fragment, 0, self.iter)
                bcast_buffer[0] = path_fragment[-1]
                comm.Bcast([bcast_buffer, MPI.INT], root=owner)
            else:
                if rank == 0:
                    total_path += comm.recv(source=owner, tag=self.iter)
                comm.Bcast([bcast_buffer, MPI.INT], root=owner)
            curr_node = bcast_buffer[0]
            self.iter += 1

        return total_path[::-1]


if __name__ == "__main__":
    seed = 32
    G, pos = knn_graph(1000, seed=seed)
    nodes = list(G.nodes())
    nodes.sort(key=make_sorting_fn(G))
    src, dst = nodes[0], nodes[-1]
    print(src, dst)
    astar = ParallelAStar(G, src, dst, make_l2_heuristic(G))
    astar.run_astar()
    path = astar.retrace_path()
    comm.Barrier()
    if rank == 0:
        print("Shortest Path is:")
        print(path)
        visualize(G, pos, "sample_graph.png")
        visualize_path(G, pos, path, "sample_path_mpi.png")
        print("Shortest Path Length is:")
        print(path_length(G, path))

        print("Shortest path length found by sequential algorithm:")
        seq_path = astar_path(G, src, dst, make_l2_heuristic(G))
        print(path_length(G, seq_path))
        visualize_path(G, pos, seq_path, "sample_path_seq.png")
    


    

