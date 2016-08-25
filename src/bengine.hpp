#ifndef INCLUDE_BENGINE
#define INCLUDE_BENGINE

#include <SDL2/SDL.h>
#undef main
#include <vector>
#include <string>

namespace Sys
{
    extern SDL_Window * MainWindow;
    extern SDL_Renderer * Renderer;
    extern const SDL_Rect shape;
    extern bool quit;
    
    extern std::vector<bool(*)()> tems; // Sys::tems
    
    struct bsprite
    {
        bsprite();
        bsprite(const char * filename);
        bsprite(const bsprite&) = delete; // delete copy constructor: class interacts with refcounter on destruction
        bsprite(bsprite &&) = default; // force creation of move constructor 
        bsprite& operator=(bsprite &&) = default; // force assignment to use move
        ~bsprite();
        bool valid();
        SDL_Texture * texture = nullptr;
        private:
        int w = 0;
        int h = 0;
    };
    
    bool FrameLimit();
    
    typedef enum
    {
        CHAIN_CONTINUE,
        CHAIN_FINISH
    } chainreturn;
    typedef chainreturn(*event_bubble)(SDL_Event);
    extern std::vector<event_bubble> event_chain;
    
    bool SDLEvents();
}

#endif
