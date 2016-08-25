#include "renderers.hpp"
#include "commonrenderdep.hpp"

namespace Sys
{
    namespace Renderers
    {
        bool DrawBullets(float x, float y)
        {
            SDL_SetRenderDrawColor(Sys::Renderer, 0, 127, 255, 255);
            for(auto bullet : Sys::Bullets)
            {
                double dx = bullet->position->x - bullet->lastposition->x;
                double dy = bullet->position->y - bullet->lastposition->y;
                double dist = sqrt(dx*dx + dy*dy);
                if(dist == 0) continue;
                dx = dx/dist*20;
                dy = dy/dist*20;
                SDL_RenderDrawLine(Sys::Renderer, bullet->position->x-x, bullet->position->y-y, bullet->position->x-dx-x, bullet->position->y-dy-y);
            };
            return false;
        }
    }
}
