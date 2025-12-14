# TrabalhoCG — 2D Arena Game with OpenGL

This project implements a **2D arena-based game** developed in **C/C++ using OpenGL and FreeGLUT**, created as part of a Computer Graphics coursework assignment.

The game loads its entire initial configuration from an **SVG file provided via the command line**, interpreting colored circles as the arena, players, and obstacles. From this configuration, the application renders and simulates a real-time two-player game with movement, collision detection, shooting mechanics, and basic character animation.

---

## Features

- **SVG-driven initialization**
  - Blue circle: arena boundary  
  - Green circle: Player 1 spawn  
  - Red circle: Player 2 spawn  
  - Black circles: obstacles  

- **2D character rendering**
  - Players are composed of a head, torso, one controllable arm, and two animated legs
  - Character proportions are derived from the head radius defined in the SVG

- **Real-time movement and rotation**
  - Forward/backward movement and body rotation
  - Multiple simultaneous key inputs supported
  - Frame-rate-independent motion using delta time

- **Collision handling**
  - Player–arena, player–obstacle, and player–player collisions
  - All collision tests use circular approximations based on head radius

- **Shooting mechanics**
  - Player-controlled arm rotation
  - Projectile speed equals twice the player movement speed
  - Projectiles disappear when hitting obstacles or leaving the arena
  - Players lose one life when hit by a projectile

- **Game state management**
  - Each player starts with 3 lives
  - On-screen life counter
  - Game-over message displayed when a player loses all lives
  - Game restart using a single key press

---

## Controls

### Player 1
- Move forward: `W`
- Move backward: `S`
- Rotate left: `A`
- Rotate right: `D`
- Arm control: Mouse horizontal movement
- Shoot: Left mouse button

### Player 2
- Move forward: `O`
- Move backward: `L`
- Rotate left: `K`
- Rotate right: `Ç`
- Arm control: `4` (rotate left), `6` (rotate right)
- Shoot: `5`

---

## Build Instructions

### Requirements

- Linux environment
- C/C++ compiler (GCC or Clang)
- OpenGL
- FreeGLUT
- Make

### Compilation

A `Makefile` is provided with the following targets:

```bash
make all
make clean
```

This will generate an executable named:

```text
trabalhocg
```

No precompiled binaries are included in the repository.

---

## Running the Game

The program must be executed with the path to an SVG file as a command-line argument:

```bash
./trabalhocg path/to/arena.svg
```

The SVG file is used **only for initialization**. All rendering and animation are handled programmatically.

---

## SVG Format Requirements

The SVG file must contain only **circle elements** with the following color conventions:

- **Blue**: arena boundary (exactly one)
- **Green**: Player 1 initial position
- **Red**: Player 2 initial position
- **Black**: obstacles

Each circle must define:
- `cx` — x-coordinate of the center
- `cy` — y-coordinate of the center
- `r` — radius
- `fill` — color

---

## Project Structure

```
trabalhocg/
├── Makefile
├── README.md
├── assets/
│   └── arena.svg
├── include/
│   ├── game/
│   ├── world/
│   ├── entity/
│   ├── io/
│   └── math/
├── src/
│   ├── main.cpp
│   ├── game/
│   ├── world/
│   ├── entity/
│   ├── io/
│   └── math/
└── third_party/
    └── tinyxml2/
```

The codebase is modularized into components responsible for:
- Game state management
- Input handling
- Rendering
- Physics and collision detection
- SVG parsing
- Mathematical utilities

---

## Notes

- The window size is fixed at **500×500 pixels**
- The camera view is configured to fully contain the arena
- All movements and animations are time-based (delta time)
- The project is designed for clarity and ease of extension rather than graphical complexity

---

## License

This project is intended for academic use. Licensing details may be added later.
