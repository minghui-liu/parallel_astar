#include "worker.decl.h"

#include "worker.h"
#include "main.decl.h"


extern /* readonly */ CProxy_Main mainProxy;


Worker::Worker(int src, int dst, int numElements) {
  // Nothing to do when the Worker chare object is created.
  //   This is where member variables would be initialized
  //   just like in a C++ class constructor.
  G = new Graph((char*)"../test_graphs-4/nodes.out", (char*)"../test_graphs-4/edges.out");
  CkPrintf("Chare %d: G->nodes size = %d\n", thisIndex, G->nodes.size());
  dst_found = false;
  in_barrier_mode = false;
  this->numElements = numElements;

  this->src = src;
  this->dst = dst; 

   if (hash(G->nodes[src]) == thisIndex) {
        CkPrintf("Chare %d: I have src\n", thisIndex);
        float src_heuristic = G->heuristic(G->nodes[src], G->nodes[dst]);
        open_list.push(std::make_tuple(src_heuristic, 0.0, &(G->nodes[src]), nullptr));
        G->nodes[src].open = true;
        G->nodes[src].latest_shortest_distance_in_open_list = 0;
   }

  // int dst_rank = hash(G->nodes[dst]);
}



// Constructor needed for chare object migration (ignore for now)
// NOTE: This constructor does not need to appear in the ".ci" file
Worker::Worker(CkMigrateMessage *msg) { }

Worker::~Worker() {
  delete G;
}

void Worker::next_iter(){
  if (!open_list.empty()) {
    thisProxy[thisIndex].hdastar();
  } else if (dst_found) {
    mainProxy.done();
  }

}

void Worker::hdastar() {

  if (!open_list.empty()) {

    // Step 2a : process open list
    OpenListMember front = open_list.top();
    open_list.pop();
    float h = std::get<0>(front);
    float proposed_shortest_dist = std::get<1>(front);
    Node * curr_node = std::get<2>(front);
    //std::cout << "Popped " << curr_node->id << std::endl;
    CkPrintf("Chare %d: Popped %d\n", thisIndex, curr_node->id);
    Node * proposed_parent = std::get<3>(front);

    if (proposed_shortest_dist > curr_node->latest_shortest_distance_in_open_list) { // outdated entry
        next_iter();
        return;
    }

    if (curr_node->closed) {
      if (curr_node->id == src) {
          next_iter();
          return;
      }
      if (proposed_shortest_dist > curr_node->f) {
          next_iter();
          return;
      }
      // this means a new shorter path to a closed node is found
    }
    curr_node->parent = proposed_parent;
    curr_node->f = proposed_shortest_dist;
    curr_node->g = h - proposed_shortest_dist;
    curr_node->open = false;
    curr_node->closed = true;

    if (curr_node->id == dst) {
      CkPrintf("Chare %d: Destination found .. Distance = %f\n", thisIndex, curr_node->f);
      if (!dst_found) {
        dst_found = true;
      }
      mainProxy.dstFound();
      next_iter();
      return;
    }

    // expand current node
    for (auto edge : curr_node->neighbours) {
      Node * neighbour = edge.first;
      float dist = curr_node->f + edge.second;
      int neighbour_rank = hash(G->nodes[neighbour->id]);
      CkPrintf("Chare %d: expanding %d, neighbour %d, belogs to chare %d\n", thisIndex, curr_node->id, neighbour->id, neighbour_rank);
      // if (neighbour->open) //already in open list
      // {
      //     if (dist > neighbour->latest_shortest_distance_in_open_list) //no need to add this top open list
      //         continue;
      // }
      float h_ = dist + G->heuristic(*neighbour, G->nodes[dst]);
      //neighbour->open=true;
      //neighbour->latest_shortest_distance_in_open_list = dist; 
      //open_list.push(std::make_tuple(h_, dist, neighbour, curr_node));
  // message_set[hash(G->nodes[neighbour->id])].push_back(create_msg(h_, dist, neighbour->id, curr_node->id));
      //std::cout << "Rank " << rank << " Pushed msg for " <<hash(nodes[neighbour->id]) << " Node " << neighbour->id  << std::endl;
      if (neighbour_rank != thisIndex) {
        thisProxy[neighbour_rank].receiveNode(h_, dist, neighbour->id, curr_node->id);
      } else {
        if (neighbour->open) { //already in open list
          if (dist > neighbour->latest_shortest_distance_in_open_list) //no need to add this top open list
            continue;
        }
        neighbour->open=true;
        neighbour->latest_shortest_distance_in_open_list = dist;
        open_list.push(std::make_tuple(h_, dist, neighbour, curr_node));
      }
    }

  }
  next_iter();

  return;
}


void Worker::receiveNode(float h, float dist, int node, int parent) {
  CkPrintf("Chare %d: received node %d, parent %d, h=%f, dist=%f\n", thisIndex,node, parent, h, dist);
  bool enqueue = open_list.empty();
  Node * neighbour = &G->nodes[node];
  Node * curr_node = &G->nodes[parent];
  if (neighbour->open) { //already in open list
    if (dist > neighbour->latest_shortest_distance_in_open_list) //no need to add this top open list
      return;
  }
  neighbour->open=true;
  neighbour->latest_shortest_distance_in_open_list = dist;
  open_list.push(std::make_tuple(h, dist, neighbour, curr_node));

  if (enqueue)
    thisProxy[thisIndex].hdastar();
}


void Worker::setDstFound() {
  if (!dst_found) {
    dst_found = true;
  }
}

void Worker::reportOpenListSize() {
  int size = open_list.size();
  contribute(sizeof(size), &size, CkReduction::sum_int);
}

int Worker::hash(Node &n) {
  return n.id % numElements;
}


#include "worker.def.h"
