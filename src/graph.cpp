#include "astar.hpp"
#include <algorithm>
#include <map>
#include <math.h>
#include <stack>
#include <stddef.h>


using namespace std::placeholders;
using namespace AStar;

Graph::Graph(char* node_addr, char* edges_addr)
{
    FILE *fp = fopen(node_addr,"r");
    int node_id;
    float x,y;
    while(fscanf(fp, "%d %f %f", &node_id, &x, &y) != EOF){
        //std::cout << node_id << " " << x << " " << y << std::endl;
        //nodes_to_coord[node_id] = Vec2i(x,y);
        nodes.emplace_back(Vec2i(x,y), node_id);
    }
    fclose(fp);

    fp = fopen(edges_addr, "r");
    int n1,n2;
    float edge_weight;

    while(fscanf(fp, "%d %d %f", &n1, &n2, &edge_weight) != EOF){
        nodes[n1].add_neighbour(nodes[n2], edge_weight);
        nodes[n2].add_neighbour(nodes[n1], edge_weight);
    }
    tag = 0;
    num_sends = 0;
    dst_found = false;
    fclose(fp);
  
}

void Graph::print_info()
{
    for (auto node : nodes)
    {
        node.print_neighbours();
        std::cout << "====" << std::endl;
    }
}

float Graph::heuristic(Node &n, Node &dst)
{
    float x1 = n.coordinates.x, y1 = n.coordinates.y;
    float x2 = dst.coordinates.x, y2 = dst.coordinates.y;

    return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}

void Graph::show_path(int dst)
{
    std::stack<int> path;
    path.push(dst);
    Node * curr = &nodes[dst];
    while(curr->parent){
        curr = curr->parent;
        path.push(curr->id);
    }
    while (!path.empty()) {
        std::cout << " " << path.top();
        path.pop();
    }
    std::cout << std::endl;
}

void Graph::astar_seq(int src, int dst)
{
    //std::cout << heuristic(nodes[src], nodes[dst]) << std::endl;
    float src_heuristic = heuristic(nodes[src], nodes[dst]);
    open_list.push(std::make_tuple(src_heuristic, 0.0, &nodes[src], nullptr));
    nodes[src].open=true;
    //nodes[src].f = 0;
    //nodes[src].g = src_heuristic;
    //nodes[src].parent = NULL;
    nodes[src].latest_shortest_distance_in_open_list = 0;
    //std::pair<float, Node*> temp;

    while (!open_list.empty())
    {
        OpenListMember front = open_list.top();
        open_list.pop();
        Node * curr_node = std::get<2>(front);
        //std::cout << "Popped " << curr_node->id << std::endl;
        Node * proposed_parent = std::get<3>(front);
        float h = std::get<0>(front);
        float proposed_shortest_dist = std::get<1>(front);
        
        if (proposed_shortest_dist > curr_node->latest_shortest_distance_in_open_list) //outdated entry
            continue;


        if (curr_node->closed)
        {
            if (curr_node->id == src)continue;
            if (proposed_shortest_dist > curr_node->f)continue;
            // this means a new shorter path to a closed node is found  
        }
        curr_node->parent = proposed_parent;
        curr_node->f = proposed_shortest_dist;
        curr_node->g = h - proposed_shortest_dist;
        curr_node->open = false;
        curr_node->closed = true;

        if (curr_node->id == dst)
            return; 

        for (auto edge : curr_node->neighbours)
        {
            Node * neighbour = edge.first;
            float dist = curr_node-> f + edge.second;
            if (neighbour->open) //already in open list
            {
                if (dist > neighbour->latest_shortest_distance_in_open_list) //no need to add this top open list
                    continue;
            }
            float h_ = dist + heuristic(*neighbour, nodes[dst]);
            neighbour->open=true;
            neighbour->latest_shortest_distance_in_open_list = dist; 
            open_list.push(std::make_tuple(h_, dist, neighbour, curr_node));
        }
    }
}

int Graph::hash(Node &n)
{
    return n.id % world_size;
}

void Graph::create_msg_mpi_datatype()
{
    const int nitems=4;
    int blocklengths[4] = {1,1,1,1};
    MPI_Datatype types[4] = {MPI_FLOAT, MPI_FLOAT, MPI_INT, MPI_INT};
    MPI_Aint offsets[4] = {0, sizeof(float), 2*sizeof(float), 2*sizeof(float)+sizeof(int)};


    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_msg_type);
    MPI_Type_commit(&mpi_msg_type);
}

struct msg Graph::create_msg(float h_, float dist_, int node_, int parent_)
{
    struct msg msg_;
    msg_.h = h_;
    msg_.dist = dist_;
    msg_.node = node_;
    msg_.parent = parent_;
    return msg_;
}

void Graph::clear_message_set()
{
    for(int i=0; i<message_set.size(); i++)
        message_set[i].clear();
}

