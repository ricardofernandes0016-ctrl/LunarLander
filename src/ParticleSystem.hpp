#ifndef PARTICLESYSTEM_HPP
#define PARTICLESYSTEM_HPP

#include <SFML/Graphics.hpp>
#include <vector>

// A single particle — a small circle that moves, fades, and dies over time
struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;   // Direction and speed of movement
    float lifetime;          // Remaining time alive (seconds)
    float maxLifetime;       // Total lifespan — used to calculate fade-out alpha
    sf::Color color;         // Base colour (alpha is overridden each frame based on lifetime)
    float radius;            // Size of the circle
};

// Manages a pool of particles for fire and lunar dust effects.
// Call the emit functions each frame to spawn new particles,
// then update() and draw() once per frame.
class ParticleSystem {
private:
    std::vector<Particle> m_particles; // All currently alive particles

public:
    // Spawns a burst of orange/yellow fire particles at the ship's exhaust point.
    // shipAngleDeg is the ship's current rotation so particles shoot in the right direction.
    void emitFire(sf::Vector2f pos, float shipAngleDeg);

    // Spawns grey dust particles at a ground contact point (used when thrusting near terrain).
    void emitDust(sf::Vector2f pos);

    // Moves all particles, fades them out, and removes dead ones. Call every frame.
    void update(float dt);

    // Draws all alive particles onto the window as filled circles.
    void draw(sf::RenderWindow& window);

    // Removes all particles instantly (used on game reset).
    void clear();
};

#endif
