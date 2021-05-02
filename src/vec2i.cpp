#include "astar.hpp"
#include <algorithm>


using namespace std::placeholders;
using namespace AStar;

Vec2i::Vec2i(float x_, float y_)
{
    x = x_;
    y = y_;
}

Vec2i::Vec2i()
{
    x = 0;
    y = 0;
}

