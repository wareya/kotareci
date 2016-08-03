#ifndef INCLUDE_BQUADTREE
#define INCLUDE_BQUADTREE

#include "components.hpp"

#include <set>
#include <array>
#include <vector>
#include <utility>

namespace Sys
{
    static const int depth = 8;
    struct QuadNode
    {
        std::array<QuadNode *, 4> * nodes;
        std::set<Component *> tags;
        float top, bottom, left, right;
        // top level constructor
        QuadNode(float x1, float y1, float x2, float y2)
        {
            top = y1;
            bottom = y2;
            left = x1;
            right = x2;
            
            allocate_nodes(depth);
        }
        // contained node constructor
        QuadNode(unsigned level, QuadNode * parent, int index)
        {
            if(index & 1)
            { // right half of parent
                left   = (parent->left+parent->right)/2;
                right  = parent->right;
            }
            else
            { // left half of parent
                left   = parent->left;
                right  = (parent->left+parent->right)/2;
            }
            if(index & 2)
            { // bottom half of parent
                top    = (parent->top+parent->bottom)/2;
                bottom = parent->bottom;
            }
            else
            { // top half of parent
                top    = parent->top;
                bottom = (parent->top+parent->bottom)/2;
            }
            allocate_nodes(level);
        }
        void allocate_nodes(unsigned level)
        {
            if(level > 0)
            {
                nodes = new std::array<QuadNode *, 4>
                {{new QuadNode(level-1,this,0), new QuadNode(level-1,this,1),
                  new QuadNode(level-1,this,2), new QuadNode(level-1,this,3)}};
            }
            else
                nodes = nullptr;
        }
        bool contact(float x1, float y1, float x2, float y2)
        {
            if(x1 > x2) std::swap(x1, x2);
            if(y1 > y2) std::swap(y1, y2);
            return (x1 < right and x2 >= left and y1 < bottom and y2 >= top);
        }
        void add(Component * a, float x1, float y1, float x2, float y2)
        {
            if(contact(x1, y1, x2, y2))
            {
                tags.insert(a);
                if(nodes)
                {
                    for(auto e : *nodes)
                    {
                        // check contact inside child node
                        e->add(a, x1, y1, x2, y2);
                    }
                }
            }
        }
        /* TODO: Do properly
        void remove(Component * a)
        {
            auto i = tags.find(a);
            if(i != tags.end())
                tags.erase(i);
        }*/
        // returns a sorted list of potential overlaps
        std::list<Component *> potentials(float x1, float y1, float x2, float y2)
        {
            if(contact(x1, y1, x2, y2))
            {
                std::list<Component *> ret;
                if(nodes == nullptr)
                {
                    // iteration over std::set proceeds in sorted order
                    for(auto e : tags)
                        ret.push_back(e);
                    ret.sort();
                }
                else
                {
                    for(auto e : *nodes)
                        ret.merge(e->potentials(x1, y1, x2, y2));
                }
                return ret;
            }
            else
                return {};
        }
    };
    
    extern QuadNode * QuadTree;
}

#endif
