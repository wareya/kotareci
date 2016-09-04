#include "subroutines.hpp"
#include "../input.hpp"
#include "../components/player.hpp"
#include "../blib/bmath.hpp"
#include <vector>

#ifdef CLIENT
#include "../samples.hpp"
#endif

static const float basegravity = 720;
static const float max_gravity = 2000;
static const float maxspeed = 300;
                    
static const float jumpspeed = -300; // initial velocity of a jump in physics pixels per second
static const int max_bounces = 10; // number of bounces to handle during motion
static const int stepsize = 4; // size of stairs in physics pixels

float baseaccel = 1000;
float deaccel = 1300;
float struggleslow = 0.2f; // struggle reaches 0.2 at low end
float strugglehard = 0.15f; // struggle reaches 0.15 at high end
float strugglepoint = 0.25f; // The point on the acceleration curve with the LEAST struggle


namespace Sys
{
    namespace Physicsers
    {
        // Benetnasch predicts movement and rescales it based on the actual time taken
        // in order to compromise on the advantages of both delta time and interpolated timestep.
        // Returns the hspeed after a given period of movement in terms of a given fixed rate.
        float given_movement(double hspeed, Input::PlayerInput input, Sys::Character * character, float delta, int * info)
        {
            /* Due to REASONS, Benetnasch isn't using the typical += accel *= friction method of handling walking speed.
             * Instead, we're going to "manually" model struggle as we approach the desired max speed.
             */
            float delta_baseaccel = baseaccel*delta;
            float delta_deaccel = deaccel*delta;
            
            int direction = (input.inputs[Input::RIGHT] - input.inputs[Input::LEFT]);
            
            // whether we're accelerating in the same direction we're moving
            // (with "not" for not controlling)
            int direction_agreement = (sign(direction) == sign(hspeed) and direction) != 0 ? 1 : -1;
            // with "yes" for not moving, yet controlling
            if (hspeed == 0 and direction) direction_agreement = 1;
            
            if(direction_agreement == 1)
            {
                float speed = fabs(hspeed);
                // We need to get how close we are to the max speed to model struggle
                float fraction = speed/maxspeed;
                bool started_lesser = (fraction < 1.0f);
                
                //hspeed += direction * delta_baseaccel;
                
                
                if(started_lesser)
                {
                    // Transform how close we are to the max speed, into the modifier
                    if (fraction < strugglepoint)
                    {
                        fraction /= strugglepoint; // gradient 0 ~ 1
                        
                        fraction *= 1.0f-struggleslow; // gradient ex. 0 ~ 0.8
                        fraction += struggleslow; // gradient ex. 0.2 ~ 1
                    }
                    else
                    {
                        fraction -= strugglepoint; // gradient ex. 0.75 ~ 0
                        fraction /= 1.0f-strugglepoint; //gradient ex. 1.0 ~ 0
                        fraction *= -1; // gradient ex. -1.0 ~ 0
                        fraction += 1; // gradient ex. 0 ~ 1.0
                        
                        fraction *= fraction; // curve down
                        
                        fraction *= 1.0f-strugglehard; // gradient ex. 0 ~ 0.85
                        fraction += strugglehard; // gradient ex. 0.15 ~ 1
                        
                    }
                    
                    //fraction /= 1.0f-strugglemodel;
                    
                    speed += delta_baseaccel*fraction;
                    #define INFO_CHECKFAST (1<<0)
                    *info |= INFO_CHECKFAST;
                    hspeed = direction * speed;
                }
                else
                {   // if we're agreeing with our movement, but STARTED going too fast,
                    // we need to use a completely different model in order to
                    // agree with the non-direction_agreement case.
                    // However, it still needs to be here (rather than non-direction_agreement)
                    // in order to gracefully go to maxspeed.
                    speed -= delta_deaccel;
                    #define INFO_CHECKSLOW (1<<1)
                    *info |= INFO_CHECKSLOW;
                    hspeed = sign(hspeed) * speed;
                }
            }
            else // non-direction_agreement case
            {
                float speed = fabs(hspeed);
                
                float apply = delta_deaccel;
                if(direction == 0)
                {   // Model struggle of approaching zero speed (rather than continuing to counter-accelerate) if we're not controlling
                    float fraction = speed/maxspeed;
                    if(fraction > 1.0)
                        fraction = 1.0;
                    
                    fraction *= 1.0f-struggleslow; // gradient ex. 0 ~ 0.8
                    fraction += struggleslow; // gradient ex. 0.2 ~ 1
                    
                    apply *= fraction;
                }
                
                #define INFO_CHECKREVERSE (1<<2)
                if(fabs(hspeed) < delta_deaccel and direction == 0)
                    *info |= INFO_CHECKREVERSE;
                hspeed -= sign(hspeed)*apply;
            }
            
            return hspeed;
        }
        
        
        bool MoveCharacters()
        {
            if(!Physicsers::delta_is_too_damn_low)
            {
                for(auto player : Sys::Players)
                {
                    auto character = player->character;
                    if(!character)
                        continue;
                    
                    /*
                     *  predef
                     */
                    double &x = character->position->x;
                    double &y = character->position->y;
                    double &hspeed = character->hspeed;
                    double &vspeed = character->vspeed;
                    Input::PlayerInput & input = player->input;
                    
                    // Our acceleration code isn't perfectly framerate-independent, so we need to do a little black magic.
                    // It turns out that if you only have motion and force, doing delta time on it can work.
                    // I was stubborn about this for a while, but you just have to make the motion assuming half the force
                    // and then apply the full force to the velocity afterwards.
                    
                    // Get the force of handling left/right inputs at particular "virtual" delta time
                    float virtual_delta = 1.0/85;
                    int info = 0;
                    float hforce = given_movement(hspeed, input, character, virtual_delta, &info) - hspeed;
                    hforce /= virtual_delta;
                    
                    int oldhsign = sign(hspeed);
                    
                    int jumping = (input.inputs[Input::JUMP] & !input.last_inputs[Input::JUMP]);
                    
                    // Apply impulse to vertical velocity if jumping
                    if(jumping)
                        vspeed = jumpspeed;
                    
                    bool started_feathery = (vspeed < max_gravity);
                    
                    // Set up force of gravity
                    float vforce = 0;
                    if(!place_meeting(character, 0, 0.01) or jumping)
                        vforce = basegravity;
                    
                    /*
                     *  This is a continuous contact solver, but it doesn't handle e.g. bunnyhopping from the point you hit the ground.
                     */
                    
                    // In continuous time, the speed would have a total effect on the position
                    // corresponding to the average of the start and end velocities, at least assuming
                    // the start and end forces have a linear force being applied to them (i.e. zero "jerk")
                    // hforce/vforce would be applied for "delta" period of time.
                    // Basically, we want to change position by a force. Easy. But we want to apply
                    // a force to that force. This happens within the delta window as well.
                    // So "times delta" *should* happen twice here for the accelerations.
                    auto h_auto = hspeed + hforce*delta/2;
                    auto v_auto = vspeed + vforce*delta/2;
                    
                    if(info & INFO_CHECKFAST)
                        if(abs(h_auto) > maxspeed)
                            h_auto = oldhsign * maxspeed;
                    if(info & INFO_CHECKSLOW)
                        if(abs(h_auto) < maxspeed)
                            h_auto = oldhsign * maxspeed;
                    if(info & INFO_CHECKREVERSE)
                        if(sign(h_auto) != oldhsign)
                            h_auto = 0;
                    
                    if(v_auto > max_gravity and started_feathery)
                        v_auto = max_gravity;
                    
                    // hspeed/vspeed would be applied to position for "delta" period of time".
                    // This is the vector of linear motion for the delta frame we're working on.
                    h_auto *= delta;
                    v_auto *= delta;
                    
                    hspeed += hforce*delta;
                    vspeed += vforce*delta;
                    
                    if(info & INFO_CHECKFAST)
                        if(abs(hspeed) > maxspeed)
                            hspeed = oldhsign * maxspeed;
                    if(info & INFO_CHECKSLOW)
                        if(abs(hspeed) < maxspeed)
                            hspeed = oldhsign * maxspeed;
                    if(info & INFO_CHECKREVERSE)
                        if(sign(hspeed) != oldhsign)
                            hspeed = 0;
                    
                    if(vspeed > max_gravity and started_feathery)
                        vspeed = max_gravity;
                    
                    /* movement solver starts here */
                    // we're in the wallmask; try to get out of it if it's really easy
                    if (place_meeting(character, 0, 0))
                    {
                        //puts("woaaAAAHHHh we're in the wallmask 1!");
                        for (int i = 1; i < stepsize; i += 1)
                        {
                            if(!place_meeting(character, 0, -i))
                            {
                                y -= i;
                                //puts("eject up");
                                break;
                            }
                        }
                    }
                    for (int i = max_bounces; i > 0; --i)
                    {
                        //puts("rectifying a collision");
                        float mx, my;
                        // move snugly to whatever we might've hit, and store the x and y deltas from that motion
                        std::tie(mx, my) = move_contact(character, h_auto, v_auto);
                        float travelled = sqdist(mx, my);
                        float totravel = sqdist(h_auto, v_auto);
                        h_auto -= mx;
                        v_auto -= my;
                        if (travelled < totravel) // we collided with something
                        {
                            // Check whether there's anything to our side
                            if(place_meeting(character, crop1(h_auto), 0))
                            {
                                // store original y before sloping
                                auto oy = y;
                                // check for slopes
                                if(!place_meeting(character, crop1(h_auto), stepsize)) // slope down a sloped ceiling step
                                    y += stepsize;
                                else if(!place_meeting(character, crop1(h_auto), -stepsize)) // slope up a normal ground slope
                                    y -= stepsize;
                                // no slope; it's a wall
                                if(oy == y)
                                {
                                    hspeed = 0;
                                    h_auto = 0;
                                }
                            }
                            // assume floor otherwise
                            else
                            {
                                vspeed = 0;
                                v_auto = 0;
                            }
                        }
                        // we did not collide with something
                        else
                        {
                            // we might want to "down" a slope
                            bool sloped = false;
                            // only if we're walking on the ground
                            if(vspeed == 0)
                            {
                                for (int i = stepsize; i <= abs(mx)+stepsize; i += stepsize)
                                {
                                    if(!place_meeting(character, 0, i) and place_meeting(character, 0, i+1))
                                    {
                                        sloped = true;
                                        //puts("downslope");
                                        y += i;
                                        vspeed = 0;
                                        v_auto = 0;
                                        break;
                                    }
                                }
                            }
                            if(!sloped) // whole bounce with no collisions to do
                            {
                                h_auto = 0;
                                v_auto = 0;
                                break;
                            }
                        }
                        if(v_auto == 0 and h_auto == 0) // exhausted travellable period anyways
                            break;
                    }
                    
                    // update weapon things
                    
                    auto rawangle = input.aimDirection;
                    auto dir = deg2rad(rawangle);
                    
                    int shooting = (input.inputs[Input::SHOOT]);// and not input.last_inputs[Input::SHOOT]);
                    if(shooting)
                    {
                        auto shotspeed = 800;
                        auto predistance = 20;
                        
                        auto vx = cos(dir);
                        auto vy = -sin(dir);
                        for(auto i = -5; i <= 5; i++)
                            new Bullet(Ent::New(), character->center_x()+vx*predistance + vy*i*3, character->center_y()-6+vy*predistance - vx*i*3, vx*shotspeed + hspeed, vy*shotspeed, 1);
                        #ifdef CLIENT
                            //fauxmix_emitter_fire(character->gun_emitter);
                        #endif
                    }
                    
                    auto aiming_left = (rawangle >= 90 and rawangle < 270);
                    
                    character->weaponsprite->angle = rawangle-180*aiming_left;
                    character->weaponsprite->flip = aiming_left;
                    character->stand->flip = aiming_left;
                    character->run->flip = aiming_left;
                    
                    auto running = (fabs(hspeed) > 1);
                    character->stand->visible = !running;
                    character->run->visible = running;
                    if(running)
                        character->run->index += hspeed/35*delta*(character->run->flip*-2+1);
                    else
                        character->run->index = 0;
                }
            }
            return false;
        }
    }
}
