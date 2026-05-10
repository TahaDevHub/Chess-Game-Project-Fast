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

    bool run(Board& board);

private:
    // ── Window & fonts ────────────────────────────────────────────────
    sf::RenderWindow m_window;
    sf::Font         m_font;
    sf::Font         m_pieceFont;

    // ── App state ─────────────────────────────────────────────────────
    AppState    m_state;
    std::string m_whiteName;
    std::string m_blackName;
    std::string m_inputBuffer;

    // ── Game state ────────────────────────────────────────────────────
    std::string   m_turn = "White";
    std::string   m_capturedByWhite = "";
    std::string   m_capturedByBlack = "";
    std::ofstream m_saveFile;
    int           m_moveNumber = 1;

    // ── FIX 2: flag set when "New Game" names are confirmed ───────────
    // handleGameClick() sees this on the first click and calls
    // resetGame(board) which has access to Board& (handleNameInputText
    // does not). This guarantees board.initialize() always runs before
    // the first move of a new game, clearing any loaded-game state.
    bool m_pendingNewGame = false;

    int  m_selRow = -1;
    int  m_selCol = -1;

    // ── Red-flash ─────────────────────────────────────────────────────
    bool      m_flashActive = false;
    int       m_flashRow = -1;
    int       m_flashCol = -1;
    sf::Clock m_flashClock;
    static constexpr float FLASH_DURATION = 0.50f;

    // ── Checkmate / game-over ─────────────────────────────────────────
    bool        m_gameOver = false;
    std::string m_winnerText = "";

    // ── Layout constants ──────────────────────────────────────────────
    static constexpr float WIN_W = 1050.f;
    static constexpr float WIN_H = 750.f;
    static constexpr float BOARD_X = 30.f;
    static constexpr float BOARD_Y = 75.f;
    static constexpr float CELL = 75.f;
    static constexpr float BOARD_PX = 8.f * CELL;
    static constexpr float SIDEBAR_X = 660.f;
    static constexpr float SIDEBAR_W = 370.f;

    // ── Menu buttons ──────────────────────────────────────────────────
    Button* m_btnNewGame = nullptr;
    Button* m_btnLoadGame = nullptr;
    Button* m_btnConsole = nullptr;
    Button* m_btnMenuExit = nullptr;

    // ── In-game buttons ───────────────────────────────────────────────
    Button* m_btnGameExit = nullptr;

    // ── Internal helpers ──────────────────────────────────────────────
    void processEvents(Board& board);
    void render(Board& board);

    void renderMenu();
    void renderNameInput();
    void renderGame(Board& board);

    void handleMenuClick(sf::Vector2f mp, Board& board);
    void handleNameInputText(char32_t unicode);
    void handleGameClick(sf::Vector2f mp, Board& board);

    bool loadSavedGame(Board& board);

    void openSaveFile();
    void closeSaveFile();
    void appendMove(int sr, int sc, int dr, int dc);

    void resetGame(Board& board);

    void         drawPiece(Piece* p, int row, int col);
    sf::Vector2f cellToScreen(int row, int col) const;
    void         boardCoordsFromMouse(sf::Vector2f mouse, int& row, int& col) const;
    void         centreText(sf::Text& t, float cx, float cy);
};

#endif // SFML_GUI_H
