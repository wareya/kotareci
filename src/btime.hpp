#ifndef INCLUDED_BTIME
#define INCLUDED_BTIME

#include <chrono>
#include <deque>

namespace Time
{
    extern bool dostart;
    
    extern const double Framerate;
    
    extern const double Framesnum;
    extern const double Frametime;
    extern double ticks;
    
    extern double scale;
    
    extern double delta;
    
    double get_us();
}

#endif
