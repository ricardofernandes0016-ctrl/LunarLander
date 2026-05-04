# LunarLander

A 2D physics-based simulation built with **C++** and **SFML 3**.

## Overview
LunarLander is my first game development project, designed to explore C++ programming concepts and basic game physics. 
The objective is to pilot a spacecraft and land it safely on the lunar surface by managing descent velocity and fuel (thrust).

## Current Features
* **Physics Engine:** Real-time simulation of gravity, thrust, and vertical velocity.
* **Landing Logic:** Detects impact speed to determine if the landing was successful (Green) or a crash (Red).
* **HUD:** Real-time velocity display with 4-decimal precision for precise landings.
* **Responsive Controls:** Spacebar-activated thrusters to counteract gravitational pull.

## How to Play
1.  **Launch:** The game starts with the ship in mid-air.
2.  **Control:** Press and hold the **Spacebar** to apply upward thrust.
3.  **Objective:** Touch the bottom of the screen with a velocity lower than **0.1**. 

## Technical Goals
* Apply C++ Object-Oriented Programming (OOP) principles.
* Understand the SFML 3 event loop and rendering pipeline.
* Implement basic Euler integration for physics movement.

## Planned Features
* Horizontal movement and fuel consumption limits.
* Procedural terrain generation.
* Menu screens (Start, Game Over, and Restart functionality).