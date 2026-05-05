#ifndef SHIP_HPP
#define SHIP_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <optional>

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
	bool m_isThrusting = false;               // True while the engine is firing this frame
	bool m_wasThrustingLastFrame = false;      // Used to detect when thrust starts

	sf::SoundBuffer thrustBuffer;
	std::optional<sf::Sound> thrustSound;

public:
	Ship();

	// Loads the ship textures and sounds from file. Returns false if either texture fails to load.
	bool loadAssets(const std::string& hPath, const std::string& dPath, const std::string& thrustSoundPath = "");

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

	// Returns the total speed (magnitude of the velocity vector)
	float getSpeed() const { return std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y); }

	// Returns true if the engine was fired this frame (used to trigger particle effects)
	bool isThrusting() const { return m_isThrusting; }

	// Applies a vertical-only thrust controlled by the autopilot.
	// power is expected in [0.0, 1.0]: 0 = no thrust, 1 = full thrust.
	// Consumes fuel proportionally to power, exactly like manual thrust.
	void applyAutopilotThrust(float power, float dt);

	// Applies a horizontal velocity correction controlled by the autopilot.
	// force is signed: positive = push right, negative = push left.
	// Uses a fraction of thrustMagnitude so it doesn't dominate the vertical axis.
	void applyAutopilotHorizontal(float force, float dt);
};

#endif