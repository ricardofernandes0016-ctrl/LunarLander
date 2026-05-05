#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include <SFML/Graphics.hpp>
#include <vector>

// Represents the lunar surface.
// Stores a series of connected points that form the ground.
class Terrain {
private:
    sf::VertexArray m_vertices;   // Terrain outline drawn as a connected line strip
    sf::VertexArray m_solidFill;  // Solid filled area below the terrain line (TriangleStrip)
    float m_width;                // Total horizontal width (matches window width)
    float m_height;               // Total vertical height (matches window height)

    // Landing pad boundaries — the only safe zone to land
    float m_padX1 = 0.f;
    float m_padX2 = 0.f;
    float m_padY  = 0.f;

public:
    Terrain(float width, float height);

    // Randomly generates the terrain and places one flat landing pad
    void generate();

    // Draws the solid fill and the terrain outline onto the window
    void draw(sf::RenderWindow& window);

    // Given an X position, returns the Y (ground level) using linear interpolation
    float getHeightAt(float x) const;

    // Returns true if the given X is within the landing pad zone
    bool isOnLandingPad(float x) const;

    // Returns the X centre of the landing pad
    float getPadCentreX() const { return (m_padX1 + m_padX2) * 0.5f; }

    // Returns the Y of the landing pad surface
    float getPadY() const { return m_padY; }
};

#endif