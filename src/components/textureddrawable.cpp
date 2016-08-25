#include "drawingcomponents.hpp"

// TODO: move Sys::Renderer and TexturePile outside of the main engine ("bengine")

#include "../bengine.hpp" // TexturePile, Sys::Renderer
#include "../blib/sdlhelpers.hpp"

namespace Sys
{
    TexturedDrawable::TexturedDrawable(entityid_t myEntity, double argx, double argy, double argxoffset, double argyoffset ) : Component(myEntity), xoffset(argxoffset), yoffset(argyoffset)
    {
        position = new Position(myEntity, argx, argy);
        TexturedDrawables.add(this);
    }
    TexturedDrawable::TexturedDrawable(entityid_t myEntity, double argx, double argy, double argxoffset, double argyoffset, char noaddifdefined ) : Component(myEntity), xoffset(argxoffset), yoffset(argyoffset)
    {
        position = new Position(myEntity, argx, argy);
    }
    TexturedDrawable::~TexturedDrawable()
    {
        if(position->entityID == entityID) delete position;
        
        TexturedDrawables.remove(this);
    }
    bool TexturedDrawable::set_sprite(const char * sarg)
    {
        sprite = bsprite(sarg);
        return sprite.valid();
    }
}
