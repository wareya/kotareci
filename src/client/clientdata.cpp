#include "clientdata.hpp"

#include "../input.hpp"
#include "../components/player.hpp"
#include "clientdata.hpp"

namespace Sys
{
    // input before being sent to server
    Input::ClientInput myinput;
    // pointer to own player component
    Player * myself;
    // vector of known own speeds for debug display
    std::vector<float> speeds;
}

