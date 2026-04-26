#include <libtcod.hpp>
#include "Actor.h"
#include "Map.h"
#include "Engine.h"
#include <SDL3/SDL.h>

// Global engine instance - accessible from any file via extern Engine engine; in Engine.h
Engine engine;

int main() {
    bool running = true;
    while (running) {
        // Process all pending SDL events before updating game state
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                // Forward keyboard input to the engine for movement/actions
                engine.update(event.key.scancode);
            }
        }
        // Draw the current frame; context.present() is called inside engine.render()
        engine.render();
    }
    return 0;
}