#include "physics.hpp"
//#include "blib.hpp"

namespace Sys
{
    namespace Physicsers
    {
        // Time which the PREVIOUS frame took. Is the target frame time if framelimiter is running correctly.
        double delta = Time::Frametime;
        // read UpdateDelta
        bool delta_is_too_damn_low = false;
    }
    bool UpdateDelta()
    {
        #ifndef B_NO_DELTA
        if(Physicsers::delta_is_too_damn_low)
            Physicsers::delta += Time::delta;
        else
            Physicsers::delta = Time::delta;
        
    	if(Physicsers::delta > 0.1)
    		Physicsers::delta = 0.1;
        
        // don't do physics with low deltas; accumulate low deltas instead
        if(Physicsers::delta < 1.0/1000) 
    		//Physicsers::delta = 1.0/1000;
            Physicsers::delta_is_too_damn_low = true;
        else // will execute even if >= 1.0
            Physicsers::delta_is_too_damn_low = false;
        #endif
        return false;
    }
    bool Physics()
    {
    	if(!Physicsers::delta_is_too_damn_low)
        {
            Physicsers::MoveCharacters(); // characters.cpp
            Physicsers::MoveBullets();    // bullets.cpp
        }
        return false;
    }
}
