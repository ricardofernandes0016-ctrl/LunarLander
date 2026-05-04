# Lunar Lander 🚀

A 2D physics-based lunar landing simulator built with **C++** and **SFML 3**. This project explores vector physics, procedural generation, and Object-Oriented Programming (OOP) principles.

## 🎮 Current Features

* **Procedural Terrain Generation:** Every game session features a unique lunar surface generated using linear interpolation.
* **Full 2D Physics:** Real-time simulation of gravity, angular thrust, and rotational inertia.
* **Resource Management:** Limited fuel system that requires careful descent planning.
* **Dynamic Collision Detection:** The game calculates the exact terrain height beneath the ship to determine impact points.
* **Landing Conditions:** Success depends on maintaining a low vertical/horizontal speed and keeping the ship upright (angle near 0°).
* **Heads-Up Display (HUD):** Real-time tracking of velocity and fuel levels, with visual warnings when fuel is low.

## 🕹️ How to Play

1.  **Objective:** Land safely on any part of the procedural terrain.
2.  **Controls:**
    * `Left / Right Arrows`: Rotate the ship.
    * `Spacebar`: Fire main thrusters (consumes fuel).
    * `R Key`: Reset the game and generate a brand-new terrain.
3.  **Pro Tip:** Keep your total speed below **50.0** and your rotation within **15 degrees** of vertical to avoid a crash.

## 📂 Project Structure

* `/src`: Contains all source code files (`.cpp`) and headers (`.hpp`).
* `/assets`: Game textures (ship, destroyed state) and HUD fonts.

## 🛠️ Tech Stack

* **Language:** C++
* **Graphics Library:** SFML 3 (Simple and Fast Multimedia Library)
* **Mathematics:** Trigonometry for force decomposition and linear interpolation for ground height.