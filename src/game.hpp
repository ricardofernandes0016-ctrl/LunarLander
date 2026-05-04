#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include "Ship.hpp"
#include "Terrain.hpp"

// The main class that ties everything together.
// Owns the window, the player ship, and the terrain.
// Drives the game loop: input -> update -> render.
class Game {
private:
	sf::RenderWindow window;  // The OS window where everything is drawn
	sf::Font font;            // Font used for all on-screen text
	sf::Text velocityText;    // HUD element showing current speed
	sf::Text fuelText;        // HUD element showing remaining fuel
	Ship player;              // The player-controlled spaceship
	bool isGameOver;          // True once the player has landed or crashed
	Terrain terrain;          // The randomly generated lunar surface

public:
	Game();

	// Starts and drives the main game loop until the window is closed
	void run();

	// Handles window events (close button, keyboard reset with R)
	void processEvents();

	// Reserved for future logic updates (currently handled inline in run)
	void update(float dt);

	// Clears the screen and draws terrain, ship, and HUD each frame
	void render();

	// Checks if the ship has touched the ground and decides win/crash
	void checkCollisions();
};

#endif