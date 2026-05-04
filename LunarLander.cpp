#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>
#include <iomanip> 
#include <sstream> 

int main() {
    // Initialize the game window (SFML 3 uses sf::VideoMode with brace initialization)
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "LunarLander");

    // --- HUD SETUP ---
    // Load font and configure the velocity display
    // Ensure "verdana.ttf" is in the project's working directory
    const sf::Font font("verdana.ttf");
    sf::Text VelocityText(font);
    VelocityText.setCharacterSize(30);
    VelocityText.setFillColor(sf::Color::White);

    // --- ENTITY SETUP ---
    // Define the lander as a 50x50 rectangle
    sf::RectangleShape ship(sf::Vector2f({ 50.f, 50.f }));
    ship.setFillColor(sf::Color::Yellow);

    // Set origin to center so rotations and positioning are intuitive
    ship.setOrigin({ 25.f, 25.f });

    // State variables for position and physics
    sf::Vector2f position{ 400.f, 100.f };
    sf::Vector2f velocity{ 0.f, 0.f };

    // Physics constants
    float gravity = 0.0005f;  // Constant downward acceleration
    float thrust = -0.0015f;  // Upward acceleration applied by user
    bool isGameOver = false;  // Prevents updates after landing/crashing

    // --- MAIN GAME LOOP ---
    while (window.isOpen()) {

        // SFML 3 Event Handling: Uses std::optional for safer event polling
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // Logic only updates if the game is currently active
        if (!isGameOver) {

            // 1. INPUT HANDLING
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
                velocity.y += thrust;
            }

            // 2. PHYSICS UPDATE (Euler Integration)
            velocity.y += gravity;      // Apply gravity to velocity
            position += velocity;       // Apply velocity to position
            ship.setPosition(position); // Sync the visual sprite with physics data

            // 3. COLLISION & WIN/LOSS LOGIC
            // 575.f accounts for the screen height minus half the ship's height
            if (position.y >= 575.f) {

                // Determine if impact speed was safe (Landing) or too high (Crash)
                if (velocity.y < 0.1f) {
                    ship.setFillColor(sf::Color::Green); // Safe Landing
                }
                else {
                    ship.setFillColor(sf::Color::Red);   // Crash
                }

                isGameOver = true;     // Stop physics updates
                position.y = 575.f;    // Snap to ground surface
                // Note: velocity is reset below after being captured by the HUD
            }
        }

        // --- HUD UPDATE ---
        // Format the velocity to a string with 4 decimal places
        // We use std::abs so the user sees a positive speed value
        std::stringstream ss;
        ss << "Velocity: " << std::fixed << std::setprecision(4) << std::abs(velocity.y);
        VelocityText.setString(ss.str());

        // Stop movement completely after impact for the UI update
        if (isGameOver) {
            velocity.y = 0;
        }

        // --- RENDERING ---
        window.clear();             // Clear previous frame
        window.draw(ship);          // Render the lander
        window.draw(VelocityText);  // Render the UI text
        window.display();           // Swap buffers to show the frame
    }

    return 0;
}