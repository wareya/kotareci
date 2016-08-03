#include "subroutines.hpp"


namespace Sys
{
    namespace Physicsers
    {
        bool MoveBullets()
        {
            float gravity = 800*delta;
            for(auto i = Bullets.begin(); i != Bullets.end();)
            {
                auto bullet = *i;
                
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
                
                // Move to next bullet before deleting this bullet
                i++;
                
                // kill bullet if it collided something collideable
                if(bullet->life < 0 or line_meeting(bullet->lastposition->x, bullet->lastposition->y, x, y))
                    delete bullet;
            }
            return false;
        }
    }
}
