#include "Game.hpp"
#include <iomanip>
#include <sstream>
#include <cmath>
#include <cstdlib>

// Set up the window, all game objects, text, sounds and generate the first terrain
Game::Game()
    : window(sf::VideoMode({ 800, 600 }), "Lunar Lander"),
      terrain(800.f, 600.f),
      font("assets/verdana.ttf"),
      velocityText(font), fuelText(font), autopilotText(font),
      resultTitle(font), resultStats(font), promptText(font),
      menuTitle(font), menuControls(font), menuPrompt(font),
      state(GameState::MainMenu),
      shakeTimer(0.f), shakeMagnitude(0.f),
      totalScore(0), lastScore(0),
      landingSpeed(0.f), fuelRemaining(0.f),
      pidVertical(1.5f, 0.4f, 0.8f, 0.0f, 1.0f),         // vertical: PID owns full thrust [0,1]
      pidHorizontal(0.04f, 0.005f, 0.06f, -1.0f, 1.0f), // horizontal: steer toward pad
      autopilotActive(false), pidLastThrust(0.f)
{
    velocityText.setCharacterSize(18);
    fuelText.setCharacterSize(18);

    resultTitle.setCharacterSize(42);
    resultStats.setCharacterSize(20);
    promptText.setCharacterSize(18);

    menuTitle.setCharacterSize(52);
    menuTitle.setStyle(sf::Text::Bold);
    menuTitle.setString("LUNAR LANDER");
    menuTitle.setFillColor(sf::Color::White);

    menuControls.setCharacterSize(18);
    menuControls.setFillColor(sf::Color(180, 180, 180));
    menuControls.setString(
        "LEFT / RIGHT   Rotate ship\n"
        "SPACE          Fire engine\n"
        "A (hold)       Autopilot (PID descent)\n"
        "R              Restart after landing"
    );

    autopilotText.setCharacterSize(18);

    menuPrompt.setCharacterSize(20);
    menuPrompt.setFillColor(sf::Color::Yellow);
    menuPrompt.setString("Press ENTER to start");

    player.loadAssets("assets/spaceship.png", "assets/spaceship_destroyed.png", "assets/movement.wav");
    player.reset({ 400.f, 100.f });

    if (explosionBuffer.loadFromFile("assets/explosion.wav"))
        explosionSound.emplace(explosionBuffer);

    terrain.generate(); // build the ground before the first frame
}

void Game::calculateScore(float speed, float fuel, bool isOnPad) {
    int fuelBonus  = static_cast<int>(fuel) * 10;                          // reward fuel conservation
    int speedBonus = static_cast<int>(std::max(0.f, 500.f - speed * 5.f)); // slower = higher bonus
    int score = fuelBonus + speedBonus;
    if (isOnPad) score *= 3; // 3x multiplier for hitting the designated pad
    lastScore   = score;
    totalScore += score;
}

void Game::checkCollisions() {
    float groundHeight = terrain.getHeightAt(player.getPosition().x); // ground Y below the ship

    if (player.getPosition().y >= groundHeight && state == GameState::Playing) {
        float angle    = std::abs(player.getRotation());
        bool isUpright = (angle < 15.f || angle > 345.f); // within 15 degrees of straight up
        float speed    = player.getSpeed();
        bool isOnPad   = terrain.isOnLandingPad(player.getPosition().x);

        landingSpeed  = speed;
        fuelRemaining = player.getFuel();

        if (isOnPad && speed < 50.0f && isUpright) {
            calculateScore(speed, fuelRemaining, true);
            state = GameState::Success;
        } else {
            player.crash();
            if (explosionSound) explosionSound->play();
            lastScore      = 0;
            shakeTimer     = 0.6f;                           // shake duration in seconds
            shakeMagnitude = std::min(speed * 0.15f, 18.f); // harder crash = bigger shake
            state = GameState::Failure;
        }
    }
}

