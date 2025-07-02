
# Magikarp Swimmer – Computer Graphics Project

A third-person swimming demo featuring **Magikarp**, This project focuses on practicing **OBJ model loading**, **OpenGL shaders**, and **instanced rendering** techniques.

---

## 🎥 Demo

<<<<<<< HEAD
![Demo](assets/normal.gif)
![Demo](assets/sprint.gif)
=======
<video src="assets/demo.mp4" controls width="600"></video>
>>>>>>> befdd527b40ec1340ffa4a1acf3331fdfafdee38

---

## ✨ Features

### 🔧 Controls
- **Mouse**: Orbit camera
- **Scroll Wheel**: Zoom in/out
- **WASD**: Move the fish (supports diagonal movement, with smooth interpolation)
- **Q**: Boost speed + trigger motion effects (speed lines)
- **F4**: Toggle skybox rotation
- **F5–F8**: Switch skybox textures

---

## 🐟 Main Character – Magikarp
- Animated directly in the **vertex shader** (`wag` + `twist` movement)
- Movement and rotation use:
  - **Spherical interpolation** for smooth turning
  - **Linear interpolation** for speed-up/slow-down transitions
- `WorldMatrix` updates are calculated manually to support fluid directional control

---

## 🌊 Scene Design

### 🫧 Bubble Field
- Built using **instanced rendering**
- ~5000 transparent bubbles randomly placed within the skybox
- Each bubble moves independently using randomized motion
- Alpha blending enabled via `glEnable(GL_BLEND)`

### 🐠 Fish School
- Two types of fish (gray and yellow), orbiting in circles like in real-life aquariums
- Fish always face their direction of movement using **tangent-based orientation**
  - Required computing tangents and applying custom rotation matrices to fix model alignment
- Inner circle: gray fish; outer circle: yellow fish

---

## 🖼️ Skybox
- A dark, underwater-themed environment
- Supports runtime switching via F5–F8
- Rotatable using F4

---

## 🛠️ How to Run

1. Clone the repo:
   ```bash
   git clone https://github.com/nice3310/CG2023.git
   ```
2. Open the solution (`.sln`) file in Visual Studio
3. Build and run in **Release mode**

> ⚠️ Note: Debug binaries are not included to reduce repository size

---
## 📚 Technologies Used

- C++
- OpenGL
- GLSL shaders
- Instanced Rendering
- OBJ Model Loader (custom)

---
