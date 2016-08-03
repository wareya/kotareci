#ifndef INCLUDE_BCOMPONENTS
#define INCLUDE_BCOMPONENTS

#include "entity.hpp"
#include <list>
#include <unordered_map>

namespace Sys
{
    struct Component
    {
        template<typename T> friend struct Collection;
        Component(entityid_t myEntity);
        Component();
        virtual ~Component();
        
        entityid_t entityID;
    };
    
    // define interface to a linked list of components of some arbitrary type
    template<typename CType>
    struct Collection
    {
        typename std::list<CType *>::iterator begin()
        {
            return List.begin();
        }
        typename std::list<CType *>::iterator end()
        {
            return List.end();
        }
        unsigned long count()
        {
            return List.size();
        }
        void add(CType * new_instance)
        {
            List.push_back(new_instance);
            Map[new_instance] = --List.end();
        }
        void remove(CType * dying_instance)
        {
            List.erase(Map[dying_instance]);
            Map.erase(dying_instance);
        }
        void killall()
        {
            for(auto e : List)
                delete e;
        }
        
        private:
        std::list<CType *> List;
        std::unordered_map<CType *, typename std::list<CType *>::iterator> Map;
    };
    
}

#endif
