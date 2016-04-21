#include "../rendering.hpp"
#include "../comnponents/player.hpp"

namespace Sys
{
    bool UpdateCamera()
    {
        if(Sys::myself)
        {
            if(Sys::myself->character)
            {
                Sys::view_x = floor(Sys::myself->character->center_x())-Sys::shape.w/2;
                Sys::view_y = floor(Sys::myself->character->center_y())-Sys::shape.h/2;
            }
        }
        return false;
    }
}
