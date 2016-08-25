#include "quadtree.hpp"

namespace Sys
{
    // Public domain fast power over integers
    int ipow(int base, int exp)
    {
        int result = 1;
        while (exp)
        {
            if (exp & 1)
                result *= base;
            exp >>= 1;
            base *= base;
        }
        return result;
    }
    // top level constructor
    QuadNode::QuadNode(float x1, float y1, float x2, float y2)
    {
        top = y1;
        bottom = y2;
        left = x1;
        right = x2;
        
        printf("Building quadtree of %d bottom-level nodes\n", (int)ipow(4, depth));
        printf("(%d total nodes)\n", ((int)ipow(4, depth+1)-1)/3);
        
        allocate_nodes(depth);
    }
    // contained node constructor
    QuadNode::QuadNode(unsigned level, QuadNode * parent, int index)
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
    void QuadNode::allocate_nodes(unsigned level)
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
    bool QuadNode::contact(float x1, float y1, float x2, float y2)
    {
        if(x1 > x2) std::swap(x1, x2);
        if(y1 > y2) std::swap(y1, y2);
        return (x1 < right and x2 >= left and y1 < bottom and y2 >= top);
    }
    void QuadNode::add(Component * a, float x1, float y1, float x2, float y2)
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
    // returns a sorted list of potential overlaps
    std::list<Component *> QuadNode::potentials(float x1, float y1, float x2, float y2)
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
    
    QuadNode * QuadTree;
}
