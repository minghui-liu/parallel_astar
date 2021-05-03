#ifndef __ASTAR_HPP
#define __ASTAR_HPP

#include <vector>
#include <functional>
#include <set>
#include <iostream>
#include <queue>
#include <tuple>

namespace AStar
{
    class Vec2i
    {
        public:
            float x, y;
            Vec2i(float x_, float y_);
            Vec2i();

    };

    using HeuristicFunction = std::function<uint(Vec2i, Vec2i)>;
    
    //using CoordinateList = std::vector<Vec2i>;
    
    struct Node
    {
        Vec2i coordinates;
        std::vector<std::pair<Node*,float> > neighbours;
        int id;
        float g; // heuristic
        float f; //distance from source
        float latest_shortest_distance_in_open_list; //to filter outdated entries in open list
        
        bool closed; // if its in open list
        bool open; // if its in closed list
        Node * parent; // who is the parent
        

        Node(Vec2i coord_, int id_);
        void print_info();
        //void add_neighbours(std::vector<Node> &neighbours_);
        //void add_weights(std::vector<float> &edge_weights_);
        void add_neighbour(Node &neighbour, float edge_weight);
        void print_neighbours();
        uint getScore();
    };

    using OpenListMember = std::tuple<float, float, Node*, Node*>;

//     void read_file(char *addr, int *data, int num_cols)
// {
//     FILE *fp = fopen(addr,"r");
//     int a, b;
//     while(fscanf(fp, "%d, %d", &a, &b) != EOF){
//         data[a*num_cols + b] = 1;
//     } 
//     fclose(fp);
// }

    class myComparator
    {
        public:
            int operator() (const OpenListMember& p1, const OpenListMember& p2)
            {
                return std::get<0>(p1) > std::get<0>(p2);
            }
    };

    struct Graph
    {
        std::vector<Node> nodes;
        std::priority_queue<OpenListMember, std::vector<OpenListMember>, myComparator > open_list;
        Graph(char* node_addr, char* edges_addr);
        void print_info();
        void astar(int src, int dst);
        void show_path(int dst);
        float heuristic(Node &n, Node &dst);
    };
}


#endif 