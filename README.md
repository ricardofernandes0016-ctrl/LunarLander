# Lunar Lander 🚀

A 2D physics-based lunar landing simulator built with **C++** and **SFML 3**.  
The project explores vector physics, procedural terrain generation, and autonomous flight via a PID controller.

---

## 🎮 Features

- **Procedural Terrain** — a unique lunar surface is generated every round using slope-based randomness and linear interpolation
- **2D Physics** — frame-rate-independent gravity, angular thrust decomposition, and fuel consumption
- **PID Autopilot** — automatically controls vertical descent and horizontal alignment to achieve a soft landing
- **Collision Detection** — exact ground height is interpolated at the ship's X position each frame
- **Score System** — rewards fuel conservation and landing speed; hitting the designated pad gives a 3× multiplier
- **Particles** — fire exhaust and ground dust effects driven by a lightweight particle pool
- **Screen Shake** — impact intensity scales with crash speed

---

## 🤖 The PID Autopilot

The autopilot uses a **PID (Proportional-Integral-Derivative)** controller to manage thrust autonomously.

| Term | Role |
| **P** — Proportional | Reacts to the current velocity error (falling too fast → more thrust) |
| **I** — Integral | Corrects steady-state drift over time (holds hover without manual tuning) |
| **D** — Derivative | Damps oscillation by reacting to the *rate of change* of the error |

**Vertical control** — the target descent speed is a linear ramp based on altitude (`targetVY = altitude × 0.06`, clamped to `[0.5, 15]` u/s), so the ship arrives at the ground nearly stationary.

**Horizontal control** — a cascaded system converts X distance to a desired horizontal velocity, then drives the actual velocity toward it, preventing overshoot.

**Anti-windup** — the integral term is clamped to avoid runaway accumulation when thrust is saturated.

---

## 🕹️ How to Play

**Objective:** land on the yellow pad with low speed and level orientation.

| Key | Action |
| `←` / `→` | Rotate the ship |
| `Space` | Fire main thruster |
| `A` | Toggle PID autopilot |
| `R` | Restart and regenerate terrain |

**Landing conditions:** speed below `50 u/s` and rotation within `15°` of vertical.

---

## 🛠️ Building

### Prerequisites
- [SFML 3](https://www.sfml-dev.org/download/) (Graphics + Audio modules)
- Visual Studio 2022 (or any C++17-capable compiler)

### Visual Studio
1. Clone the repo
2. Open `LunarLander.vcxproj`
3. Ensure the SFML include and lib paths are set in **Project → Properties → VC++ Directories**
4. Build and run (`F5`)

---

## 📂 Project Structure

```
src/
├── LunarLander.cpp     Entry point — creates Game and calls run()
├── Game.hpp/cpp        Main loop, autopilot logic, rendering, scoring
├── Ship.hpp/cpp        Physics, input handling, manual and autopilot thrust
├── Terrain.hpp/cpp     Procedural ground generation and collision queries
├── ParticleSystem.hpp/cpp  Fire and dust particle effects
└── PIDController.hpp   Generic reusable PID controller

assets/
├── spaceship.png / spaceship_destroyed.png
├── movement.wav / explosion.wav
└── verdana.ttf
```

---

## 🔧 Tech Stack

- **Language:** C++17
- **Graphics & Audio:** SFML 3
- **Physics:** custom — gravity, thrust decomposition via trigonometry, frame-rate-independent integration
- **Control theory:** discrete PID with anti-windup and derivative filtering