#include "think.hpp"

#include "clientdata.hpp"
#include "../physics.hpp" // delta_is_too_damn_low
#include "../blib/bmath.hpp"
#include "../rendering.hpp"

namespace Sys
{
    bool ClientThink()
    {
        if(!Physicsers::delta_is_too_damn_low)
        {
            Sys::myinput.Update();
            if(Sys::myself)
                Sys::myself->input = Sys::myinput.state;
        }
        return 0;
    }
}
