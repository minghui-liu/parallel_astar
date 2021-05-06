#include "worker.decl.h"

#include "worker.h"
#include "main.decl.h"


extern /* readonly */ CProxy_Main mainProxy;


Worker::Worker() {
  // Nothing to do when the Worker chare object is created.
  //   This is where member variables would be initialized
  //   just like in a C++ class constructor.
  G = new Graph((char*)"../test_graphs/nodes.out", (char*)"../test_graphs/edges.out");
  CkPrintf("Chare %d: G->nodes size = %d\n", thisIndex, G->nodes.size());
  dst_found = false;
  in_barrier_mode = false;
  numElements = 5;
  
  src = 92577;
  dst = 51670;

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

void Worker::hdastar(int s, int d) {

  // // Have this chare object say hello to the user.
  // CkPrintf("\"Hello\" from Worker chare # %d on "
  //          "processor %d , step %d).\n",
  //          thisIndex, CkMyPe(), step);


  while (!open_list.empty()) {

    // Step 2a : process open list
    OpenListMember front = open_list.top();
    open_list.pop();
    float h = std::get<0>(front);
    float proposed_shortest_dist = std::get<1>(front);
    Node * curr_node = std::get<2>(front);
    //std::cout << "Popped " << curr_node->id << std::endl;
    CkPrintf("Chare %d: Popped %d\n", thisIndex, curr_node->id);
    Node * proposed_parent = std::get<3>(front);

    if (proposed_shortest_dist > curr_node->latest_shortest_distance_in_open_list) // outdated entry
      continue;

    if (curr_node->closed) {
      if (curr_node->id == src) continue;
      if (proposed_shortest_dist > curr_node->f) continue;
      // this means a new shorter path to a closed node is found
    }
    curr_node->parent = proposed_parent;
    curr_node->f = proposed_shortest_dist;
    curr_node->g = h - proposed_shortest_dist;
    curr_node->open = false;
    curr_node->closed = true;

    if (curr_node->id == dst) {
      CkPrintf("Destination found..\n");
      CkPrintf("Distance = %f", curr_node->f);
      // std::cout << "Destination found.." << std::endl;
      // std::cout << "Distance = " << curr_node->f << std::endl;
      if (!dst_found)  {
        dst_found = true;
        // MPI_Ibcast(&dst_rcv, 1, MPI_INT, dst_rank, MPI_COMM_WORLD, &dst_req);
      }
      continue;
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
        open_list.push(std::make_tuple(h_, dist, neighbour, curr_node));
      }
    }



    

    // if ( doneCondition() ) {
    //   // Report to the Main chare object that this chare object
    //   //   has completed its task.
    //   mainProxy.done();
    // }
  }



}

void Worker::receiveNode(float h, float dist, int node, int parent) {
  CkPrintf("Chare %d: received node %d, parent %d, h=%f, dist=%f\n", node, parent, h, dist);
  Node * neighbour = &G->nodes[node];
  Node * curr_node = &G->nodes[parent];
  if (neighbour->open) { //already in open list
    if (dist > neighbour->latest_shortest_distance_in_open_list) //no need to add this top open list
      return;
  }
  neighbour->open=true;
  neighbour->latest_shortest_distance_in_open_list = dist;
  open_list.push(std::make_tuple(h, dist, neighbour, curr_node));

  hdastar(0, 0);
}

// void Worker::send_message_set() {
//   int is_complete;
//   for (int i = 0; i < message_set.size(); i++) {
//     if (i == thisIndex)
//       continue;
//     for (int j = 0; j < message_set[i].size(); j++) {
//       thisProxy[i].receiveNode();
//     }
//   }
//   num_sends += 1;
// }


int Worker::hash(Node &n) {
  return n.id % numElements;
}

// struct msg Worker::create_msg(float h_, float dist_, int node_, int parent_) {
//   struct msg msg_;
//   msg_.h = h_;
//   msg_.dist = dist_;
//   msg_.node = node_;
//   msg_.parent = parent_;
//   return msg_;
// }


#include "worker.def.h"