void Game::run() {
    sf::Clock clock; // measures real elapsed time between frames

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds(); // seconds since last frame (~0.016 at 60 FPS)

        processEvents();

        if (state == GameState::Playing) {
            player.handleInput(dt); // read keyboard, apply thrust and rotation

            if (autopilotActive) {
                float shipX     = player.getPosition().x;
                float shipY     = player.getPosition().y;
                float padX      = terrain.getPadCentreX();
                float padY      = terrain.getPadY();
                float distY     = padY - shipY;        // vertical distance to the pad (px)
                float currentVY = player.getVelocity().y;
                float currentVX = player.getVelocity().x;

                // Target descent speed scales with altitude: slower closer to the ground
                float targetVY = std::clamp(distY * 0.06f, 0.5f, 15.0f);

                // PID error = currentVY - targetVY: positive when falling too fast (needs thrust)
                pidLastThrust = pidVertical.calculate(currentVY, targetVY, dt);
                player.applyAutopilotThrust(pidLastThrust, dt);

                // Horizontal: convert X distance to a desired velocity, then correct toward it
                float xError    = padX - shipX;
                float desiredVX = xError * 0.4f;           // 1 px off = 0.4 u/s desired speed
                if (desiredVX >  40.f) desiredVX =  40.f;
                if (desiredVX < -40.f) desiredVX = -40.f;

                float approachCap = (std::abs(xError) < 80.f) ? 12.f : 40.f; // slow down near pad
                if (desiredVX >  approachCap) desiredVX =  approachCap;
                if (desiredVX < -approachCap) desiredVX = -approachCap;

                float vxError     = desiredVX - currentVX;
                float hCorrection = vxError * 0.08f; // gentle proportional correction on VX
                if (hCorrection >  1.f) hCorrection =  1.f;
                if (hCorrection < -1.f) hCorrection = -1.f;
                player.applyAutopilotHorizontal(hCorrection, dt);
            }

            player.update(dt);    // apply gravity and move the ship
            checkCollisions();    // detect ground contact

            if (player.isThrusting()) {
                // Exhaust position: 18px behind the ship in the direction opposite to thrust
                const float PI = 3.14159265f;
                float rad = player.getRotation() * (PI / 180.f);
                sf::Vector2f exhaustPos = player.getPosition() +
                    sf::Vector2f(std::sin(rad) * 18.f, -std::cos(rad) * 18.f) * -1.f;
                particles.emitFire(exhaustPos, player.getRotation());

                // Emit dust when the engine wash is close to the ground
                float groundY = terrain.getHeightAt(player.getPosition().x);
                if (groundY - player.getPosition().y < 80.f)
                    particles.emitDust({ player.getPosition().x, groundY });
            }

            particles.update(dt); // move, fade and remove expired particles
        }

        if (shakeTimer > 0.f)
            shakeTimer -= dt; // count down screen-shake duration

        render();
    }
}

void Game::processEvents() {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) window.close();

        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Enter && state == GameState::MainMenu)
                state = GameState::Playing; // start the game from the main menu

            if (keyPressed->code == sf::Keyboard::Key::A && state == GameState::Playing) {
                autopilotActive = !autopilotActive;
                // Always reset PID state when toggling — prevents stale integral causing a jolt
                pidVertical.reset();
                pidHorizontal.reset();
                pidLastThrust = 0.f;
            }

            if (keyPressed->code == sf::Keyboard::Key::R) {
                // Restart: keep total score but regenerate everything else
                state = GameState::Playing;
                shakeTimer = 0.f;
                autopilotActive = false;
                pidVertical.reset();
                pidHorizontal.reset();
                pidLastThrust = 0.f;
                player.reset({ 400.f, 100.f });
                terrain.generate();
                particles.clear();
            }
        }
    }
}

void Game::renderHUD() {
    float speed = player.getSpeed();
    std::stringstream ss;
    ss << "Speed: " << std::fixed << std::setprecision(1) << speed;
    velocityText.setString(ss.str());
    velocityText.setFillColor(sf::Color::White);
    velocityText.setPosition({ 10.f, 10.f });
    window.draw(velocityText);

    std::stringstream fs;
    fs << "Fuel:  " << std::fixed << std::setprecision(1) << player.getFuel();
    fuelText.setString(fs.str());
    fuelText.setFillColor(player.getFuel() < 20.f ? sf::Color::Red : sf::Color::White); // red = low fuel warning
    fuelText.setPosition({ 10.f, 32.f });
    window.draw(fuelText);

    {
        std::stringstream as;
        if (autopilotActive) {
            as << "[A] AUTOPILOT ON  |  Thrust: "
               << std::fixed << std::setprecision(2) << pidLastThrust
               << "  |  VY: "
               << std::fixed << std::setprecision(1) << player.getVelocity().y
               << "  |  Pad X: " << static_cast<int>(terrain.getPadCentreX());
            autopilotText.setFillColor(sf::Color::Cyan);
        } else {
            as << "[A] AUTOPILOT OFF";
            autopilotText.setFillColor(sf::Color(100, 100, 100));
        }
        autopilotText.setString(as.str());
        autopilotText.setPosition({ 10.f, 54.f });
        window.draw(autopilotText);
    }

    // Score in the top-right corner
    std::stringstream sc;
    sc << "Score: " << totalScore;
    velocityText.setString(sc.str());
    velocityText.setFillColor(sf::Color::Yellow);
    auto bounds = velocityText.getLocalBounds();
    velocityText.setPosition({ 800.f - bounds.size.x - 10.f, 10.f });
    window.draw(velocityText);
}

