#include "libtcod.hpp"
#include "Engine.h"
#include "Actor.h"
#include "Map.h"

Engine::Engine() : gameStatus(STARTUP), fovRadius(10) {
    auto tileset = tcod::load_tilesheet("data/fonts/terminal.png", {16, 16}, tcod::CHARMAP_CP437);
    // Configure the window and renderer via TCOD_ContextParams
    // This replaces the old TCODConsole::initRoot()
    TCOD_ContextParams params{};
    params.tileset = tileset.get();
    params.columns = 80;
    params.rows = 50;
    params.window_title = "Tollmark";
    params.renderer_type = TCOD_RENDERER_SDL2;
    params.vsync = true;
    context_ = std::make_unique<tcod::Context>(params);

    // The console is the off-screen tile buffer (width x height in tiles)
    console_ = std::make_unique<tcod::Console>(80, 50);

    player = new Actor(40, 25, '@', "player", tcod::ColorRGB(255, 255, 255));
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
    int dx = 0, dy = 0;
    // SDL_SCANCODE values replace the deprecated TCODK_ constants
    switch(key) {
        case SDL_SCANCODE_UP:    dy = -1; break;
        case SDL_SCANCODE_DOWN:  dy =  1; break;
        case SDL_SCANCODE_LEFT:  dx = -1; break;
        case SDL_SCANCODE_RIGHT: dx =  1; break;
        default: break;
    }
    if( gameStatus == STARTUP ) {
        map->computeFov();
        gameStatus = IDLE;
    }

    if (dx != 0 || dy != 0) {
        gameStatus = NEW_TURN;
        if ( player->moveOrAttack(player->x + dx, player->y + dy) ) {
            map->computeFov();
        }
    }

    if ( gameStatus == NEW_TURN ) {
        for (Actor* actor : actors) {
            if (actor != player) {
                actor->update();
            }
        }
    }
}

void Engine::render() {
    // Clear the console to black before drawing each frame
    console_->clear();

    // Draw map tiles (sets each tile's background color)
    map->render(*console_);

    // Draw all actors (sets each tile's character and foreground color)
    for (Actor* actor : actors) {
        if( map->isInFov(actor->x, actor->y) ) {
            actor->render(*console_);
        }
    }

    // Present the finished console to the screen
    // This replaces the old TCODConsole::flush()
    context_->present(*console_);
    map->computeFov(); // Recompute FOV after rendering to update explored tiles
    computeFov = false; // Reset FOV computation flag until next movement
}