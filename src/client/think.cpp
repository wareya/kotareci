#include "think.hpp"

#include "clientdata.hpp"
#include "../physics.hpp" // delta_is_too_damn_low
#include "../blib/bmath.hpp"
#include <fauxmix/api.hpp>
#include "../rendering.hpp"

namespace Sys
{
    // Runs immediately after the framelimiter
    bool ClientThinkStart()
    {
        if(!Physicsers::delta_is_too_damn_low)
        {
            Sys::myinput.Update();
            if(Sys::myself)
                Sys::myself->input = Sys::myinput.state;
        }
        return 0;
    }
    // Runs immediately after game logic, *before* rendering
    bool ClientThinkEnd()
    {
        fauxmix_push();
        return 0;
    }
}
