#pragma once
#ifndef SFML_GUI_H
#define SFML_GUI_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <optional>
#include <fstream>
#include <vector>
#include "Board.h"

//  App states
enum class AppState
{
    Menu,
    NameInputWhite,
    NameInputBlack,
    Game,
    ConsoleMode
};

//  Simple reusable button
struct Button
{
    sf::RectangleShape shape;
    sf::Text           label;

    Button(sf::Font& font,
        const std::string& caption,
        sf::Vector2f        position,
        sf::Vector2f        size,
        unsigned int        charSize = 22);

    bool contains(sf::Vector2f point) const;
    void draw(sf::RenderWindow& window) const;
    void setHovered(bool hovered);
    void setColors(sf::Color fill, sf::Color outline);
};

//  Sound IDs
enum class SoundId
{
    ButtonClick,
    PieceMove,
    InvalidMove,
    Check,
    Checkmate,
    NewGame,
    LoadGame,
    COUNT
};

//  Main GUI class
class sfml_GUI
{
public:
    sfml_GUI();
    ~sfml_GUI();

    bool run(Board& board);

private:
    // -- Window & fonts -----
    sf::RenderWindow m_window;
    sf::Font         m_font;
    sf::Font         m_pieceFont;

    // -- App state ----------
    AppState    m_state;
    std::string m_whiteName;
    std::string m_blackName;
    std::string m_inputBuffer;

    // -- Game state -----------------------------------------------------------
    std::string   m_turn = "White";
    std::string   m_capturedByWhite = "";
    std::string   m_capturedByBlack = "";
    std::ofstream m_saveFile;

    // Move counter
    int  m_moveNumber = 1;

    // True only between name confirmation and the very first board interaction
    // for a New Game.  MUST be false when entering Game via Load.
    bool m_pendingNewGame = false;

    int  m_selRow = -1;
    int  m_selCol = -1;

    // -- Red-flash (invalid / wrong-turn click) -------
    bool      m_flashActive = false;
    int       m_flashRow = -1;
    int       m_flashCol = -1;
    sf::Clock m_flashClock;
    static constexpr float FLASH_DURATION = 0.50f;

    // -- Check-sound gate (once per check entry) ------
    bool m_checkSoundPlayed = false;

    // -- Checkmate / game-over ------------------------
    bool        m_gameOver = false;
    std::string m_winnerText = "";

    // -- "No saved game" toast ------------------------
    bool      m_showNoSaveToast = false;
    sf::Clock m_toastClock;
    static constexpr float TOAST_DURATION = 2.5f;

    // -- Layout constants -----------------------------
    static constexpr float WIN_W = 1050.f;
    static constexpr float WIN_H = 750.f;
    static constexpr float BOARD_X = 30.f;
    static constexpr float BOARD_Y = 75.f;
    static constexpr float CELL = 75.f;
    static constexpr float BOARD_PX = 8.f * CELL;
    static constexpr float SIDEBAR_X = 660.f;
    static constexpr float SIDEBAR_W = 370.f;

    // -- Menu buttons ----------------------------------
    Button* m_btnNewGame = nullptr;
    Button* m_btnLoadGame = nullptr;
    Button* m_btnConsole = nullptr;
    Button* m_btnMenuExit = nullptr;

    // -- In-game buttons --------------------------------
    Button* m_btnGameExit = nullptr;

    //  SOUND SYSTEM  (SFML 3 compatible)
    static constexpr int SND_COUNT = static_cast<int>(SoundId::COUNT);

    std::vector<sf::SoundBuffer>          m_soundBuffers;
    std::vector<std::optional<sf::Sound>> m_sounds;

    void initSounds();

    static sf::SoundBuffer makeTone(
        const std::vector<std::pair<float, int>>& segments,
        unsigned  sampleRate = 44100,
        float     amplitude = 0.40f);

    void playSound(SoundId id);

    // -- Internal helpers ---------------------
    void processEvents(Board& board);
    void render(Board& board);

    void renderMenu();
    void renderNameInput();
    void renderGame(Board& board);

    void handleMenuClick(sf::Vector2f mp, Board& board);
    void handleNameInputText(char32_t unicode);
    void handleGameClick(sf::Vector2f mp, Board& board);

    // Load a previously saved game completely from scratch.
    // Returns true on success, false if the file is missing / empty / invalid.
    bool loadSavedGame(Board& board);

    // Helper: wipe the entire 8x8 grid (delete pieces, set all to nullptr)
    // then call board.initialize() so we start from a clean slate.
    void fullBoardReset(Board& board);

    void openSaveFile();
    void closeSaveFile();
    void appendMove(int sr, int sc, int dr, int dc);

    // Called on first handleGameClick() after names are confirmed (New Game).
    void resetGame(Board& board);

    void         drawPiece(Piece* p, int row, int col);
    sf::Vector2f cellToScreen(int row, int col) const;
    void         boardCoordsFromMouse(sf::Vector2f mouse, int& row, int& col) const;
    void         centreText(sf::Text& t, float cx, float cy);
};

#endif // SFML_GUI_H
