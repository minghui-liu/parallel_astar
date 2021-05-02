#ifndef __ASTAR_HPP
#define __ASTAR_HPP

#include <vector>
#include <functional>
#include <set>
#include <iostream>

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
        uint G, H;
        Vec2i coordinates;
        std::vector<Node> neighbours;
        int id;

        Node(Vec2i coord_, int id_);
        void print_info();
        void add_neighbours(std::vector<Node> &neighbours_);
        void print_neighbours();
        uint getScore();
    };



}


#endif 