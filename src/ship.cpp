#include "Ship.hpp"

// Constructor: initializes textures, binds the sprite to the healthy texture, and zeroes out velocity
Ship::Ship() : healthyTexture(), destroyedTexture(), sprite(healthyTexture), velocity(0.f, 0.f) {}

bool Ship::loadAssets(const std::string& hPath, const std::string& dPath) {
	// Try to load both textures from disk — fail early if either is missing
	if (!healthyTexture.loadFromFile(hPath) || !destroyedTexture.loadFromFile(dPath))
		return false;

	// Apply the healthy texture and scale it down (original image is large)
	sprite.setTexture(healthyTexture);
	sprite.setTextureRect(sf::IntRect({ 0, 0 }, sf::Vector2i(healthyTexture.getSize())));
	sprite.setScale({ 0.15f, 0.15f });

	// Set the rotation origin to the center of the sprite
	// This makes the ship rotate around its own middle point instead of the top-left corner
	auto bounds = sprite.getLocalBounds();
	sprite.setOrigin({ bounds.size.x / 2.f, bounds.size.y / 2.f });
	return true;
}

void Ship::handleInput(float deltaTime) {
	// Left/Right arrows rotate the ship (changes facing direction, not movement directly)
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
		sprite.rotate(sf::degrees(-rotationSpeed * deltaTime));
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
		sprite.rotate(sf::degrees(rotationSpeed * deltaTime));

	// Space fires the engine — but only if there's fuel remaining
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && m_fuel > 0.f) {
		// Convert the ship's current angle to radians for trigonometry
		float radians = sprite.getRotation().asDegrees() * (PI / 180.f);

		// Decompose thrust into X and Y components based on the ship's facing angle:
		// sin(angle) gives horizontal push, -cos(angle) gives upward push
		velocity.x += std::sin(radians) * thrustMagnitude * deltaTime;
		velocity.y -= std::cos(radians) * thrustMagnitude * deltaTime;

		// Consume fuel, clamped so it never goes below 0
		m_fuel = std::max(0.f, m_fuel - fuelConsumptionRate * deltaTime);
	}
}

void Ship::update(float deltaTime) {
	// Gravity pulls the ship down every frame regardless of input
	velocity.y += gravity * deltaTime;

	// Move the sprite by its current velocity (scaled to frame time so speed is consistent)
	sprite.move(velocity * deltaTime);
}

void Ship::crash() {
	// Swap to the destroyed sprite to visually indicate the crash
	sprite.setTexture(destroyedTexture);
	sprite.setTextureRect(sf::IntRect({ 0, 0 }, sf::Vector2i(destroyedTexture.getSize())));

	// Stop all movement
	velocity = { 0.f, 0.f };
}

void Ship::reset(sf::Vector2f startPos) {
	// Restore the healthy texture and clear any crash state
	sprite.setTexture(healthyTexture);
	sprite.setTextureRect(sf::IntRect({ 0, 0 }, sf::Vector2i(healthyTexture.getSize())));

	// Reset orientation, position, movement, and fuel to initial values
	sprite.setRotation(sf::degrees(0.f));
	sprite.setPosition(startPos);
	velocity = { 0.f, 0.f };
	m_fuel = 100.0f;
}

float Ship::getFuel() const {
	return m_fuel;
}