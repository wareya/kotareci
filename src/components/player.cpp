#include "gamecomponents.hpp"
#include "player.hpp"

namespace Sys
{
    Player::Player(entityid_t myEntity, const char * name) : Component(myEntity), character(nullptr), name(name), myself(false)
    {
        spawntimer = -1;
        Players.add(this);
        puts("MAKING A PLAYER");
    }
    Player::~Player()
    {
        delete character;
        Players.remove(this);
    }
    void Player::spawn(double x, double y)
    {
        if(!character)
            character = new Character(entityID, x, y);
    }
    Collection<Player> Players;
}
