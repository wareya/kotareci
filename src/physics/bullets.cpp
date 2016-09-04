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
                
                x += h * delta;
                y += (v+gravity/2) * delta;
                
                v += gravity;
                
                // reduce remaining lifespan of bullet
                bullet->life -= delta;
                
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
