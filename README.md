# 🐦 FlappyBird-in-C

*I'll just try to make the Flappy Bird game in C, this is my very first time programming a game in C.*

A classic Flappy Bird clone created entirely in C to be played in the Windows console/terminal. It features customised physics, an interactive menu, and dynamic settings.

## Contents
- [Features](#features)
- [Project Structure](#project-structure)
- [Requirements](#requirements)
- [Installation & Compilation](#installation--compilation)
- [How to Play](#how-to-play)
## Features
*   **Console Graphics:** Smooth rendering using ANSI characters and colours.
*   **State Machine:** Clean navigation between the main menu, game, settings, and Game Over screens.
*   **Dynamic Settings:** Change the screen resolution, FPS limit, pipe distance, and gap size directly from within the game.
*   **Auto-save:** Automatically saves your records, nickname, and settings in a `settings.cfg` file.

## Project Structure
The code is organised in a modular way:
```text
FlappyBird/
├── bin/                 # Folder where the compiled game is saved (.exe)
├── include/             # Headers (.h) with definitions and structures
│   ├── types.h          
│   ├── game.h           
│   ├── render.h         
│   ├── settings.h       
│   └── utils.h          
└── src/                 # Source code (.c) with all the logic
    ├── main.c           
    ├── game.c           
    ├── render.c         
    ├── settings.c       
    └── utils.c          
```

## Requirements
*   **Operating System:** Windows (currently uses `windows.h` for asynchronous keyboard input).
*   **Compiler:** GCC (MinGW is recommended for Windows).

## Installation & Compilation
1.  Open your terminal in the main project folder (where the `src` and `include` directories are located).
2.  Create the `bin` folder if it doesn't exist:
    ```bash
    mkdir bin
    ```
3.  Run the following command to compile and link all modules:
    ```bash
    gcc src/*.c -I./include -o bin/flappybird
    ```

## How to Play
1.  Run the compiled file from your terminal:
    ```bash
    ./bin/flappybird.exe
    ```
2.  **Menus:** Use the **UP** and **DOWN** arrows to navigate, and **ENTER** to select an option.
3.  **Playing:** Use the **SPACEBAR** or **UP ARROW** to make the bird flap and try to survive by passing through the gaps in the pipes.
4.  **Exit:** Press **ESC** mid-game to return to the main menu at any time.