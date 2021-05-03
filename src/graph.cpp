#include "astar.hpp"
#include <algorithm>
#include <map>
#include <math.h>
#include <stack>

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

void Graph::astar(int src, int dst)
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

        if (curr_node->id == dst)
        {
            std::cout << "found path" << std::endl;
            curr_node->parent = proposed_parent;
            curr_node->f = proposed_shortest_dist;
            curr_node->g = h - proposed_shortest_dist;
            curr_node->open = false;
            curr_node->closed = true;
            return;
        }

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

void Graph::show_path(int dst)
{
    std::stack<int> path;
    path.push(dst);
    Node * curr = &nodes[dst];
    std::cout << "Shortest Path Length is " << curr->f << std::endl;
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