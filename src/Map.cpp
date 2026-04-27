#include "libtcod.hpp"
#include "Map.h"
#include "Actor.h"
#include "Engine.h"

static const int ROOM_MAX_SIZE = 12;
static const int ROOM_MIN_SIZE = 6;

class BspListener : public ITCODBspCallback {
public:
    Map& map;
    int roomNum = 0;   // how many rooms have been created so far
    int lastx = 0;     // center x of the last room — used to dig corridors
    int lasty = 0;     // center y of the last room — used to dig corridors

    BspListener(Map& map) : map(map) {}

    bool visitNode(TCODBsp* node, void* userData) {
        if (node->isLeaf()) {
            TCODRandom* rng = TCODRandom::getInstance();

            // Pick a random room size that fits inside the BSP node.
            // We subtract 2 so rooms never touch the node border (avoids merged rooms).
            int w = rng->getInt(ROOM_MIN_SIZE, node->w - 2);
            int h = rng->getInt(ROOM_MIN_SIZE, node->h - 2);
            // Pick a random position inside the node for the top-left corner.
            int x = rng->getInt(node->x + 1, node->x + node->w - w - 1);
            int y = rng->getInt(node->y + 1, node->y + node->h - h - 1);

            map.createRoom(roomNum == 0, x, y, x + w - 1, y + h - 1);

            if (roomNum != 0) {
                // Dig an L-shaped corridor from the center of the last room.
                // First a horizontal segment, then a vertical one.
                map.dig(lastx, lasty, x + w / 2, lasty);
                map.dig(x + w / 2, lasty, x + w / 2, y + h / 2);
            }

            // Record the center of this room for the next corridor
            lastx = x + w / 2;
            lasty = y + h / 2;
            roomNum++;
        }
        return true; // returning false would stop the BSP traversal early
    }
};

Map::Map(int width, int height) : width(width), height(height) {
    tiles = new Tile[width * height];
    map = new TCODMap(width, height);
    TCODBsp bsp(0, 0, width, height);
    bsp.splitRecursive(NULL, 8, ROOM_MAX_SIZE, ROOM_MAX_SIZE, 1.5f, 1.5f);
    BspListener listener(*this);
    bsp.traverseInvertedLevelOrder(&listener,NULL);
}

Map::~Map() {
    delete[] tiles;
    delete map;
}

bool Map::isWall(int x, int y) const {
    return !map->isWalkable(x, y);
}

bool Map::isExplored(int x, int y) const {
    return tiles[x + y * width].explored;
}

bool Map::isInFov(int x, int y) const {
    if (map->isInFov(x, y)) {
        tiles[x + y * width].explored = true;
        return true;
    }
    return false;
}

void Map::computeFov() {
    map->computeFov(engine.player->x, engine.player->y,
        engine.fovRadius);
}

void Map::dig(int x1, int y1, int x2, int y2) {
    if (x2 < x1 ) {
        int tmp=x2;
        x2=x1;
        x1=tmp;
    }
    if (y2 < y1 ) {
        int tmp=y2;
        y2=y1;
        y1=tmp;
    }
    for (int tilex = x1; tilex <= x2; tilex++) {
        for (int tiley = y1; tiley <= y2; tiley++) {
            map->setProperties(tilex, tiley, true, true);
        }
    }
}

void Map::createRoom(bool first, int x1, int y1, int x2, int y2) {
    dig(x1, y1, x2, y2);
    if (first) {
        // Place the player in the first room
        engine.player->x = (x1 + x2) / 2;
        engine.player->y = (y1 + y2) / 2;
    } else {
        TCODRandom *rng = TCODRandom::getInstance();
        if (rng->getInt(0, 3) == 0) {
            // 25% chance to place a monster in this room
            engine.actors.push_back(new Actor((x1 + x2) / 2, (y1 + y2) / 2, 'X', tcod::ColorRGB(255, 0, 0)));
        }
    }
}

void Map::render(tcod::Console& console) const {
    // TCOD_ColorRGBA {r, g, b, a} replaces TCODColor for direct tile access
    static const TCOD_ColorRGBA darkWall{0, 0, 100, 255};
    static const TCOD_ColorRGBA darkGround{50, 50, 150, 255};
    static const TCOD_ColorRGBA lightWall{130, 110, 50, 255};
    static const TCOD_ColorRGBA lightGround{200, 180, 50, 255};
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            if( isInFov(x, y) ) {
                console.at(x, y).bg = isWall(x, y) ? lightWall : lightGround;
            } else if (isExplored(x, y)) {
                console.at(x, y).bg = isWall(x, y) ? darkWall : darkGround;
            }
        }
    }
}