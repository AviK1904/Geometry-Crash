# Geometry Crash

A fast-paced, shape-based survival game developed in **C++** using the **SFML** library. This project implements a modular **Entity Component System (ECS)** to manage dynamic game objects and complex physics interactions.



## 🚀 Features

* **Custom ECS Engine**: High-performance architecture using `std::shared_ptr` and a centralized `EntityManager` for efficient memory handling.
* **Black Hole Special Attack**: A physics-driven ability (Right Click) that creates a localized gravity well, pulling enemies and fragments into the void using vector normalization.
* **Procedural Enemy Generation**: Enemies spawn with randomized polygons (triangles to decagons) and colors.
* **Lives & Invincibility System**: Includes a multi-life system with visual "blinking" invincibility frames after the player respawns.
* **State Management**: Features a functional Main Menu state with transitions to active gameplay.

## 🎮 Controls

| Action | Input |
| :--- | :--- |
| **Move Player** | `W`, `A`, `S`, `D` |
| **Standard Shoot** | `Left Click` |
| **Black Hole Special** | `Right Click` (Cooldowned) |
| **Toggle Fullscreen** | `F` |
| **Start / Pause** | `Space` or `P` |



## 🛠️ Technical Details

The game logic is divided into specialized **Systems**:
* **sMovement**: Handles vector-based velocity and gravity-well pulls.
* **sCollision**: Manages entity-to-entity overlaps and screen-boundary bounces.
* **sLifespan**: Controls the fading and cleanup of temporary entities.
* **sRender**: Draws shapes and UI text based on the current game state.

## 📦 How to Build and Run

1. **Clone the Repo**: `git clone https://github.com/AviK1904/Geometry-Crash.git`.
2. **Dependencies**: Requires **SFML** installed and linked in Visual Studio.
3. **Assets**: Ensure the `fonts/` folder and `config.txt` are in the executable's directory.
4. **Build**: Build in **Release x64** mode to avoid debug overhead.

---
