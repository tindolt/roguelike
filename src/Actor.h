#pragma once
#include <libtcod.hpp>

class Actor {
    public :
        int x, y; //Position of the actor on the map
        int ch; //ASCII code of the character representing the actor
        TCODColor color; //Color of the actor

        Actor(int x, int y, int ch, const TCODColor &col);
    // console is the tile buffer passed down from Engine::render()
    void render(tcod::Console& console) const;
};
