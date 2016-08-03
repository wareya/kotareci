#include "gamecomponents.hpp"
#include "player.hpp"

namespace Sys
{
    Character::Character(entityid_t myEntity, double argx, double argy) : Component(myEntity), player(nullptr), hspeed(0), vspeed(0), myself(false)
    {
        // movement hull
        hull = new Hull(myEntity, 12, 36, 10, 12);
        // hitboxes
        head = new Hull(myEntity, 8, 8, 12, 10);
        chest = new Hull(myEntity, 16, 12, 8, 18);
        legs = new Hull(myEntity, 12, 18, 10, 30);
        
        position = new Position(myEntity, argx, argy);
        
        stand = new TexturedDrawable(Ent::New(), argx, argy, 0, 0);
        run = new AnimatedTexturedDrawable(stand->entityID, argx, argy, 0, 0, 4, 32, 48);
        weaponsprite = new RotatingTexturedDrawable(stand->entityID, argx, argy, -8, 8, 0, 20, 16);
        
        stand->set_sprite(image_stand);
        run->set_sprite(image_run);
        run->visible = false;
        weaponsprite->set_sprite(image_weapon);
        
        delete stand->position;
        stand->position = position;
        
        delete run->position;
        run->position = position;
        
        delete weaponsprite->position;
        weaponsprite->position = position;
        
        gun_emitter = fauxmix_emitter_create(sample::shot);
        
        puts("making a character");
        
        Characters.add(this);
    }
    Character::~Character()
    {
        delete hull;
        delete head;
        delete chest;
        delete legs;
        delete position;
        delete stand;
        delete run;
        delete weaponsprite;
        player->character = nullptr;
        Characters.remove(this);
    }
    void Character::center_on(float x, float y)
    {
        position->x = x - (hull->w/2+hull->xoffset);
        position->y = y - (hull->h/2+hull->yoffset);
    }
    float Character::center_x ()
    {
        return position->x + hull->w/2 + hull->xoffset;
    }
    float Character::center_y ()
    {
        return position->y + hull->h/2 + hull->yoffset;
    }
}
