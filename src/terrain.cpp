#include "Terrain.hpp"
#include <cmath>

// Store screen size and set the primitive types for the two draw layers
Terrain::Terrain(float width, float height)
    : m_width(width), m_height(height),
      m_vertices(sf::PrimitiveType::LineStrip),    // outline drawn as a connected line
      m_solidFill(sf::PrimitiveType::TriangleStrip) {} // solid ground below the line

void Terrain::generate() {
    m_vertices.clear();
    m_solidFill.clear();

    // Place the landing pad (80px wide) at a random X in the middle third of the screen
    float padWidth  = 80.f;
    float padStartX = 200.f + static_cast<float>(rand() % static_cast<int>(m_width - 400.f));
    float padEndX   = padStartX + padWidth;

    float currentY  = m_height - 100.f; // starting height of the terrain line
    float slope     = 0.f;              // current slope, updated each step
    float roughness = 2.0f;             // how much the slope can change per step

    bool  inPad      = false;
    float padLockedY = 0.f; // Y frozen at the moment we enter the pad zone

    for (float x = 0; x <= m_width; x += 10.f) {

        if (x >= padStartX && x <= padEndX) {
            if (!inPad) {
                padLockedY = currentY; // freeze height at the pad's entry point
                m_padY     = currentY;
                inPad      = true;
            }
            currentY = padLockedY; // keep the pad perfectly flat
            slope    = 0.f;
        } else {
            inPad = false;

            float randomVariation = (static_cast<float>(rand() % 101) / 100.f) - 0.5f; // [-0.5, 0.5]
            slope += randomVariation * roughness; // nudge the slope randomly
            slope *= 0.86f;                       // dampen so it doesn't grow forever
            currentY += slope;

            // keep terrain within the lower band of the screen
            if (currentY < m_height - 120.f) { currentY = m_height - 120.f; slope = 0.f; }
            if (currentY > m_height - 40.f)  { currentY = m_height - 40.f;  slope = 0.f; }
        }

        sf::Color color = (x >= padStartX && x <= padEndX) ? sf::Color::Yellow : sf::Color::White;
        m_vertices.append(sf::Vertex({ x, currentY }, color)); // terrain outline vertex

        // Two vertices per column: one on the surface, one at the bottom — forms the solid fill
        m_solidFill.append(sf::Vertex({ x, currentY }, sf::Color(80, 80, 80))); // surface
        m_solidFill.append(sf::Vertex({ x, m_height }, sf::Color(40, 40, 40))); // bottom
    }

    m_padX1 = padStartX;
    m_padX2 = padEndX;
}

float Terrain::getHeightAt(float x) const {
    // Safety check: no vertices means terrain hasn't been generated yet
    if (m_vertices.getVertexCount() == 0)
        return m_height;

    // Walk through each segment of the terrain line
    for (size_t i = 0; i < m_vertices.getVertexCount() - 1; ++i) {
        float x1 = m_vertices[i].position.x;
        float x2 = m_vertices[i + 1].position.x;

        // Check if the queried X falls within this segment
        if (x >= x1 && x <= x2) {
            float y1 = m_vertices[i].position.y;
            float y2 = m_vertices[i + 1].position.y;

            // Linear interpolation: t=0 → y1, t=1 → y2
            float t = (x - x1) / (x2 - x1);
            return y1 + t * (y2 - y1);
        }
    }

    // X is outside terrain range — return screen bottom as fallback
    return m_height;
}

bool Terrain::isOnLandingPad(float x) const {
    // Returns true only if the ship's X is within the flat landing pad zone
    return x >= m_padX1 && x <= m_padX2;
}

void Terrain::draw(sf::RenderWindow& window) {
    // Draw the solid grey fill first (behind the outline)
    window.draw(m_solidFill);

    // Draw the terrain outline on top (white line with yellow pad segment)
    window.draw(m_vertices);
}
