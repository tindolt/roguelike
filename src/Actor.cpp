#include "libtcod.hpp"
#include "Actor.h"

Actor::Actor(int x, int y, int ch, const TCODColor &col) :
 x(x), y(y), ch(ch), color(col) {
}

void Actor::render(tcod::Console& console) const {
    // Set the tile's character (codepoint) at this actor's grid position
    console.at(x, y).ch = ch;
    // Set the foreground color; tile colors are TCOD_ColorRGBA {r, g, b, a}
    console.at(x, y).fg = {color.r, color.g, color.b, 255};
}