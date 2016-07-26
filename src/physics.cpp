#include "physics.hpp"
//#include "blib.hpp"

namespace Sys
{
    namespace Physicsers
    {
        // Time which the PREVIOUS frame took, from framelimiter call to framelimiter call. Is the frame time if framelimiter is running correctly
        double delta = 1/Time::Framerate;
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
            
         // seemingly safest max delta time (4fps) because low framerates break delta time
    	if(Physicsers::delta > 0.25)
    		Physicsers::delta = 0.25;
        
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
