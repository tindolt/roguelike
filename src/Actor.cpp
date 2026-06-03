#include <stdio.h>
#include "libtcod.hpp"
#include "Actor.h"
#include "Map.h"
#include "Engine.h"

Actor::Actor(int x, int y, int ch, const char *name, const TCODColor &col) :
 x(x), y(y), ch(ch), color(col) {
    this->name = name;
}

void Actor::render(tcod::Console& console) const {
    // Set the tile's character (codepoint) at this actor's grid position
    console.at(x, y).ch = ch;
    // Set the foreground color; tile colors are TCOD_ColorRGBA {r, g, b, a}
    console.at(x, y).fg = {color.r, color.g, color.b, 255};
}

void Actor::update() {
    printf("The %s growls.\n", name);
}

bool Actor::moveOrAttack(int x, int y) {
    if ( engine.map->isWall(x,y) ) return false;
    for (auto iterator = engine.actors.begin();
        iterator != engine.actors.end(); iterator++) {
        Actor *actor = *iterator;
        if ( actor->x == x && actor->y == y ) {
            printf("The %s laughs at your puny efforts to attack him!\n", actor->name);
            return false;
        }
    }
    this->x=x;
    this->y=y;
    return true;
}