void Graph::send_message_set()
{
    int is_complete;
    for(int i=0; i<message_set.size(); i++)
    {
        if(i==rank)
            continue;
        if(send_requests[i] != nullptr) //check if previous send is pending
        {
            MPI_Test(send_requests[i], &is_complete, MPI_STATUS_IGNORE);
            if(!is_complete)continue; //send is still executing
            send_buffers[i].clear(); //empty send buffer
            send_requests[i] = nullptr;
        }
        if( !(message_set[i].empty()) )
        {
            send_requests[i] = new MPI_Request;
            send_buffers[i].assign(message_set[i].begin(), message_set[i].end()); //copy data into send buffer   
            MPI_Isend(&send_buffers[i][0], message_set[i].size(), mpi_msg_type, i, tag, MPI_COMM_WORLD, send_requests[i]);
            tag += 1;
            message_set[i].clear(); //clear data
        }
    }
    num_sends += 1;
}

int Graph::receive_message_set()
{
    int flag, size;
    MPI_Status status;
    MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
    int buf_size=0;
    
    while(flag)
    {
        MPI_Get_count(&status, mpi_msg_type, &size);
        //receive_buffer
        //MPI_Request req;    
        MPI_Recv(recv_buffer+buf_size, size, mpi_msg_type, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        //MPI_Wait(&req, MPI_STATUS_IGNORE);
        //std::cout << "Rank " << rank << " Received message of size " << size << " Tag " <<status.MPI_TAG << " vector size " << receive_buffer.size() << std::endl;
        buf_size+=size;
        MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
    }
    return buf_size;
}

void Graph::add_msgs_to_open_list(int num_msgs_recvd)
{
    // receive_buffer
    // message_set
    Node * neighbour, * curr_node;
    float dist, h_;
    msg msg_;
  
    for(int i=0; i<num_msgs_recvd; i++)
    {
        // if (rank==0)
        //     std::cout << "rank" << rank << " hereeee1" << std::endl;
        msg_ = recv_buffer[i];
        neighbour = &nodes[msg_.node];
        curr_node = &nodes[msg_.parent];
        // if (rank==0)
        //     std::cout << "rank" << rank << " hereeee2" << std::endl;
        // if(rank==0)
        //     std::cout << "Parent : " << msg_.node << "Child : " << msg_.parent <<  " Heuristic: " << msg_.h << " Dist: " << msg_.dist << std::endl;
        // if (rank==0)
        //     std::cout << "=============================" << std::endl;
        dist = msg_.dist;
        // if (rank==0)
        //     std::cout << "=============================" << std::endl;
        // if (rank==0)
        //     std::cout << "rank" << rank << " hereeee3" << std::endl;
        if (neighbour->open) //already in open list
        {
            if (dist > neighbour->latest_shortest_distance_in_open_list) //no need to add this top open list
                continue;
        }
        h_ = msg_.h;
        neighbour->open=true;
        neighbour->latest_shortest_distance_in_open_list = dist; 
        // if (rank==0)
        //     std::cout << "rank" << rank << " hereeee4" << std::endl;
        open_list.push(std::make_tuple(h_, dist, neighbour, curr_node));
    }
    // if (rank==0)
    //     std::cout << "rank" << rank << "hereeee100" << std::endl;
    for(int i=0; i<message_set[rank].size(); i++)
    {
        msg_ = message_set[rank][i];
        neighbour = &nodes[msg_.node];
        curr_node = &nodes[msg_.parent];
        dist = msg_.dist;
        if (neighbour->open) //already in open list
        {
            if (dist > neighbour->latest_shortest_distance_in_open_list) //no need to add this top open list
                continue;
        }
        h_ = msg_.h;
        neighbour->open=true;
        neighbour->latest_shortest_distance_in_open_list = dist; 
        
        open_list.push(std::make_tuple(h_, dist, neighbour, curr_node));
    }
    message_set[rank].clear();
   
    //float dist = curr_node->f + edge.second;
    // if (neighbour->open) //already in open list
    // {
    //     if (dist > neighbour->latest_shortest_distance_in_open_list) //no need to add this top open list
    //         continue;
    // }
    //float h_ = dist + heuristic(*neighbour, nodes[dst]);
    //neighbour->open=true;
    //neighbour->latest_shortest_distance_in_open_list = dist; 
    //open_list.push(std::make_tuple(h_, dist, neighbour, curr_node));
    //message_set[hash(nodes[neighbour->id])].push_back(msg(h_, dist, neighbour->id, curr_node->id));
    //std::cout << "Rank " << rank << " Pushed msg for " <<hash(nodes[neighbour->id]) << " Node " << neighbour->id  << std::endl;

}

void Graph::astar_mpi(int src, int dst)
{
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    if(hash(nodes[src])==rank)
    {
        float src_heuristic = heuristic(nodes[src], nodes[dst]);
        open_list.push(std::make_tuple(src_heuristic, 0.0, &nodes[src], nullptr));
        nodes[src].open=true;
        nodes[src].latest_shortest_distance_in_open_list = 0;
    }

    int dst_rank = hash(nodes[dst]), dst_rcv, dst_flag=0, barrier_flag=0;
    MPI_Request dst_req, barrier_req;
     
    if(dst_rank!=rank)
    {
        //in this broadcast we will wait for information about whether the destination has been found
        MPI_Ibcast(&dst_rcv, 1, MPI_INT, dst_rank, MPI_COMM_WORLD, &dst_req);
    
        // do{
        //     MPI_Test(&dst_req, &dst_flag, MPI_STATUS_IGNORE);
        //     //std::cout << "Flag value " << dst_flag << std::endl;
        // }while(!dst_flag);
        // std::cout << "Rank " << rank << " broadcast status " << dst_flag << std::endl; 
    }
    
    //register mpi data type
    create_msg_mpi_datatype();
    MPI_Barrier(MPI_COMM_WORLD);

    //receive any message from anywhere 
    
    message_set.resize(world_size);
    send_buffers.resize(world_size);
    send_requests.resize(world_size, nullptr);

    
    while(true)
    {
        // Step 2: process current open list and populate message set
        if((!open_list.empty()) && (!in_barrier_mode))
        {
            // Step 2a : process open list
            OpenListMember front = open_list.top();
            open_list.pop();
            Node * curr_node = std::get<2>(front);
            //std::cout << "Popped " << curr_node->id << std::endl;
            Node * proposed_parent = std::get<3>(front);
            float h = std::get<0>(front);
            float proposed_shortest_dist = std::get<1>(front);
            
            if (proposed_shortest_dist > curr_node->latest_shortest_distance_in_open_list) //outdated entry
                continue;

            if (curr_node->closed)
            {
                if (curr_node->id == src)continue;
                if (proposed_shortest_dist > curr_node->f)continue;
                // this means a new shorter path to a closed node is found  
            }
            curr_node->parent = proposed_parent;
            curr_node->f = proposed_shortest_dist;
            curr_node->g = h - proposed_shortest_dist;
            curr_node->open = false;
            curr_node->closed = true;

            if (curr_node->id == dst){
                std::cout << "Destination found.." << std::endl;
                std::cout << "Distance = " << curr_node->f << std::endl;
                if (!dst_found)
                {
                    dst_found=true;
                    MPI_Ibcast(&dst_rcv, 1, MPI_INT, dst_rank, MPI_COMM_WORLD, &dst_req);
                }
                
                continue;
            }

            // 2b: populate message set
            for (auto edge : curr_node->neighbours)
            {
                Node * neighbour = edge.first;
                float dist = curr_node->f + edge.second;
                // if (neighbour->open) //already in open list
                // {
                //     if (dist > neighbour->latest_shortest_distance_in_open_list) //no need to add this top open list
                //         continue;
                // }
                float h_ = dist + heuristic(*neighbour, nodes[dst]);
                //neighbour->open=true;
                //neighbour->latest_shortest_distance_in_open_list = dist; 
                //open_list.push(std::make_tuple(h_, dist, neighbour, curr_node));
                message_set[hash(nodes[neighbour->id])].push_back(create_msg(h_, dist, neighbour->id, curr_node->id));
                //std::cout << "Rank " << rank << " Pushed msg for " <<hash(nodes[neighbour->id]) << " Node " << neighbour->id  << std::endl;
            }
        }
        else
        {
            //open list is empty
            // Check if broadcast has been received;
            if(!dst_found)
            {
                if(dst_rank != rank)
                {
                    MPI_Test(&dst_req, &dst_flag, MPI_STATUS_IGNORE);
                    if(dst_flag)
                        dst_found = true;
                }
            }
            else
            {
                if(!in_barrier_mode)
                {
                    MPI_Ibarrier(MPI_COMM_WORLD, &barrier_req);
                    in_barrier_mode = true;
                }
                else
                {
                    MPI_Test(&barrier_req, &barrier_flag, MPI_STATUS_IGNORE);
                    if(barrier_req){
                        int to_send = open_list.size(), to_recv=0;
                        MPI_Allreduce(&to_send, &to_recv, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
                        if(to_recv==0)
                        {
                            std::cout << "Program Finished executing.. " << std::endl;
                            return;
                        }
                        else
                        {
                            in_barrier_mode = false;
                        }

                    }
                }
            }
        }

        //step 3: send messages
        send_message_set();

        //step 4: receive message set
        int num_msgs_recvd = receive_message_set();

        // if(num_msgs_recvd + message_set[rank].size())
        // {
        //     std::cout << "Rank (receive size)" << rank << " : " << num_msgs_recvd << " " << message_set[rank].size() << std::endl;
            
        // }
        add_msgs_to_open_list(num_msgs_recvd);

        

    }
}