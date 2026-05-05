#include "ParticleSystem.hpp"
#include <cmath>
#include <cstdlib>

// Helper: returns a random float between min and max
static float randF(float min, float max) {
    return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
}

void ParticleSystem::emitFire(sf::Vector2f pos, float shipAngleDeg) {
    const float PI = 3.14159265f;

    // Convert the ship's angle to radians so we can find the exhaust direction.
    // The exhaust shoots out the BOTTOM of the ship, which is opposite to thrust (+ PI).
    float baseAngle = shipAngleDeg * (PI / 180.f) + PI;

    // Spawn several particles per call to create a dense flame effect
    for (int i = 0; i < 6; ++i) {
        // Spread the particles in a narrow cone around the exhaust direction
        float spread = randF(-0.3f, 0.3f);
        float angle  = baseAngle + spread;
        float speed  = randF(60.f, 140.f);

        Particle p;
        p.position    = pos;
        p.velocity    = { std::sin(angle) * speed, -std::cos(angle) * speed };
        p.lifetime    = randF(0.15f, 0.35f);
        p.maxLifetime = p.lifetime;
        p.radius      = randF(2.f, 5.f);

        // Randomly pick orange or yellow for variety
        p.color = (rand() % 2 == 0)
            ? sf::Color(255, 140, 0)   // Orange
            : sf::Color(255, 220, 0);  // Yellow

        m_particles.push_back(p);
    }
}

void ParticleSystem::emitDust(sf::Vector2f pos) {
    // Spawn a few grey particles that fly sideways like kicked-up regolith
    for (int i = 0; i < 3; ++i) {
        Particle p;
        p.position    = pos;

        // Dust sprays horizontally outward with a slight upward kick
        p.velocity    = { randF(-50.f, 50.f), randF(-30.f, -10.f) };
        p.lifetime    = randF(0.3f, 0.6f);
        p.maxLifetime = p.lifetime;
        p.radius      = randF(1.5f, 3.5f);
        p.color       = sf::Color(180, 170, 150); // Dusty grey-beige

        m_particles.push_back(p);
    }
}

void ParticleSystem::update(float dt) {
    for (auto& p : m_particles) {
        // Move the particle by its velocity
        p.position += p.velocity * dt;

        // Slow it down slightly so it doesn't shoot off forever
        p.velocity *= 0.95f;

        // Tick down the remaining lifetime
        p.lifetime -= dt;

        // Fade alpha proportionally: fully opaque when born, transparent when dying
        float ratio = p.lifetime / p.maxLifetime;
        p.color.a   = static_cast<uint8_t>(ratio * 255.f);
    }

    // Remove all particles whose lifetime has expired
    m_particles.erase(
        std::remove_if(m_particles.begin(), m_particles.end(),
            [](const Particle& p) { return p.lifetime <= 0.f; }),
        m_particles.end()
    );
}

void ParticleSystem::draw(sf::RenderWindow& window) {
    sf::CircleShape circle;

    for (const auto& p : m_particles) {
        circle.setRadius(p.radius);
        // Origin at centre so position is the circle's middle, not its top-left corner
        circle.setOrigin({ p.radius, p.radius });
        circle.setFillColor(p.color);
        circle.setPosition(p.position);
        window.draw(circle);
    }
}

void ParticleSystem::clear() {
    m_particles.clear();
}
