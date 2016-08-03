#include "renderers.hpp"
#include "commonrenderdep.hpp"

namespace Sys
{
    namespace Renderers
    {
        bool DrawBoxes(float x, float y)
        {
            SDL_SetRenderDrawBlendMode(Sys::Renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor( Sys::Renderer, 255, 255, 255, 64 );
            for(auto drawable : Sys::BoxDrawables)
            {
                SDL_RenderFillRect( Sys::Renderer, drawable->getShape(x, y, 0) );
            }
            SDL_SetRenderDrawColor( Sys::Renderer, 255, 0, 255, 255 );
            for(auto drawable : Sys::BoxDrawables)
            {
                SDL_RenderDrawRect( Sys::Renderer, drawable->getShape(x, y, 1) );
            };
            return false;
        }
    }
}