void Game::renderMainMenu() {
    // --- Dark background ---
    sf::RectangleShape bg({ 800.f, 600.f });
    bg.setFillColor(sf::Color(10, 10, 30));
    window.draw(bg);

    // --- Title centred near the top ---
    auto titleBounds = menuTitle.getLocalBounds();
    menuTitle.setOrigin({ titleBounds.size.x / 2.f, 0.f });
    menuTitle.setPosition({ 400.f, 120.f });
    window.draw(menuTitle);

    // --- Controls legend centred in the middle ---
    auto ctrlBounds = menuControls.getLocalBounds();
    menuControls.setOrigin({ ctrlBounds.size.x / 2.f, 0.f });
    menuControls.setPosition({ 400.f, 280.f });
    window.draw(menuControls);

    // --- "Press ENTER" prompt near the bottom ---
    auto promptBounds = menuPrompt.getLocalBounds();
    menuPrompt.setOrigin({ promptBounds.size.x / 2.f, 0.f });
    menuPrompt.setPosition({ 400.f, 460.f });
    window.draw(menuPrompt);
}

void Game::renderResultScreen() {
    // --- Semi-transparent dark overlay so the terrain is still visible behind ---
    sf::RectangleShape overlay({ 800.f, 600.f });
    overlay.setFillColor(sf::Color(0, 0, 0, 160));
    window.draw(overlay);

    // --- Title: green for success, red for failure ---
    if (state == GameState::Success) {
        resultTitle.setString("MISSION SUCCESS");
        resultTitle.setFillColor(sf::Color::Green);
    } else {
        resultTitle.setString("CRITICAL FAILURE");
        resultTitle.setFillColor(sf::Color::Red);
    }
    // Centre the title horizontally
    auto titleBounds = resultTitle.getLocalBounds();
    resultTitle.setOrigin({ titleBounds.size.x / 2.f, 0.f });
    resultTitle.setPosition({ 400.f, 140.f });
    window.draw(resultTitle);

    // --- Flight statistics ---
    std::stringstream stats;
    stats << "Impact Speed  : " << std::fixed << std::setprecision(1) << landingSpeed  << " u/s\n"
          << "Fuel Remaining: " << std::fixed << std::setprecision(1) << fuelRemaining << " units\n";

    if (state == GameState::Success) {
        stats << "Landing Score : +" << lastScore << "\n"
              << "Total Score   :  " << totalScore;
    } else {
        stats << "No score awarded.\n"
              << "Total Score   :  " << totalScore;
    }

    resultStats.setString(stats.str());
    resultStats.setFillColor(sf::Color::White);
    auto statsBounds = resultStats.getLocalBounds();
    resultStats.setOrigin({ statsBounds.size.x / 2.f, 0.f });
    resultStats.setPosition({ 400.f, 260.f });
    window.draw(resultStats);

    // --- Restart prompt ---
    promptText.setString("Press R to try again");
    promptText.setFillColor(sf::Color(180, 180, 180));
    auto promptBounds = promptText.getLocalBounds();
    promptText.setOrigin({ promptBounds.size.x / 2.f, 0.f });
    promptText.setPosition({ 400.f, 420.f });
    window.draw(promptText);
}

void Game::render() {
    window.clear();

    // --- Screen shake: offset the camera view randomly while shakeTimer is active ---
    sf::View view = window.getDefaultView();
    if (shakeTimer > 0.f) {
        // Random offset scaled by magnitude — decreases as the timer winds down
        float ratio   = shakeTimer / 0.6f;  // Goes from 1.0 down to 0.0
        float offsetX = (static_cast<float>(rand() % 201) - 100.f) / 100.f * shakeMagnitude * ratio;
        float offsetY = (static_cast<float>(rand() % 201) - 100.f) / 100.f * shakeMagnitude * ratio;
        view.move({ offsetX, offsetY });
    }
    window.setView(view);

    // Draw the terrain, particles and ship only when the game is active
    if (state != GameState::MainMenu) {
        terrain.draw(window);
        particles.draw(window);
        window.draw(player.getSprite());
    }

    // Reset to default view before drawing HUD so it never shakes
    window.setView(window.getDefaultView());

    if (state == GameState::Playing) {
        renderHUD();
    } else if (state == GameState::MainMenu) {
        renderMainMenu();
    } else {
        renderResultScreen();
    }

    // Flip the back buffer to the screen
    window.display();
}


