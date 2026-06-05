# LCOM Project - Dual Arena

## Getting Started

Welcome to the **Dual Arena** repository.

This project was developed as the final assignment for the LCOM course at FEUP.  
It is a 1v1 local multiplayer top down arena shooter built from scratch in C for the MINIX 3 operating system, directly interacting with the hardware peripherals studied throughout the semester.

The game is designed around asymmetric controls: one player uses the keyboard, while the other uses the mouse.

## Project Overview

**Dual Arena** is a real time arena shooter where two players compete locally in the same match. Each player automatically rotates in place. When the movement input is held, the player stops rotating and moves forward in the direction they are currently facing.

The objective is simple: move, shoot, and defeat the opponent.

## Main Features

- Local 1v1 multiplayer gameplay.
- Asymmetric controls: keyboard versus mouse.
- Real time player movement and shooting.
- Automatic player rotation mechanic.
- Menu navigation using both keyboard and mouse.
- Pause and exit handling.
- Sprite based graphical interface.
- Double buffering to reduce flickering.
- Modular code structure based on an MVC inspired architecture.

## Project Structure

The project follows an **MVC inspired architecture** to keep the code modular, maintainable, and easier to understand.

```text
src/
├── app/           Game state machine and main loop orchestration
├── model/         Game logic, physics, collision detection, and match state
├── view/          Rendering engine, double buffering, sprites, menus, and UI screens
├── controller/    Keyboard and mouse input mapping to game actions
├── devices/       Low-level device drivers interacting with MINIX and the hardware
└── lab*/          Reusable code and device- elated support from previous LCOM labs
```

## Used Devices

This project uses several hardware devices studied in LCOM:

### Timer

Used to control the main game loop and real time behaviour, including movement updates, rotation, shooting cooldowns, and frame timing.

### Keyboard

Used for Player 1 controls, menu navigation, pausing, and exiting the game.

### Mouse

Used for Player 2 controls and menu interaction. During gameplay, the mouse buttons are used for movement and shooting.

### Graphics Card

Used to render the arena, players, menus, UI elements, sprites, and other visual feedback in graphical mode.

### RTC

Used for time-related information, such as match history timestamps, if enabled in the final version of the project.

## Building and Running

The project must be compiled and executed inside the MINIX 3 LCOM environment.

### 1. Compile the project

Navigate to the project directory and compile using `make`:

```bash
cd /home/lcom/labs/project
make clean
make
```

### 2. Run the game

Execute the compiled binary with the appropriate permissions:

```bash
lcom_run proj
```

### 3. Stop the game

If you need to force stop the execution from another terminal:

```bash
lcom_stop proj
```

## Game Controls and Mechanics

 The game uses a dynamic auto rotation mechanic. Players automatically rotate in place. When the movement input is held, the rotation stops and the player moves forward in the direction they are currently facing.

### Player 1 - Keyboard

| Action | Key |
|---|---|
| Move Forward | `W` |
| Shoot | `SPACE` |

### Player 2 - Mouse

| Action | Input |
|---|---|
| Move Forward | `Left Mouse Button` |
| Shoot | `Right Mouse Button` |

### Menu and UI Navigation

| Action | Input |
|---|---|
| Navigate Menus | `Up / Down Arrows` or `Mouse Movement` |
| Select Option | `ENTER` or `Left Mouse Button` |
| Go Back / Exit | `ESC` |
| Pause Game | `P` |


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
