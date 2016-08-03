#include "blib/sdlhelpers.hpp"
#include "blib/bmath.hpp"
#include "rendering/renderers.hpp"
#include "bengine.hpp"
#include "components/gamecomponents.hpp"

namespace Sys
{
    float view_x, view_y;
    bfont * afont;
    
    bool RenderThings()
    {
    	#ifndef B_DEBUG_COREFRAMES
        #ifndef B_DEBUG_NORENDER
        // Clear screen
        // Cheap clear; use SDL_RenderClear() instead of SDL_RenderFillRect() if there are problems
        SDL_SetRenderDrawColor( Renderer, 0, 0, 0, 255 );
        SDL_RenderFillRect( Renderer, &shape );
        
        // Draw simple drawables
        Renderers::DrawBackground(view_x, view_y);
        //Renderers::DrawBoxes(view_x, view_y);
        Renderers::DrawAnimatedTextured(view_x, view_y);
        Renderers::DrawTextured(view_x, view_y);
        Renderers::DrawRotateTextured(view_x, view_y);
        Renderers::DrawCharacterDebug(view_x, view_y);
        Renderers::DrawBullets(view_x, view_y);
        Renderers::DrawSpeedometer(view_x, view_y);
        Renderers::DrawTextWindows(view_x, view_y);
        #endif
        #endif
        Renderers::DrawScreenText(view_x, view_y);
        
        return false;
    }
    bool PresentScreen()
    {
        SDL_RenderPresent(Renderer);
        return false;
    }
}
