#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <optional>
#include "Ship.hpp"
#include "Terrain.hpp"
#include "ParticleSystem.hpp"
#include "PIDController.hpp"

// The three possible states the game can be in at any moment
enum class GameState {
	MainMenu, // Initial screen shown before the first game starts
	Playing,  // The ship is in flight and the player is in control
	Success,  // The ship landed safely on the pad — result screen shown
	Failure   // The ship crashed — result screen shown
};

// The main class that ties everything together.
// Owns the window, the player ship, the terrain, and all visual effects.
class Game {
private:
	sf::RenderWindow window;   // The OS window where everything is drawn
	sf::Font font;             // Font used for all on-screen text
	sf::Text velocityText;     // HUD element showing current speed
	sf::Text fuelText;         // HUD element showing remaining fuel
	sf::Text autopilotText;    // HUD element showing PID thrust (debug)

	Ship player;               // The player-controlled spaceship
	Terrain terrain;           // The randomly generated lunar surface
	ParticleSystem particles;  // Handles fire and dust particle effects

	GameState state;           // Tracks whether we are flying, succeeded, or failed

	// --- Autopilot (PID) ---
	PIDController pidVertical;    // Keeps vertical descent speed at target
	PIDController pidHorizontal;  // Steers the ship toward the landing pad centre
	bool autopilotActive;         // Toggled on/off by pressing 'A'
	float pidLastThrust;          // Last vertical thrust value (shown in HUD)

	// --- Screen shake ---
	float shakeTimer;          // How many seconds of shake remain
	float shakeMagnitude;      // Maximum pixel offset during the shake

	// --- Scoring ---
	int totalScore;            // Accumulated score across all successful landings
	int lastScore;             // Score earned on the most recent landing (shown on result screen)

	// --- Flight stats captured at the moment of landing (shown on result screen) ---
	float landingSpeed;        // Total speed at the moment of touchdown
	float fuelRemaining;       // Fuel left when the ship touched down

	// --- Sounds ---
	sf::SoundBuffer explosionBuffer;
	std::optional<sf::Sound> explosionSound;

	// --- Result screen text objects ---
	sf::Text resultTitle;      // "MISSION SUCCESS" or "CRITICAL FAILURE"
	sf::Text resultStats;      // Flight statistics (speed, fuel, score)
	sf::Text promptText;       // "Press R to retry" instruction

	// --- Main menu text objects ---
	sf::Text menuTitle;        // Game title on the main menu
	sf::Text menuControls;     // Controls legend
	sf::Text menuPrompt;       // "Press Enter to start" instruction

	// Calculates the score for a successful landing and updates totals
	void calculateScore(float speed, float fuel, bool isOnPad);

	// Draws the in-flight HUD (speed, fuel, score)
	void renderHUD();

	// Draws the full-screen result overlay after landing or crashing
	void renderResultScreen();

	// Draws the initial main menu screen
	void renderMainMenu();

public:
	Game();

	// Starts and drives the main game loop until the window is closed
	void run();

	// Handles window events (close button, keyboard reset with R)
	void processEvents();

	// Reserved for future logic updates (currently handled inline in run)
	void update(float dt);

	// Clears the screen and draws terrain, ship, particles, and HUD each frame
	void render();

	// Checks if the ship has touched the ground and decides win/crash
	void checkCollisions();
};

#endif