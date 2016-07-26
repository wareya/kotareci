#include "bengine.hpp"
#include "physics.hpp"
#include "input.hpp"
#include "blib/sdlhelpers.hpp"
#include "blib/bmath.hpp"

#include <iostream>

namespace Sys
{
    // self-explanatory
    SDL_Window * MainWindow;
    // read the SDL documentation
    SDL_Renderer * Renderer;
    // do it
    const SDL_Rect shape = {0, 0, 800, 600};
    // whether we want to quit out of the engine
    bool quit = false;
    
    // list of Sys::tems -- basically just parts of the mainloop
    std::vector<bool(*)()> tems;
    
    // Texture storage map. Nice and simple, right?
    // Sorry for using an obscure library.
    nall::map<std::string, SDL_Texture *> TexturePile;
    
    SDL_Texture * check_texture_pile(std::string sarg)
    {
        auto t = TexturePile.find( std::string(sarg) );
        std::cout << "Looking for texture " << sarg;
        if(!t.valid) // null/not found
        {
            puts("Nonfound texture, loading");
            auto sprite = loadTexture( sarg, Sys::Renderer );
            if (sprite)
                TexturePile.insert(std::string(sarg), sprite);
            return sprite;
        }
        else // already loaded
        {
            puts("Found texture");
            return *t.value;
        }
    }
    
    bool FrameLimit()
    {
        static double lastend;
        static unsigned long long consecutive;
        static double reference_time;
        if( Time::dostart )
        {
            consecutive = 0;
            lastend = Time::get_us();
            reference_time = lastend;
            
            Time::dostart = false;
            Time::delta = Time::Frametime / Time::scale;
        }
        else
        {
            consecutive++;
            double TargetTime = reference_time + consecutive*Time::Frametime*Time::scale;
            double Now = Time::get_us();
            double WaitSeconds = (TargetTime-Now)/Time::scale;
            if(WaitSeconds > 0)
            {
                int delayvalue = round(WaitSeconds*1000-1);
                if(delayvalue < 0) delayvalue = 0;
                SDL_Delay(delayvalue);
                while(Time::get_us() < TargetTime);
                lastend = TargetTime;
                Time::delta = Time::Frametime;
            }
            else
            {
                Time::delta = (Now-lastend) / Time::scale;
                if(Time::delta < 0) Time::delta = 0;
                lastend = Now;
                reference_time = Now;
                consecutive = 0;
            }
        }
        
        return false;
    }
    
    
    std::vector<event_bubble> event_chain;
    
    bool SDLEvents()
    {
        SDL_PumpEvents();
        SDL_Event event;
        while ( SDL_PollEvent( &event ) )
        {
            switch ( event.type )
            {
            case SDL_QUIT:
                quit = true;
                break;
            default:
                for ( auto func : event_chain )
                {
                    if( func(event) == CHAIN_FINISH )
                        break;
                }
                break;
            }   
        }
        return false;
    }
}
