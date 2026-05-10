#pragma once
#ifndef SFML_GUI_H
#define SFML_GUI_H

#include <SFML/Graphics.hpp>
#include <string>
#include <optional>
#include <fstream>
#include "Board.h"

// ─────────────────────────────────────────────────────────────────────
//  App states
// ─────────────────────────────────────────────────────────────────────
enum class AppState
{
    Menu,
    NameInputWhite,
    NameInputBlack,
    Game,
    ConsoleMode
};

// ─────────────────────────────────────────────────────────────────────
//  Simple reusable button
// ─────────────────────────────────────────────────────────────────────
struct Button
{
    sf::RectangleShape shape;
    sf::Text           label;

    Button(sf::Font& font,
        const std::string& caption,
        sf::Vector2f position,
        sf::Vector2f size,
        unsigned int charSize = 22);

    bool contains(sf::Vector2f point) const;
    void draw(sf::RenderWindow& window) const;
    void setHovered(bool hovered);
    void setColors(sf::Color fill, sf::Color outline);
};

// ─────────────────────────────────────────────────────────────────────
//  Main GUI class
// ─────────────────────────────────────────────────────────────────────
class sfml_GUI
{
public:
    sfml_GUI();
    ~sfml_GUI();

    // Returns true  -> user played / closed the GUI
    // Returns false -> user chose console mode
    bool run(Board& board);

private:
    // ── Window & fonts ────────────────────────────────────────────────
    sf::RenderWindow m_window;
    sf::Font         m_font;       // ASCII UI text  (Arial)
    sf::Font         m_pieceFont;  // chess-glyph font (DejaVuSans / FreeSerif)

    // ── App state ─────────────────────────────────────────────────────
    AppState    m_state;
    std::string m_whiteName;
    std::string m_blackName;
    std::string m_inputBuffer;

    // ── Game state  (reset each new game) ─────────────────────────────
    std::string  m_turn = "White";
    std::string  m_capturedByWhite = "";   // UTF-8 piece symbols separated by spaces
    std::string  m_capturedByBlack = "";
    std::ofstream m_saveFile;              // stays open while a game is live
    int          m_moveNumber = 1;   // for display only

    int  m_selRow = -1;
    int  m_selCol = -1;

    // ── Red-flash  (wrong-turn click  OR  invalid move) ───────────────
    bool      m_flashActive = false;
    int       m_flashRow = -1;
    int       m_flashCol = -1;
    sf::Clock m_flashClock;
    static constexpr float FLASH_DURATION = 0.50f;

    // ── Checkmate / game-over ─────────────────────────────────────────
    bool        m_gameOver = false;
    std::string m_winnerText = "";

    // ── Layout constants ──────────────────────────────────────────────
    // Window:  1050 × 750
    // Board:   8 × 75 = 600 px, starts at x=30, y=75  → ends at x=630, y=675
    // Sidebar: x=650 .. x=1020  (370 px wide)
    static constexpr float WIN_W = 1050.f;
    static constexpr float WIN_H = 750.f;
    static constexpr float BOARD_X = 30.f;
    static constexpr float BOARD_Y = 75.f;
    static constexpr float CELL = 75.f;
    static constexpr float BOARD_PX = 8.f * CELL;   // 600
    static constexpr float SIDEBAR_X = 660.f;
    static constexpr float SIDEBAR_W = 370.f;

    // ── Menu buttons ─────────────────────────────────────────────────
    Button* m_btnNewGame = nullptr;
    Button* m_btnLoadGame = nullptr;
    Button* m_btnConsole = nullptr;
    Button* m_btnMenuExit = nullptr;

    // ── In-game buttons ───────────────────────────────────────────────
    Button* m_btnGameExit = nullptr;

    // ── Internal helpers ─────────────────────────────────────────────
    void processEvents(Board& board);
    void render(Board& board);

    void renderMenu();
    void renderNameInput();
    void renderGame(Board& board);

    void handleMenuClick(sf::Vector2f mp, Board& board);
    void handleNameInputText(char32_t unicode);
    void handleGameClick(sf::Vector2f mp, Board& board);

    // Load savedGame.txt into `board`, return false if file is empty/absent
    bool loadSavedGame(Board& board);

    // Open / close the save-file
    void openSaveFile();
    void closeSaveFile();
    void appendMove(int sr, int sc, int dr, int dc);

    // Reset all in-game state and re-initialise the board
    void resetGame(Board& board);

    void         drawPiece(Piece* p, int row, int col);
    sf::Vector2f cellToScreen(int row, int col) const;
    void         boardCoordsFromMouse(sf::Vector2f mouse, int& row, int& col) const;
    void         centreText(sf::Text& t, float cx, float cy);
};

#endif // SFML_GUI_H
