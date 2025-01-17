---

# Chess GUI

## Overview
This project implements a graphical chess game engine using **C++** with the **SDL2** library. It handles the representation of chess pieces, game logic, and rendering of a chessboard, allowing players to interact with a graphical interface for chess gameplay.

## Features
- **Graphical Interface**: Utilizes SDL2 and SDL2_image for rendering the chessboard and pieces.
- **Chess Mechanics**: Implements chess rules, including valid moves for pawns, rooks, knights, bishops, queens, and kings.
- **Board Setup**: Initializes and displays the chessboard with piece textures for both black and white sides.
- **Interactive Gameplay**: Supports user interaction with moves, captures, and piece movements using texture-based rendering.
- **Music & Sound Effects**: Incorporates sound effects for game events (e.g., moves, captures) with SDL2_mixer.

## Key Components
- **Game Logic**: Classes for different chess pieces handle move validation, rendering, and interactions.
- **Rendering System**: `Render_Window` class manages SDL2 rendering operations.
- **File-Based Assets**: Uses `.png` images for pieces and `.mp3` files for sound effects stored in the `res` directory.
- **Board Management**: The chessboard is implemented as an 8x8 grid with class-based square management.

## Dependencies
The project requires the following SDL2 libraries:
- **SDL2**
- **SDL2_image**
- **SDL2_ttf**
- **SDL2_mixer**

## How to Run
1. Install SDL2 and the required SDL2 extensions on your system.
2. Clone the repository and ensure the `res` folder contains all required assets.
3. Compile the project using a C++ compiler with SDL2 linked:
   ```bash
   g++ Sample.cpp -o ChessGame -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
   ```
4. Run the game:
   ```bash
   ./ChessGame
   ```

## Future Enhancements
- Add multiplayer functionality (local or online).
- Implement AI for single-player mode.
- Expand sound effects and animations for better gameplay experience.

---
