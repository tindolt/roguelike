#pragma once
#include <libtcod.hpp>

struct Tile {
    bool canWalk; // Can actors walk on this tile?
    Tile() : canWalk(false) {}
};

class Map {
    public :
        int width, height;

        Map(int width, int height);
        ~Map();
        bool isWall(int x, int y) const;
        // console is the tile buffer passed down from Engine::render()
        void render(tcod::Console& console) const;
    protected :
        Tile *tiles;
        friend class BspListener;

        void dig(int x1, int y1, int x2, int y2);
        void createRoom(bool first, int x1, int y1, int x2, int y2);
};