#include "gamecomponents.hpp"
#include "../quadtree.hpp"

namespace Sys
{
    // misnomer: actually a wallmask chunk
    BoxDrawable::BoxDrawable(entityid_t myEntity, double argx, double argy, double argw, double argh) : Component(myEntity)
    {
        position = new Position(myEntity, argx, argy);
        hull = new Hull(myEntity, argw, argh, 0, 0);
        BoxDrawables.add(this);
    }
    BoxDrawable::~BoxDrawable()
    {
        delete position;
        delete hull;
        BoxDrawables.remove(this);
    }
    SDL_Rect * BoxDrawable::getShape(float x, float y, int buffer) // offsets
    {
        shape = {int(ceil(position->x-x)),
                 int(ceil(position->y-y)),
                 int(round(hull->w+buffer)),
                 int(round(hull->h+buffer))};
        return &shape;
    }
}
