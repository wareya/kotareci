#include "bengine.hpp"
#include "physics.hpp"
#include "input.hpp"
#include "blib/sdlhelpers.hpp"
#include "blib/bmath.hpp"

#include <iostream>
#include <unordered_map>

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
    
    // Cache image files loaded from disk so they can be shared without being reloaded
    std::unordered_map<std::string, SDL_Texture *> TexturePile;
    std::unordered_map<SDL_Texture *, int> references;
    SDL_Texture * check_texture_pile(std::string sarg)
    {
        auto t = TexturePile.find(sarg);
        if(t == TexturePile.end()) // null/not found
        {
            puts("Missed texture cache");
            auto sprite = loadTexture( Sys::Renderer, sarg.data() );
            if (sprite)
                TexturePile[std::string(sarg)] = sprite;
            references[sprite] = 1;
            return sprite;
        }
        else // already loaded
        {
            puts("Hit texture cache");
            references[(*t).second] += 1;
            return (*t).second;
        }
    }
    
    void abandon_texture(SDL_Texture * sprite)
    {
        if(references.count(sprite))
            references[sprite]--;
    }
    
    bsprite::bsprite()
    {
        texture = nullptr;
    }
    bsprite::bsprite(const char * filename)
    {
        texture = check_texture_pile( std::string(filename) );
        if (texture)
            SDL_QueryTexture( texture, NULL, NULL, &w, &h );
    }
    bsprite::~bsprite()
    {
        abandon_texture(texture);
    }
    bool bsprite::valid()
    {
        return !!texture;
    }
    
    double lastdelta;
    void UpdateDelta(double newdelta)
    {
        Time::delta = (newdelta+lastdelta)/2;
        lastdelta = newdelta;
    }
    
    bool FrameLimit()
    {
        fflush(stdout); // best place to put this
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
            lastdelta = Time::delta;
        }
        else
        {
            consecutive++;
            // Get desired timestamp for framelimiter to end on based on number of consecutive framelimited frames
            double TargetTime = reference_time + consecutive*Time::Frametime*Time::scale;
            double Now = Time::get_us();
            double WaitSeconds = (TargetTime-Now)/Time::scale;
            // If we haven't yet missed that timestamp, wait on it. Otherwise, reset.
            if(WaitSeconds > 0)
            {
                int delayvalue = floor(WaitSeconds*1000-1);
                if(delayvalue < 0) delayvalue = 0;
                if(delayvalue)
                {
                    SDL_Delay(delayvalue);
                    while(Time::get_us() < TargetTime);
                }
                lastend = TargetTime;
                UpdateDelta(Time::Frametime);
            }
            // we're out of sync but haven't lost a full frametime of phase, pretend it's not happening
            else if(Now < TargetTime + Time::Frametime*Time::scale)
            {
                lastend = TargetTime;
                UpdateDelta(Time::Frametime);
            }
            // we've fallen a full frame behind our framelimiter's ideal in terms of phase, reset
            else
            {
                UpdateDelta((Now-lastend)/Time::scale);
                if(Time::delta < 0) Time::delta = 0;
                
                lastend = Now;
                consecutive = 0;
                reference_time = Now;
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
