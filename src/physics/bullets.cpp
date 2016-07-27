#include "subroutines.hpp"

namespace Sys
{
    namespace Physicsers
    {
        bool MoveBullets()
        {
            float gravity = 800*delta;
            std::vector<Bullet *> marked_for_removal;
            for(auto bullet : Sys::Bullets)
            {
                // get reference to attributes of bullet for less typing
                auto &x = bullet->position->x;
                auto &y = bullet->position->y;
                
                *bullet->lastposition = *bullet->position;
                
                auto &h = bullet->hspeed;
                auto &v = bullet->vspeed;
                
                
                // Only function by half a frame on the first think, because the first think happens in the same frame as firing the bullet. This prevents low framerates from having bullets "come out" further along the barrel, gameplay-wise.
                double falsedelta = (bullet->life == 1) ? delta*0.5 : delta;
                
                x += h * falsedelta;
                y += (v+gravity/2) * falsedelta;
                
                v += gravity;
                
                // reduce remaining lifespan of bullet
                bullet->life -= falsedelta;
                
                // mark bullet for death if it collided something collideable
                if(bullet->life < 0 or line_meeting_which(bullet->lastposition->x, bullet->lastposition->y, x, y).size()>0)
                {
                    marked_for_removal.push_back(bullet);
                }
            }
            // kill marked bullets
            for(auto bullet : marked_for_removal)
                delete bullet;
            return false;
        }
    }
}
