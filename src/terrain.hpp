#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include <SFML/Graphics.hpp>
#include <vector>

// Represents the lunar surface.
// Stores a series of connected points that form the ground.
class Terrain {
private:
    sf::VertexArray m_vertices; // The list of points that make up the terrain line
    float m_width;              // Total horizontal width of the terrain (matches window width)
    float m_height;             // Total vertical height (matches window height), used as fallback ground level

public:
    Terrain(float width, float height);

    // Randomly generates a new terrain by placing points at different heights
    void generate();

    // Draws the terrain as a white line strip on the given window
    void draw(sf::RenderWindow& window);

    // Given an X position, returns the Y (ground level) at that point using linear interpolation
    float getHeightAt(float x) const;

    // Returns true if the terrain segment at X is roughly flat (safe landing zone)
    bool isFlat(float x) const;
};

#endif