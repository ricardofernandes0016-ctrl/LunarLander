#include "Game.hpp"
#include <iomanip>
#include <sstream>

// Constructor: initializes all game objects and prepares everything before the loop starts
Game::Game() : window(sf::VideoMode({ 800, 600 }), "Lunar Lander"), terrain(800.f, 600.f), isGameOver(false), font("assets/verdana.ttf"), velocityText(font), fuelText(font) {
    // Configure the HUD text elements
    velocityText.setFont(font);
    velocityText.setCharacterSize(30);
    fuelText.setFont(font);
    fuelText.setCharacterSize(20);

    // Load ship textures and place it at the starting position
    player.loadAssets("assets/spaceship.png", "assets/spaceship_destroyed.png");
    player.reset({ 400.f, 100.f });

    // Generate the initial terrain so the ground exists before the first frame
    terrain.generate();
}

void Game::checkCollisions() {
    // Get the terrain height directly below the ship's current X position
    float groundHeight = terrain.getHeightAt(player.getPosition().x);

    // Check if the ship has reached or passed ground level
    if (player.getPosition().y >= groundHeight && !isGameOver) {
        float angle = std::abs(player.getRotation());

        // The ship is considered upright if its angle is within 15 degrees of straight up
        // (accounts for both near 0° and near 360°)
        bool isUpright = (angle < 15.f || angle > 345.f);

        // Calculate total speed from both velocity components
        float speed = std::sqrt(std::pow(player.getVelocity().x, 2) + std::pow(player.getVelocity().y, 2));

        // Successful landing: slow enough and pointing upward
        if (speed < 50.0f && isUpright) {
            isGameOver = true; // Success!
        } else {
            // Too fast or tilted — the ship crashes
            player.crash();
            isGameOver = true;
        }
    }
}

void Game::run() {
    // The clock measures real time between frames to keep physics frame-rate independent
    sf::Clock clock;

    while (window.isOpen()) {
        // dt = time elapsed since last frame (in seconds), e.g. 0.016 at 60 FPS
        float dt = clock.restart().asSeconds();

        // Handle window events (close, reset key)
        processEvents();

        // Only update game logic while the game is still active
        if (!isGameOver) {
            player.handleInput(dt); // Read keyboard and adjust velocity
            player.update(dt);      // Apply gravity and move the ship
            checkCollisions();      // Check if the ship has touched the ground
        }

        // Always render, even after game over (so the final state is visible)
        render();
    }
}

void Game::processEvents() {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) window.close();
        
        // RESET: Listen for 'R' key to restart the game state
        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::R) {
                isGameOver = false;
                player.reset({ 400.f, 100.f });
                terrain.generate();
            }
        }
    }
}

void Game::render() {
    // Clear the previous frame (fills the window with black)
    window.clear();

    // Calculate the ship's total speed from its X and Y velocity components
    std::stringstream ss;
    float speed = std::sqrt(
        std::pow(player.getVelocity().x, 2) +
        std::pow(player.getVelocity().y, 2)
    );

    // Build and position the speed HUD string
    ss << "Speed: " << std::fixed << std::setprecision(2) << speed;
    velocityText.setString(ss.str());
    velocityText.setPosition({ 20.f, 20.f });

    // Build and position the fuel HUD string
    std::stringstream fuelStream;
    fuelStream << "Fuel: " << std::fixed << std::setprecision(1) << player.getFuel();
    fuelText.setString(fuelStream.str());
    fuelText.setPosition({ 20.f, 60.f });

    // Turn the fuel text red when fuel drops below 20 to warn the player
    if (player.getFuel() < 20.f)
        fuelText.setFillColor(sf::Color::Red);
    else
        fuelText.setFillColor(sf::Color::White);

    // Draw everything in order: terrain first (background), then ship, then HUD on top
    terrain.draw(window);
    window.draw(player.getSprite());
    window.draw(velocityText);
    window.draw(fuelText);

    // Flip the back buffer to the screen (makes the new frame visible)
    window.display();
}