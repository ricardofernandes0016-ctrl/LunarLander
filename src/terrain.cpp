#include "Terrain.hpp"
#include <cmath>

// Constructor: stores the screen dimensions and sets up the vertex array to draw as a connected line
Terrain::Terrain(float width, float height) : m_width(width), m_height(height), m_vertices(sf::PrimitiveType::LineStrip) {}

void Terrain::generate() {
    // Remove all previous terrain points
    m_vertices.clear();

    // Place a point every 50 pixels across the screen width
    // Each point gets a random Y height: between 50 and 250 pixels from the bottom
    for (float x = 0; x <= m_width; x += 50.f) {
        float y = m_height - (rand() % 200 + 50);
        m_vertices.append(sf::Vertex({ x, y }, sf::Color::White));
    }
}

float Terrain::getHeightAt(float x) const {
    // Safety check: if the terrain hasn't been generated yet, return the bottom of the screen
    if (m_vertices.getVertexCount() == 0) {
        return m_height;
    }

    // Loop through each line segment of the terrain
    for (size_t i = 0; i < m_vertices.getVertexCount() - 1; ++i) {
        float x1 = m_vertices[i].position.x;     // Left X of this segment
        float x2 = m_vertices[i + 1].position.x; // Right X of this segment

        // Check if the queried X falls within this segment
        if (x >= x1 && x <= x2) {
            float y1 = m_vertices[i].position.y;     // Left Y of this segment
            float y2 = m_vertices[i + 1].position.y; // Right Y of this segment

            // Linear interpolation: find the exact Y between the two endpoints
            // t=0 means at x1, t=1 means at x2
            float t = (x - x1) / (x2 - x1);
            return y1 + t * (y2 - y1);
        }
    }

    // X is outside the terrain range — return the bottom of the screen as fallback
    return m_height;
}

void Terrain::draw(sf::RenderWindow& window) {
    // Draws all the terrain vertices as a connected white line
    window.draw(m_vertices);
}