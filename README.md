kotareci
===========
Kotareci is a demonstration game for Benetnasch2's engine structure.

Benetnasch2 is a basic platformer engine written in C++/SDL. Absolutely everything is implemented from scratch, with a very small number of libraries used:

* Faucet Mixer, which uses SDL, provides games audio functionality
* Lua provides scripting capabilities for things like the HUD

These dependencies can be compiled without any dependencies that Benetnasch2 does not already depend on.

The character movement solver is extremely well-responding to very dynamic framerates. Character collision detection is floatingpoint-perfect.

Benetnasch compiles and works properly on windows and linux. Other platforms have not been tested, but almost definitely work (with compile script tweaking).
