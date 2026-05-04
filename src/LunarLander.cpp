#include "Game.hpp"

// Entry point of the application.
// Creates the Game object and starts the main loop.
int main() {
    // Create the game — this sets up the window, loads assets, and generates the terrain
    Game lunarLander;

    // Runs until the player closes the window
    lunarLander.run();

    return 0;
}