#include "libtcod.hpp"
#include "Engine.h"
#include "Actor.h"
#include "Map.h"

Engine::Engine() {
    auto tileset = tcod::load_tilesheet("data/fonts/terminal.png", {16, 16}, tcod::CHARMAP_CP437);
    // Configure the window and renderer via TCOD_ContextParams
    // This replaces the old TCODConsole::initRoot()
    TCOD_ContextParams params{};
    params.tileset = tileset.get();
    params.columns = 80;
    params.rows = 50;
    params.window_title = "Roguelike Game";
    params.renderer_type = TCOD_RENDERER_SDL2;
    params.vsync = true;
    context_ = std::make_unique<tcod::Context>(params);

    // The console is the off-screen tile buffer (width x height in tiles)
    console_ = std::make_unique<tcod::Console>(80, 50);

    player = new Actor(40, 25, '@', tcod::ColorRGB(255, 255, 255));
    actors.push_back(player);
    map = new Map(80, 45);
}

Engine::~Engine() {
    for (Actor* actor : actors) {
        delete actor;
    }
    actors.clear();
    delete map;
}

// key is the SDL_Scancode forwarded from the event loop in main.cpp
void Engine::update(SDL_Scancode key) {
    // SDL_SCANCODE values replace the deprecated TCODK_ constants
    switch(key) {
        case SDL_SCANCODE_UP:
            if (!map->isWall(player->x, player->y - 1)) player->y--;
            break;
        case SDL_SCANCODE_DOWN:
            if (!map->isWall(player->x, player->y + 1)) player->y++;
            break;
        case SDL_SCANCODE_LEFT:
            if (!map->isWall(player->x - 1, player->y)) player->x--;
            break;
        case SDL_SCANCODE_RIGHT:
            if (!map->isWall(player->x + 1, player->y)) player->x++;
            break;
        default: break;
    }
}

void Engine::render() {
    // Clear the console to black before drawing each frame
    console_->clear();

    // Draw map tiles (sets each tile's background color)
    map->render(*console_);

    // Draw all actors (sets each tile's character and foreground color)
    for (Actor* actor : actors) {
        actor->render(*console_);
    }

    // Present the finished console to the screen
    // This replaces the old TCODConsole::flush()
    context_->present(*console_);
}