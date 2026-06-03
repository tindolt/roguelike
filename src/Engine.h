#pragma once
#include <libtcod.hpp>
#include <SDL3/SDL.h>
#include <memory>
#include <vector>

class Actor;
class Map;

class Engine {
public:
    enum GameStatus {
        STARTUP,
        IDLE,
        NEW_TURN,
        VICTORY,
        DEFEAT
    } gameStatus;
    
    std::vector<Actor*> actors;
    Actor* player;
    Map* map;

    int fovRadius;

    Engine();
    ~Engine();
    // Receives an SDL_Scancode from the event loop in main.cpp instead of
    // polling for events internally with the deprecated TCODSystem::checkForEvent()
    void update(SDL_Scancode key);
    void render();
private:
    // tcod::Context owns the window and renderer for its entire lifetime
    std::unique_ptr<tcod::Context> context_;
    // tcod::Console is the off-screen tile buffer we draw into each frame
    std::unique_ptr<tcod::Console> console_;
    bool computeFov;
};

extern Engine engine;