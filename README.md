# ♟ Chess Game — C++ Edition

<p align="center">
  <img src="https://img.shields.io/badge/Language-C%2B%2B17-blue?style=for-the-badge&logo=cplusplus"/>
  <img src="https://img.shields.io/badge/GUI-SFML%203-green?style=for-the-badge"/>
  <img src="https://img.shields.io/badge/Platform-Windows-lightgrey?style=for-the-badge&logo=windows"/>
  <img src="https://img.shields.io/badge/Paradigm-OOP-orange?style=for-the-badge"/>
  <img src="https://img.shields.io/badge/Status-Complete-brightgreen?style=for-the-badge"/>
</p>

<p align="center">
  A fully-featured two-player Chess game written in C++, playable in two modes:<br/>
  a rich <strong>SFML 3 graphical GUI</strong> and a colour-coded <strong>Windows Console</strong> interface.
</p>

---

## 📑 Table of Contents

1. [Overview](#overview)
2. [Features](#features)
3. [Project Structure](#project-structure)
4. [Class Hierarchy & OOP Design](#class-hierarchy--oop-design)
5. [How Each Piece Moves](#how-each-piece-moves)
6. [Game Logic Deep Dive](#game-logic-deep-dive)
7. [Save & Load System](#save--load-system)
8. [GUI Mode — SFML 3](#gui-mode--sfml-3)
9. [Console Mode](#console-mode)
10. [Getting Started](#getting-started)
11. [Dependencies](#dependencies)
12. [Controls & Input](#controls--input)
13. [File Reference](#file-reference)
14. [Authors & Credits](#authors--credits)

---

## Overview

This project is a complete Chess game built as a semester project for the **Object-Oriented Programming** course at **FAST-NUCES Faisalabad**. It demonstrates real-world application of OOP principles including inheritance, polymorphism, encapsulation, and abstraction — all applied to a working, playable game.

The game supports:
- A polished **SFML 3 GUI** with click-to-move piece selection, red flash for invalid/wrong-turn moves, captured piece display, and live save/load.
- A fully featured **console mode** with Unicode chess symbols, colour-coded squares using the Windows Console API, move highlighting, and ASCII art menus.
- Both modes share **identical chess logic** — the same `Board`, `Piece`, and move-validation classes power both interfaces.

---

## Features

### ✅ Chess Rules Implemented
| Rule | Supported |
|---|---|
| All standard piece movements | ✔ |
| Turn enforcement (White moves first) | ✔ |
| Pawn forward move (1 or 2 squares on first move) | ✔ |
| Pawn diagonal capture | ✔ |
| Knight L-shape movement | ✔ |
| Bishop diagonal movement | ✔ |
| Rook straight-line movement | ✔ |
| Queen (Rook + Bishop combined) | ✔ |
| King (1-square move + safe-square check) | ✔ |
| Castling (Kingside & Queenside) | ✔ |
| Check detection | ✔ |
| Checkmate detection | ✔ |
| Blocking own pieces | ✔ |
| Cannot move into check | ✔ |

### ✅ GUI Features (SFML 3)
- Click to select a piece → click again to move it
- **Green highlight** on the selected square
- **Red flash** when you click an opponent's piece (wrong turn) or attempt an invalid move
- Real-time **whose-turn indicator** with colour-coded badge
- **Captured pieces panel** showing all pieces taken by each player using Unicode chess glyphs
- **Move counter** displayed in the sidebar
- **Check warning** shown in the sidebar when a king is in check
- **Checkmate overlay** with winner announcement
- **Exit Game** button (returns to main menu)
- **ESC** key also returns to main menu
- **Rank/file labels** (a–h, 1–8) on the board edges

### ✅ Console Features
- Coloured board squares using Windows Console API (light grey / blue alternating)
- **Green** source square highlight and **dark green** destination highlight after each move
- Captured pieces string displayed above the board
- Move number display
- ASCII art main menu and CHECKMATE banner
- Animated loading screen with progress bar
- Sound feedback using `Beep()` on move, invalid input, and game over

### ✅ Save & Load
- Every move is **auto-saved** to `savedGame.txt` after it is played
- Player names are saved in the file header
- **Load Game** replays the entire saved game and lets you continue from where you left off
- Typing `exit` in console mode triggers an animated "Saving Game" screen and preserves the session
- Works across both GUI and console modes — a game saved in one mode can be loaded in the other

---

## Project Structure

```
Chess/
│
├── main.cpp                  # Entry point: launches GUI, falls back to console
│
├── libraries.h               # Central include hub for all standard headers
│
├── Piece.h                   # Abstract base class for all chess pieces
│
├── Pawn.h                    # Pawn class declaration
├── Rook.h                    # Rook class declaration
├── Knight.h                  # Knight class declaration
├── Bishop.h                  # Bishop class declaration
├── Queen.h                   # Queen class declaration
├── King.h                    # King class declaration
│
├── hiba_functions.h          # isValidMove() declarations
├── hiba_functions.cpp        # All 6 piece move-validation implementations
│                             # (Pawn, Rook, Knight, Bishop, Queen, King)
│
├── Board.h                   # Board class: grid, movePiece(), check/checkmate
├── Board.cpp                 # Board logic: initialize, display, move, check
│
├── Game.h                    # Game class: turn management, console game loop
├── Game.cpp                  # startNewGame() and loadGame() implementations
│
├── save_and_load_Game.h      # saveMove() / saveLoadedMove() declarations
├── save_and_load_Game.cpp    # File I/O helpers for saving moves
│
├── taha_functions.h          # Console UI declarations (menu, colours, effects)
├── taha_functions.cpp        # Console UI implementations
│
├── SFML_GUI.h                # GUI class declaration (AppState, Button, sfml_GUI)
├── SFML_GUI.cpp              # Full SFML 3 GUI implementation
│
├── savedGame.txt             # Auto-generated save file (created at runtime)
└── assets/
    └── DejaVuSans.ttf        # Required font for chess Unicode glyphs
```

---

## Class Hierarchy & OOP Design

```
Piece  (Abstract Base Class)
│   - color : string
│   - symbol : string
│   + getColor() : string
│   + getSymbol() : string
│   + isValidMove() = 0  ← Pure Virtual
│
├── Pawn
│     - isFirstMove : bool
│     + isValidMove()  ← forward, 2-step first move, diagonal capture
│
├── Rook
│     - hasMoved : bool  (used for castling eligibility)
│     + isValidMove()   ← straight lines, blocked-path check
│     + setMoved() / getMoved()
│
├── Knight
│     + isValidMove()   ← L-shape (2+1 squares), no path blocking
│
├── Bishop
│     + isValidMove()   ← diagonals only, path clear check
│
├── Queen
│     + isValidMove()   ← combines Rook + Bishop logic
│
└── King
      - hasMoved : bool  (used for castling eligibility)
      + isValidMove()   ← 1 square any direction + castling
      + setMoved() / getMoved()
```

```
Board
│   - grid[8][8] : Piece*   ← polymorphic 2D grid
│   + initialize()          ← places all 32 pieces
│   + display()             ← renders board to console
│   + movePiece()           ← validates + executes a move
│   + getPiece() / accessCell()
│   + findKing()
│   + isCheck()
│   + canEscape()
│   + isCheckmate()
```

```
Game
│   - board : Board
│   - turn : string
│   - capturedByWhite / capturedByBlack : string
│   + startNewGame()    ← full console game loop
│   + loadGame()        ← loads savedGame.txt, resumes play
```

```
sfml_GUI
│   - m_window : sf::RenderWindow
│   - m_state  : AppState  { Menu, NameInputWhite, NameInputBlack, Game, ConsoleMode }
│   - m_turn   : string
│   - m_capturedByWhite / m_capturedByBlack : string
│   - m_selRow / m_selCol : int      ← selected square
│   - m_flashActive / m_flashRow / m_flashCol  ← red flash state
│   + run()             ← main loop; returns false for console mode
│   + renderMenu()
│   + renderNameInput()
│   + renderGame()
│   + handleGameClick() ← turn enforcement, move, flash, checkmate
│   + loadSavedGame()   ← replays savedGame.txt
│   + openSaveFile() / appendMove() / closeSaveFile()
```

### OOP Concepts Used

| Concept | Where Applied |
|---|---|
| **Inheritance** | Pawn, Rook, Knight, Bishop, Queen, King all inherit from `Piece` |
| **Polymorphism** | `Board::movePiece()` calls `piece->isValidMove()` — different behaviour per type at runtime |
| **Abstraction** | `Piece::isValidMove()` is a pure virtual function; callers never need to know the piece type |
| **Encapsulation** | `Board::grid` is private; all access goes through `getPiece()`, `accessCell()`, `movePiece()` |
| **Dynamic dispatch** | `dynamic_cast<King*>` / `dynamic_cast<Rook*>` used to check castling eligibility |

---

## How Each Piece Moves

### ♙ Pawn
- Moves **forward** 1 square (White moves up the board, Black moves down).
- On its **first move** only, can advance 2 squares if both squares are empty.
- Captures **diagonally** (1 square forward-left or forward-right) only when an enemy piece occupies that square.
- The `isFirstMove` flag is reset to `false` after the pawn's first move.

### ♖ Rook
- Moves any number of squares **horizontally or vertically**.
- Cannot jump over pieces — the path must be completely clear.
- Tracks `hasMoved` for castling eligibility.

### ♘ Knight
- Moves in an **L-shape**: 2 squares in one direction then 1 square perpendicular (or vice versa).
- The **only piece that can jump** over other pieces — path blocking does not apply.

### ♗ Bishop
- Moves any number of squares **diagonally**.
- Path must be clear; cannot jump over pieces.
- Always stays on its starting colour.

### ♕ Queen
- Combines Rook and Bishop: moves any number of squares **horizontally, vertically, or diagonally**.
- Path must be clear in all cases.

### ♔ King
- Moves exactly **1 square** in any direction (horizontal, vertical, or diagonal).
- **Cannot move into a square that is attacked** by any enemy piece — validated by simulating the move and calling `isCheck()`.
- Supports **Castling** (both kingside and queenside):
  - King and the relevant Rook must not have moved previously.
  - No pieces between the King and Rook.
  - King must not currently be in check.
  - King must not pass through or land on an attacked square.

---

## Game Logic Deep Dive

### `Board::movePiece(sr, sc, dr, dc, turn, capturedByWhite, capturedByBlack)`

This is the single function through which every move — from both the GUI and the console — passes. It performs these checks in order:

1. **Bounds check** — source and destination must be within 0–7.
2. **Empty source** — there must be a piece at `(sr, sc)`.
3. **Turn check** — the piece's colour must match `turn`.
4. **Friendly fire** — destination must not contain a piece of the same colour.
5. **Piece rule validation** — calls the virtual `isValidMove()` on the piece.
6. **Check simulation** — the move is temporarily applied; if `isCheck(turn)` is still true, the move is reverted and rejected. This prevents the player from moving into check.
7. **Capture tracking** — if a piece is captured, its Unicode symbol is appended to `capturedByWhite` or `capturedByBlack`.
8. **Actual move** — the piece is moved on the grid.
9. **Castling rook** — if the King moved 2 squares, the corresponding Rook is repositioned automatically.
10. **Moved flags** — `King::setMoved()` and `Rook::setMoved()` are called as appropriate.

### `Board::isCheck(color)`

Scans the entire board for enemy pieces and tests whether any of them have a valid move to the friendly King's square. The enemy King is handled separately (adjacent-square comparison) to avoid infinite recursion.

### `Board::isCheckmate(color)`

Returns `true` only when:
1. `isCheck(color)` is `true` (the King is currently in check), **and**
2. `canEscape(color)` is `false` — every possible move for every friendly piece still leaves the King in check.

`canEscape()` works by iterating all friendly pieces, trying all 64 destination squares, temporarily applying each move, and checking if the King is still in check.

---

## Save & Load Systemm

### File format — `savedGame.txt`

```
WhitePlayerName
BlackPlayerName
sr sc dr dc
sr sc dr dc
sr sc dr dc
...
```

- Line 1: White player's name
- Line 2: Black player's name
- Each subsequent line: one move as four space-separated integers
  - `sr` = source row (0–7, where 0 is rank 8)
  - `sc` = source column (0–7, where 0 is file a)
  - `dr` = destination row
  - `dc` = destination column

### How saving works
- **Console mode**: `Game::startNewGame()` opens `savedGame.txt` at the start, writes the two player name lines, then calls `saveMove()` after every successful move to append one line.
- **GUI mode**: `sfml_GUI::openSaveFile()` does the same when a new game starts; `appendMove()` is called inside `handleGameClick()` after every valid move. The file is flushed immediately so no data is lost if the window is closed.

### How loading works
- **Console mode**: `Game::loadGame()` reads the name lines, then replays each move using `board.movePiece()` with alternating turns, showing the board state after each replayed move. Once all saved moves are replayed, it opens the file again in append mode and continues the live game.
- **GUI mode**: `sfml_GUI::loadSavedGame()` does the same — reconstructs the board, replays all moves silently, restores `m_capturedByWhite`, `m_capturedByBlack`, `m_turn`, and `m_moveNumber`, then re-opens the file in append mode.

A game saved from console mode can be loaded in GUI mode and vice versa.

---

## GUI Mode — SFML 3

### Application States

```
Menu
 ├── [New Game]        → NameInputWhite → NameInputBlack → Game
 ├── [Load Saved Game] → Game  (board replayed from savedGame.txt)
 ├── [Play Console]    → closes window, main() runs console loop
 └── [Exit]            → closes window

Game
 ├── Click piece (correct turn) → selected (green highlight)
 ├── Click piece (wrong turn)   → red flash on that square
 ├── Click destination          → move attempted via Board::movePiece()
 │    ├── Valid   → move applied, turn switches, file appended
 │    └── Invalid → red flash on source square
 ├── [Exit Game] button         → Menu
 └── ESC key                    → Menu
```

### Window Layout

```
┌─────────────────────────────────────────────────────────┐
│                                                         │
│   ┌───────────────────────────────┐  ┌───────────────┐  │
│   │                               │  │  WHOSE TURN   │  │
│   │         Chess Board           │  │  ───────────  │  │
│   │         (600 × 600 px)        │  │  White: name  │  │
│   │                               │  │  Black: name  │  │
│   │                               │  │  ───────────  │  │
│   │                               │  │  Captured by  │  │
│   │                               │  │  White: ♛♜   │  │
│   │                               │  │  ───────────  │  │
│   │                               │  │  Captured by  │  │
│   │                               │  │  Black: ♕♖   │  │
│   │                               │  │  ───────────  │  │
│   │                               │  │  Move: 14     │  │
│   └───────────────────────────────┘  │  ───────────  │  │
│                                      │  [Exit Game]  │  │
└─────────────────────────────────────────────────────────┘
```

### Font Requirement

Chess Unicode glyphs (♔♕♖♗♘♙♚♛♜♝♞♟, U+2654–U+265F) require a font that contains them. **Arial does not include these glyphs.**

**Place `DejaVuSans.ttf` in the same folder as your `.exe`.**

| Font | Source |
|---|---|
| DejaVuSans.ttf | https://dejavu-fonts.github.io/ (recommended, free) |
| FreeSerif.ttf | https://www.gnu.org/software/freefont/ |
| seguisym.ttf | Already on Windows at `C:\Windows\Fonts\seguisym.ttf` |

The code automatically tries all three locations as a fallback chain.

---

## Console Mode

The console mode is selected from the GUI main menu ("Play Console") or runs directly if SFML fails to initialise.

### Console Main Menu Options

| Key | Action |
|---|---|
| `1` | Start New Game |
| `2` | Load Saved Game |
| `3` | Instructions |
| `4` | Credits |
| `5` | Exit |

### Move Input Format

Type the source square and destination square separated by a space:

```
e2 e4       (moves the pawn from e2 to e4)
g1 f3       (moves the knight from g1 to f3)
e1 g1       (kingside castling)
```

- Files: `a` through `h` (left to right)
- Ranks: `1` through `8` (bottom to top)
- Input is **case-insensitive**

Type `exit` to save the game and return to the menu.

### Console Board Display

```
   a  b  c  d  e  f  g  h
8  ♜  ♞  ♝  ♛  ♚  ♝  ♞  ♜   8
7  ♟  ♟  ♟  ♟  ♟  ♟  ♟  ♟   7
6                             6
5                             5
4                             4
3                             3
2  ♙  ♙  ♙  ♙  ♙  ♙  ♙  ♙   2
1  ♖  ♘  ♗  ♕  ♔  ♗  ♘  ♖   1
   a  b  c  d  e  f  g  h
```

- Light grey background = light squares
- Blue background = dark squares
- Light green background = source square of last move
- Dark green background = destination square of last move

---

## Getting Started

### Prerequisites

- Windows OS (console mode uses Windows Console API)
- A C++17-capable compiler (MSVC, MinGW-w64, Clang-cl)
- SFML 3.x library

### Build with MinGW (g++)

```bash
g++ -std=c++17 main.cpp Board.cpp Game.cpp taha_functions.cpp hiba_functions.cpp save_and_load_Game.cpp SFML_GUI.cpp -o Chess.exe -lsfml-graphics -lsfml-window -lsfml-system -mwindows
```

### Build with MSVC

1. Create a new Visual Studio project and add all `.cpp` files.
2. Link against `sfml-graphics.lib`, `sfml-window.lib`, `sfml-system.lib`.
3. Set the include path to your SFML `include/` directory.
4. Set C++ language standard to C++17.
5. Build in Release or Debug mode.

### After Building

1. Copy the SFML `.dll` files next to your `Chess.exe`.
2. Copy `DejaVuSans.ttf` next to your `Chess.exe`.
3. Run `Chess.exe`.

---

## Dependencies

| Library / Tool | Version | Purpose |
|---|---|---|
| SFML | 3.x | Windowed GUI, input events, text rendering |
| Windows Console API (`windows.h`) | Win32 | Console colours, `Beep()`, `SetConsoleTextAttribute()` |
| `conio.h` | MSVC / MinGW | `_kbhit()`, `_getch()` for real-time menu input |
| C++ Standard Library | C++17 | `fstream`, `string`, `thread`, `chrono`, `cmath` |
| DejaVuSans.ttf | Any | Unicode chess glyph rendering in SFML |

---

## Controls & Input

### GUI Mode

| Action | Input |
|---|---|
| Select a piece | Left-click on it |
| Move selected piece | Left-click on destination square |
| Deselect | Left-click the same square again |
| Re-select | Left-click another friendly piece |
| Exit to menu | Click **Exit Game** button or press **ESC** |

### Console Mode

| Action | Input |
|---|---|
| Make a move | Type e.g. `e2 e4` then Enter |
| Save and exit | Type `exit` then Enter |
| Navigate menu | Press `1`–`5` (no Enter needed) |

---

## File Reference

| File | Description |
|---|---|
| `main.cpp` | Entry point. Creates `sfml_GUI` and `Board`, launches GUI, falls back to console on user choice |
| `libraries.h` | Single include header for `iostream`, `string`, `windows.h`, `conio.h`, and the `color()` declaration |
| `Piece.h` | Abstract base class with `color`, `symbol`, pure virtual `isValidMove()` |
| `Pawn.h` | Pawn declaration with `isFirstMove` flag |
| `Rook.h` | Rook declaration with `hasMoved` flag |
| `Knight.h` | Knight: full inline `isValidMove()` (L-shape) |
| `Bishop.h` | Bishop declaration |
| `Queen.h` | Queen declaration |
| `King.h` | King declaration with `hasMoved` flag for castling |
| `hiba_functions.cpp` | Definitions of `isValidMove()` for Pawn, Rook, Bishop, Queen, King; also `Board::findKing()`, `isCheck()`, `canEscape()`, `isCheckmate()` |
| `Board.h` | Board class: 8×8 `Piece*` grid, all public method declarations |
| `Board.cpp` | `Board()` constructor, `initialize()`, `display()`, `movePiece()` |
| `Game.h` | `Game` class: board, turn, captured strings, `startNewGame()`, `loadGame()` |
| `Game.cpp` | Full console game loops for new and loaded games |
| `save_and_load_Game.h/.cpp` | `saveMove()` writes one `sr sc dr dc` line to an open `ofstream` |
| `taha_functions.h/.cpp` | All console UI: `printCentered()`, `color()`, `loadingScreen()`, `printMainMenu()`, `callFunctions()`, `displayInstructions()`, `displayCredits()` |
| `SFML_GUI.h` | `AppState` enum, `Button` struct, `sfml_GUI` class declaration |
| `SFML_GUI.cpp` | Complete GUI implementation: rendering, event handling, piece drawing, save/load |
| `savedGame.txt` | Auto-created at runtime. Contains player names + one move per line |

---

## Authors & Credits

<table>
  <tr>
    <td align="center">
      <b>Muhammad Taha</b><br/>
      <code>25F-0755</code><br/>
      <a href="mailto:f250755@cfd.nu.edu.pk">f250755@cfd.nu.edu.pk</a><br/>
      <a href="https://github.com/TahaDevHub">github.com/TahaDevHub</a>
    </td>
    <td align="center">
      <b>Hiba Eman</b><br/>
      <code>25F-0596</code><br/>
      <a href="mailto:f250596@cfd.nu.edu.pk">f250596@cfd.nu.edu.pk</a>
    </td>
  </tr>
</table>

**Institution:** FAST-NUCES Faisalabad Campus

**Course:** Object-Oriented Programming

**Special Thanks:**
- **Sir Rizwan-Ul-Haq** — rizwan.haq@nu.edu.pk
- **Mam Amna Waheed** — amna.waheed@nu.edu.pk

---

> ♚ *Protect Your King & Enjoy!* ♔
