//#include "blib.hpp"
#include "bengine.hpp"
#include "physics.hpp"
#include "maps.hpp"
#include "rendering.hpp"
#include "components/gamecomponents.hpp"
#include "input.hpp"
#include "client/clientdata.hpp"
#include "client/scripting.hpp"
#include "client/textinput.hpp"
#include "client/think.hpp"
#include "rendering/camera.hpp"
#include "physics.hpp"
#include "components/textwindow.hpp"
#include "samples.hpp"

#include <fauxmix/api.hpp>
#include <iostream>

// push systems into the mainloop
bool sys_init()
{
    #ifndef B_DEBUG_COREFRAMES
        Maps::load_wallmask("wallmask.png");
        Maps::load_background("ず.png");
        
        Sys::myinput.Init();
        Sys::myself = new Sys::Player(Ent::New(), "Wareya");
        Sys::myself->spawn(Maps::width/2, Maps::height/2);
        
        Sys::afont = new bfont(Sys::Renderer, "The Strider.png");
        ClientEngine::console.display = new Sys::TextWindow(Ent::New());
        auto t = ClientEngine::console.display;
        t->append_line("Test t");
        t->append_line("Test j");
        t->append_line("Test 1");
        t->append_line("Test 2");
        t->append_line("Test 3");
        t->append_line("Test 4");
        t->append_line("Test 5");
        ClientEngine::console.input = new Sys::TextBox(Ent::New());
        
        Sys::event_chain.push_back(&ClientEngine::Hotkeys); // client/textinput
    #endif
    
    Sys::tems.push_back(&Sys::FrameLimit); // bengine
    #ifndef B_DEBUG_COREFRAMES
        Sys::tems.push_back(&Sys::UpdateDelta); // physics
    #endif
    Sys::tems.push_back(&Sys::SDLEvents); // bengine
    #ifndef B_DEBUG_COREFRAMES
        Sys::tems.push_back(&Sys::ClientThinkStart);
        Sys::tems.push_back(&Sys::Physics); // physics
        Sys::tems.push_back(&Sys::UpdateCamera);
    #endif
    Sys::tems.push_back(&Sys::ClientThinkEnd);
    Sys::tems.push_back(&Sys::RenderThings); // rendering
    Sys::tems.push_back(&Lua::do_hud); // rendering
    Sys::tems.push_back(&Sys::PresentScreen); // rendering
    
    return 1; // remove sys_init from mainloop
}

bool main_init()
{
    // random display thing that should be moved elsewhere
	Sys::speeds.push_back(0);
	
	// initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        std::cout << "Could not initialize SDL: " << SDL_GetError() << std::endl;
    
    Sys::MainWindow = SDL_CreateWindow("Benetnasch", 300, 300, Sys::shape.w, Sys::shape.h, SDL_WINDOW_SHOWN);
    if (Sys::MainWindow == nullptr)
        std::cout << "Could not create an SDL window: " << SDL_GetError() << std::endl;
    Sys::Renderer = SDL_CreateRenderer(Sys::MainWindow, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_TARGETTEXTURE);
    if (Sys::Renderer == nullptr)
        std::cout << "Could not create an SDL renderer: " << SDL_GetError() << std::endl;
    
    fauxmix_dll_init();
    fauxmix_init(48000, false, 512);
    sample::ost = fauxmix_sample_load("sounds/duty.opus");
    fauxmix_sample_volume(sample::ost, 0.25);
    sample::footstep1 = fauxmix_sample_load("sounds/foot1.wav");
    sample::footstep2 = fauxmix_sample_load("sounds/foot2.wav");
    sample::footstep3 = fauxmix_sample_load("sounds/foot3.wav");
    sample::shot = fauxmix_sample_load("sounds/shot.wav");
    fauxmix_sample_volume(sample::shot, 0.35);
    
    //sample::emitter_ost = fauxmix_emitter_create(sample::ost);
    //fauxmix_emitter_loop(sample::emitter_ost, true);
    //fauxmix_emitter_fire(sample::emitter_ost);
    
    // don't know if I actually use randomness anywhere
    srand(time(NULL));
    SDL_PumpEvents();
    
    // sets up hud and such
    Lua::scripting_init();
    
    // something for the renderers! yeah!
    Sys::view_x = 0;
    Sys::view_y = 0;
    
    // get mainloop ready
    Sys::tems.push_back(&sys_init);
    
    return 0;
}
