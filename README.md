# Momentum-Engine-A-Dynamic-Flight-and-Collision-Simulator

![Graphics API](https://img.shields.io/badge/Graphics_API-OpenGL-512BD4?style=flat)
![Language](https://img.shields.io/badge/Language-C++-2EA44F?style=flat)
![Framework](https://img.shields.io/badge/Framework-GLUT%20%2F%20FreeGLUT-CC2927?style=flat)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20macOS%20%7C%20Linux-0078D4?style=flat)
![License](https://img.shields.io/badge/License-MIT-D4AC0D?style=flat)

A fast-paced 2D arcade survival game where players pilot an airplane through an endless scrolling sky, dodging incoming obstacles and collecting coins. Dynamic backgrounds create a strong sense of speed across 6 distinct worlds, while controls let players adjust altitude in real time. Each coin collected increases the score, while three collisions result in game over.

---

## 🌟 Key Features

* **6 Dynamic Scrolling Worlds:** Seamless automatic transitions (every ~10 seconds at 60 FPS) or instant manual scene toggling across six procedurally drawn environments.
* **Arcade Flight & Collision Engine:** Real-time distance-based collision detection against airborne obstacles, dual-tier coin collection (`+10` and `+20` points), a 3-life survival system, red screen damage flash, and nose-dive crash physics.
* **100% Procedural OpenGL Rendering:** Built entirely with mathematical OpenGL primitives (`GL_POLYGON`, `GL_QUADS`, `GL_TRIANGLE_FAN`, cubic Bézier curves, and alpha blending) with zero external image dependencies.
* **Multi-Layer Parallax & Animations:** Moving highway traffic, walking pedestrians, underground metro trains, sailing riverboats, ocean ships, and pulsing building/tower beacons.
* **Cross-Platform Ready:** Preprocessor directives built in for seamless compilation on **macOS**, **Windows**, and **Linux**.

---

## 🌍 Scene Breakdown

| Scene | Environment | Highlights & Details |
| :--- | :--- | :--- |
| **Scene 1** | **Airport Takeoff** | Runway markings, control tower with pulsing beacon, twin towers, brick perimeter wall, and multi-phase aircraft takeoff with retracting landing gear. |
| **Scene 2** | **Bustling City & Highway** | Modern skyscrapers (including Burj Khalifa & Spire Tower), billboards, street lamps with light cones, walking pedestrians, and multi-lane highway traffic. |
| **Scene 3** | **Underground Metro** | Split-level cross-section view featuring snow-capped mountains, pine forests, soil strata layers, and a 5-car animated underground metro train. |
| **Scene 4** | **Serene Village** | Winding cubic Bézier curve dirt roads, rural houses, crop fields, fences, villagers, a multi-wave river, and animated sailboats. |
| **Scene 5** | **Sunset Beach** | Gradient sunset sky with horizon haze, ocean waves with shoreline foam, 4,000+ procedural sand grains, palm trees, lounge chairs, and a moving cruise ship. |
| **Scene 6** | **Desert Night** | Starry night sky (430+ stars), crescent moon with craters, desert mountains, cacti, glowing campfires with illuminated tents, and night highway cars with headlights. |

---

## 🎮 Controls & How to Play

### Keyboard Controls

| Key | Action |
| :--- | :--- |
| <kbd>↑</kbd> (Up Arrow) | Ascend airplane (Scenes 2–6) |
| <kbd>↓</kbd> (Down Arrow) | Descend airplane (Scenes 2–6) |
| <kbd>1</kbd> | **Start / Restart Game** from Scene 1 (resets score & lives, auto-progresses through Scene 6) |
| <kbd>2</kbd> | Jump directly to **Scene 2** (City Highway) |
| <kbd>3</kbd> | Jump directly to **Scene 3** (Underground Metro) |
| <kbd>4</kbd> | Jump directly to **Scene 4** (Serene Village) |
| <kbd>5</kbd> | Jump directly to **Scene 5** (Sunset Beach) |
| <kbd>6</kbd> | Jump directly to **Scene 6** (Desert Night) |
| <kbd>Q</kbd> or <kbd>Esc</kbd> | Quit the simulator |

### Gameplay Mechanics
1. **Takeoff (Scene 1):** Watch your aircraft accelerate down the runway, retract its landing gear, and climb into the sky.
2. **Cruising & Survival (Scenes 2–6):** Once airborne, use the **Up** and **Down** arrow keys to adjust your altitude:
   * 🪙 **Yellow Coins:** `+10 Score`
   * 🟠 **Orange Bonus Coins:** `+20 Score`
   * 🎈 **Airborne Obstacles:** `-1 Life` (triggers a red collision flash)
3. **Game Over:** Colliding **3 times** causes the airplane to lose lift and crash. Press <kbd>1</kbd> on the Game Over screen to restart.

---

## 🛠️ Setup & Installation

### 1. Clone the Repository
```bash
git clone [https://github.com/enamulhasan01/Momentum-Engine-A-Dynamic-Flight-and-Collision-Simulator.git](https://github.com/enamulhasan01/Momentum-Engine-A-Dynamic-Flight-and-Collision-Simulator.git)
cd Momentum-Engine-A-Dynamic-Flight-and-Collision-Simulator