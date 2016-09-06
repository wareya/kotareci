#include "renderers.hpp"
#include "commonrenderdep.hpp"
#include "renderstack.hpp"
#include "../input.hpp"
#include "../physics.hpp"
#include "../client/clientdata.hpp"

#include <string> // to_string

namespace Sys
{
    namespace Renderers
    {
        bool DrawScreenText(float x, float y)
        {
            TemporaryOffscreenRenderMode Gimme(Sys::Renderer);
            renderText(0, 13*4,
                       (std::string("FPS:    ")+std::to_string(1/Time::delta)).data(),
                       Sys::afont);
            renderText(0, 13*5,
                       (std::string("Chunks: ")+std::to_string(Sys::BoxDrawables.count())).data(),
                       Sys::afont);
            renderText(0, 13*6,
                       (std::string("Bullets:")+std::to_string(Sys::Bullets.count())).data(),
                       Sys::afont);
            renderText(0, 13*8,
                       (std::string("Delta:  ")+std::to_string(Physicsers::delta)).data(),
                       Sys::afont);
            std::string inputstr("");
            if(myself)
            {
                for(short i = 0; i < Input::NUMBER_INPUTS; i++)
                    inputstr += std::to_string(myself->input.inputs[i]);
                renderText(0, 13*1,
                           (std::string("Inputs: ")+inputstr).data(),
                           Sys::afont);
                if(myself->character)
                {
                    renderText(0, 13*2,
                               (std::string("h:      ")+std::to_string(myself->character->hspeed)).data(),
                                Sys::afont);
                    renderText(0, 13*3,
                               (std::string("v:      ")+std::to_string(myself->character->vspeed)).data(),
                               Sys::afont);
                    renderText(0, 13*7,
                               (std::string("x, y:   ")+std::to_string(myself->character->position->x)+std::string(" ")+std::to_string(myself->character->position->y)).data(),
                               Sys::afont);
                }
            }
            
            Gimme.release();
            
            #ifndef B_DEBUG_FRAMESONLY
            TemporaryTextureColor DropshadowBottom(Gimme.texture, 0,0,0);
            renderTexture(Gimme.texture, Sys::Renderer, 0, 1);
            renderTexture(Gimme.texture, Sys::Renderer, 1, 0);
            #endif
            
            TemporaryTextureColor Body(Gimme.texture, 255,255,255);
            renderTexture(Gimme.texture, Sys::Renderer, 0, 0);
        	
            return false;
        }
    }
}
