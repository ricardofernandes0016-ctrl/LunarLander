#ifndef SHIP_HPP
#define SHIP_HPP

#include <SFML/Graphics.hpp>
#include <cmath>

// Represents the player's spaceship.
// Handles movement physics, input, rendering, and state (alive/crashed).
class Ship {
private:
	sf::Texture healthyTexture;   // Texture shown when the ship is intact
	sf::Texture destroyedTexture; // Texture shown after a crash
	sf::Sprite sprite;            // The visual representation of the ship
	sf::Vector2f velocity;        // Current speed in X and Y directions (pixels/second)

	// Physics and control constants (all per-second, scaled by deltaTime)
	const float gravity = 80.0f;              // Constant downward acceleration
	const float thrustMagnitude = 140.0f;     // Acceleration applied when engine fires
	const float rotationSpeed = 90.0f;        // How fast the ship rotates (degrees/second)
	const float fuelConsumptionRate = 8.0f;   // Fuel burned per second (~12 seconds of full thrust)
	const float PI = 3.14159265f;             // Used for degree-to-radian conversion
	float m_fuel = 100.0f;                    // Current fuel level (starts full at 100)

public:
	Ship();

	// Loads the ship textures from file. Returns false if either file fails to load.
	bool loadAssets(const std::string& hPath, const std::string& dPath);

	// Reads keyboard input and updates velocity and fuel accordingly
	void handleInput(float deltaTime);

	// Applies gravity and moves the sprite based on current velocity
	void update(float deltaTime);

	// Resets the ship to a starting position with full fuel and no velocity
	void reset(sf::Vector2f startPos);

	// Switches to the destroyed texture and stops the ship
	void crash();

	// Getters — allow the Game class to read the ship's state without modifying it
	sf::Sprite& getSprite() { return sprite; }
	sf::Vector2f getVelocity() const { return velocity; }
	float getRotation() const { return sprite.getRotation().asDegrees(); }
	sf::Vector2f getPosition() const { return sprite.getPosition(); }
	float getFuel() const;
};

#endif