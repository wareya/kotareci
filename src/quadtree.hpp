#ifndef INCLUDE_BQUADTREE
#define INCLUDE_BQUADTREE

#include "components.hpp"

#include <set>
#include <array>
#include <vector>
#include <utility>

namespace Sys
{
    static const int depth = 6;
    struct QuadNode
    {
        std::array<QuadNode *, 4> * nodes;
        std::set<Component *> tags;
        float top, bottom, left, right;
        // top level constructor
        QuadNode(float x1, float y1, float x2, float y2);
        // contained node constructor
        QuadNode(unsigned level, QuadNode * parent, int index);
        void allocate_nodes(unsigned level);
        bool contact(float x1, float y1, float x2, float y2);
        void add(Component * a, float x1, float y1, float x2, float y2);
        // returns a sorted list of potential overlaps
        std::list<Component *> potentials(float x1, float y1, float x2, float y2);
    };
    
    extern QuadNode * QuadTree;
}

#endif
