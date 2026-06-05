# LCOM Project - Dual Arena

## Getting started

Welcome to the **Dual Arena** repository. This project was developed as the final assignment for the LCOM course.
It is a 1v1 local multiplayer top-down arena shooter built entirely from scratch in C for the MINIX 3 operating system, directly interfacing with the hardware peripherals.

## Boilerplate

The project is structured following an **MVC (Model-View-Controller)** architecture to keep the code modular, maintainable, and clean:
- `src/app/`: Game state machine and main loop orchestration.
- `src/model/`: Game logic, physics, collision detection, and match history.
- `src/view/`: Rendering engine (double buffering), sprite management, UI screens.
- `src/controller/`: Device input mapping (Keyboard and Mouse) to game actions.
- `src/devices/` & `lab*/`: Low-level device drivers interacting with the MINIX kernel.

## Building and Running

To compile and play the game, follow these instructions inside the MINIX environment.

### 1. Compile the project
Navigate to the project directory and compile using `make`:
```bash
cd labs/project/
make clean
make
```

### 2. Run the game
Execute the compiled binary with the appropriate permissions:
```bash
lcom_run proj
```

### 3. Stop the game (in case of emergency)
If you need to force-stop the execution from another terminal:
```bash
lcom_stop proj
```

## Game Controls & Mechanics

**Dual Arena** uses a dynamic auto-rotation mechanic. Players automatically rotate in place. When a movement key is held, the rotation stops and the player moves forward in the direction they are currently facing.

### Player 1 (Keyboard)
* **Move Forward:** `W`
* **Shoot:** `SPACE`

### Player 2 (Mouse)
* **Move Forward:** `Left Mouse Button`
* **Shoot:** `Right Mouse Button`

### Menu & UI Navigation
* **Navigate Menus:** `Up / Down Arrows` or `Mouse Movement`
* **Select Option:** `ENTER` or `Left Mouse Button`
* **Go Back / Exit:** `ESC`
* **Pause Game:** `P`

## License
This project is for educational purposes under the scope of the LCOM course at FEUP.

## Declaration of Responsible AI Use

We declare that:

1. We are responsible for all code and documentation in this repository, and we understand that we must be able to explain and justify any part of it on request.  
2. We have used AI-based tools (e.g., code assistants, chatbots, or generators) only to support my learning, not to bypass the intended learning outcomes or any assessment rules.  
3. Wherever AI tools contributed to this work, we have:  
   - Used them within the limits set by the course policies and institutional regulations.  
   - Reviewed, tested, and, where necessary, edited the outputs, taking full responsibility for their correctness, originality, and legality.  
   - Ensured that no confidential, personal, or sensitive data were shared with AI tools.  
4. We have not used AI tools to generate complete solutions that we present as entirely our own unaided work, and we have avoided plagiarism, whether from AI outputs or other sources.  
5. If asked, we will provide details of which tools we used, for which files or parts of the project, and how we verified and adapted their outputs.

Signed: `Diogo Alves`, `Diogo Pérez`, `Gonçalo Paiva`
Date: `05/06/2026`

## Authors and acknowledgment

LCOM Project for group GRUPO_2LEIC07_4.
Group members:

Diogo Alves (up202307104@up.pt)
Diogo Pérez (up202406763@up.pt)
Gonçalo Paiva (up202309927@up.pt)
