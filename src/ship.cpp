#include "Ship.hpp"

// Bind the sprite to the healthy texture and start with zero velocity
Ship::Ship() : healthyTexture(), destroyedTexture(), sprite(healthyTexture), velocity(0.f, 0.f) {}

bool Ship::loadAssets(const std::string& hPath, const std::string& dPath, const std::string& thrustSoundPath) {
	if (!healthyTexture.loadFromFile(hPath) || !destroyedTexture.loadFromFile(dPath))
		return false; // abort if either texture file is missing

	if (!thrustSoundPath.empty() && thrustBuffer.loadFromFile(thrustSoundPath)) {
		thrustSound.emplace(thrustBuffer);
		thrustSound->setLooping(true); // keep playing while engine is on
	}

	sprite.setTexture(healthyTexture);
	sprite.setTextureRect(sf::IntRect({ 0, 0 }, sf::Vector2i(healthyTexture.getSize())));
	sprite.setScale({ 0.15f, 0.15f }); // scale down the raw texture

	auto bounds = sprite.getLocalBounds();
	sprite.setOrigin({ bounds.size.x / 2.f, bounds.size.y / 2.f }); // rotate around centre
	return true;
}

void Ship::handleInput(float deltaTime) {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
		sprite.rotate(sf::degrees(-rotationSpeed * deltaTime)); // rotate left
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
		sprite.rotate(sf::degrees(rotationSpeed * deltaTime));  // rotate right

	m_isThrusting = false;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && m_fuel > 0.f) {
		m_isThrusting = true;
		float radians = sprite.getRotation().asDegrees() * (PI / 180.f); // angle in radians

		// Push the ship in the direction it is pointing
		velocity.x += std::sin(radians) * thrustMagnitude * deltaTime;
		velocity.y -= std::cos(radians) * thrustMagnitude * deltaTime;

		m_fuel = std::max(0.f, m_fuel - fuelConsumptionRate * deltaTime); // burn fuel
	}

	// Play or stop the thrust sound when the engine state changes
	if (m_isThrusting && !m_wasThrustingLastFrame)
		{ if (thrustSound) thrustSound->play(); }
	else if (!m_isThrusting && m_wasThrustingLastFrame)
		{ if (thrustSound) thrustSound->stop(); }
	m_wasThrustingLastFrame = m_isThrusting;
}

void Ship::update(float deltaTime) {
	velocity.y += gravity * deltaTime;      // gravity accelerates the ship downward every frame
	sprite.move(velocity * deltaTime);      // move the ship by its current velocity
}

void Ship::crash() {
	if (thrustSound) thrustSound->stop();
	m_wasThrustingLastFrame = false;

	sprite.setTexture(destroyedTexture);    // show the destroyed sprite
	sprite.setTextureRect(sf::IntRect({ 0, 0 }, sf::Vector2i(destroyedTexture.getSize())));
	velocity = { 0.f, 0.f };               // freeze the ship in place
}

void Ship::reset(sf::Vector2f startPos) {
	sprite.setTexture(healthyTexture);
	sprite.setTextureRect(sf::IntRect({ 0, 0 }, sf::Vector2i(healthyTexture.getSize())));
	sprite.setRotation(sf::degrees(0.f));  // face straight up
	sprite.setPosition(startPos);
	velocity  = { 0.f, 0.f };
	m_fuel    = 100.0f;
	m_isThrusting = false;
}

float Ship::getFuel() const {
	return m_fuel;
}

void Ship::applyAutopilotThrust(float power, float dt) {
	if (m_fuel <= 0.f) return;             // no fuel, no thrust
	if (power < 0.f) power = 0.f;
	if (power > 1.f) power = 1.f;
	if (power == 0.f) return;

	velocity.y -= thrustMagnitude * power * dt;                          // push upward
	m_fuel = std::max(0.f, m_fuel - fuelConsumptionRate * power * dt);  // burn fuel
	m_isThrusting = true;
}

void Ship::applyAutopilotHorizontal(float force, float dt) {
	// force is in [-1, 1]; scale it down so lateral corrections are gentle
	velocity.x += force * thrustMagnitude * 0.4f * dt;
	m_fuel = std::max(0.f, m_fuel - fuelConsumptionRate * std::abs(force) * 0.2f * dt);
}