#include "SFML_GUI.h"
#include <iostream>
#include <stdexcept>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//  Colour palette

static const sf::Color C_BG{ 18,  18,  32 };
static const sf::Color C_LIGHT_SQ{ 240, 217, 181 };
static const sf::Color C_DARK_SQ{ 181, 136,  99 };
static const sf::Color C_SELECTED{ 80, 200,  80, 210 };
static const sf::Color C_LAST_FROM{ 205, 210,  88, 180 };   // last-move source tint
static const sf::Color C_LAST_TO{ 170, 200,  50, 180 };   // last-move dest tint
static const sf::Color C_FLASH_RED{ 210,  30,  30, 240 };
static const sf::Color C_CHECK_SQ{ 220,  55,  55, 200 };
static const sf::Color C_GOLD{ 220, 185,  90 };
static const sf::Color C_PANEL_BG{ 28,  28,  50 };
static const sf::Color C_PANEL_BORD{ 70,  70, 105 };
static const sf::Color C_WHITE_TURN{ 255, 250, 210 };
static const sf::Color C_BLACK_TURN{ 120, 170, 255 };
static const sf::Color C_BTN_N{ 42,  42,  75, 235 };
static const sf::Color C_BTN_H{ 75,  75, 135, 235 };
static const sf::Color C_BTN_EXIT_N{ 130,  28,  28, 235 };
static const sf::Color C_BTN_EXIT_H{ 195,  50,  50, 235 };

// -----------------------------------------------------------------------------
//  Font loader - tries each path until one succeeds
// -----------------------------------------------------------------------------
static bool tryOpenFont(sf::Font& font,
    std::initializer_list<const char*> paths)
{
    for (const char* p : paths)
        if (font.openFromFile(p)) return true;
    return false;
}

// -----------------------------------------------------------------------------
//  Piece symbol -> UTF-32 glyph
// -----------------------------------------------------------------------------
static sf::String pieceGlyph(Piece* p)
{
    if (!p) return {};
    const std::string& s = p->getSymbol();
    if (s == "\xe2\x99\x94") return sf::String(U"\u2654"); // white king   ♔
    if (s == "\xe2\x99\x95") return sf::String(U"\u2655"); // white queen  ♕
    if (s == "\xe2\x99\x96") return sf::String(U"\u2656"); // white rook   ♖
    if (s == "\xe2\x99\x97") return sf::String(U"\u2657"); // white bishop ♗
    if (s == "\xe2\x99\x98") return sf::String(U"\u2658"); // white knight ♘
    if (s == "\xe2\x99\x99") return sf::String(U"\u2659"); // white pawn   ♙
    if (s == "\xe2\x99\x9a") return sf::String(U"\u265A"); // black king   ♚
    if (s == "\xe2\x99\x9b") return sf::String(U"\u265B"); // black queen  ♛
    if (s == "\xe2\x99\x9c") return sf::String(U"\u265C"); // black rook   ♜
    if (s == "\xe2\x99\x9d") return sf::String(U"\u265D"); // black bishop ♝
    if (s == "\xe2\x99\x9e") return sf::String(U"\u265E"); // black knight ♞
    if (s == "\xe2\x99\x9f") return sf::String(U"\u265F"); // black pawn   ♟
    return sf::String::fromUtf8(s.begin(), s.end());
}

// =============================================================================
//  Button
// =============================================================================

Button::Button(sf::Font& font,
    const std::string& caption,
    sf::Vector2f       pos,
    sf::Vector2f       size,
    unsigned int       cs)
    : label(font, caption, cs)
{
    shape.setSize(size);
    shape.setPosition(pos);
    shape.setFillColor(C_BTN_N);
    shape.setOutlineColor(C_GOLD);
    shape.setOutlineThickness(2.f);

    sf::FloatRect lb = label.getLocalBounds();
    label.setOrigin({ lb.position.x + lb.size.x / 2.f,
                      lb.position.y + lb.size.y / 2.f });
    label.setPosition({ pos.x + size.x / 2.f,
                        pos.y + size.y / 2.f });
    label.setFillColor(sf::Color::White);
}

bool Button::contains(sf::Vector2f pt) const
{
    return shape.getGlobalBounds().contains(pt);
}

void Button::draw(sf::RenderWindow& w) const { w.draw(shape); w.draw(label); }

void Button::setHovered(bool h)
{
    shape.setFillColor(h ? C_BTN_H : C_BTN_N);
}

void Button::setColors(sf::Color fill, sf::Color outline)
{
    shape.setFillColor(fill);
    shape.setOutlineColor(outline);
}

// =============================================================================
//  Sound system  (SFML 3 compatible)
// =============================================================================

sf::SoundBuffer sfml_GUI::makeTone(
    const std::vector<std::pair<float, int>>& segments,
    unsigned  sampleRate,
    float     amplitude)
{
    std::vector<std::int16_t> samples;

    for (auto& [freq, dur_ms] : segments)
    {
        int count = static_cast<int>(
            (static_cast<long long>(sampleRate) * dur_ms) / 1000);

        for (int i = 0; i < count; ++i)
        {
            float t = static_cast<float>(i) / static_cast<float>(sampleRate);
            float env = 1.f;
            float fadeSmp = 0.005f * static_cast<float>(sampleRate); // 5 ms fade
            if (i < static_cast<int>(fadeSmp))
                env = static_cast<float>(i) / fadeSmp;
            if (i > count - static_cast<int>(fadeSmp))
                env = static_cast<float>(count - i) / fadeSmp;
            if (env < 0.f) env = 0.f;

            float wave = (freq > 0.f)
                ? amplitude * env *
                std::sin(2.f * static_cast<float>(M_PI) * freq * t)
                : 0.f;

            samples.push_back(static_cast<std::int16_t>(wave * 32767.f));
        }
    }

    const std::vector<sf::SoundChannel> monoMap = { sf::SoundChannel::Mono };

    if (samples.empty())
    {
        std::int16_t silence = 0;
        return sf::SoundBuffer(&silence, 1, 1, sampleRate, monoMap);
    }

    return sf::SoundBuffer(
        samples.data(),
        static_cast<std::uint64_t>(samples.size()),
        1,           // channelCount (mono)
        sampleRate,
        monoMap);
}

void sfml_GUI::initSounds()
{
    m_soundBuffers.reserve(SND_COUNT);
    m_sounds.reserve(SND_COUNT);

    // Build buffer, push it, then construct Sound from the stored buffer.
    // The buffer MUST live in the vector before Sound is constructed because
    // sf::Sound holds a reference to its buffer.
    auto add = [&](const std::vector<std::pair<float, int>>& segs,
        float amp = 0.40f)
        {
            m_soundBuffers.push_back(makeTone(segs, 44100, amp));
            m_sounds.emplace_back(sf::Sound(m_soundBuffers.back()));
        };

    // Button click: two-note high tick
    add({ { 880.f, 30 }, { 1100.f, 20 } }, 0.30f);

    // Piece move: low wooden thump
    add({ { 220.f, 25 }, { 180.f, 35 } }, 0.45f);

    // Invalid move: double buzz (freq 0 = silence gap)
    add({ { 200.f, 80 }, { 0.f, 20 }, { 200.f, 80 } }, 0.50f);

    // Check: tense two-note alarm
    add({ { 440.f, 120 }, { 0.f, 30 }, { 370.f, 160 } }, 0.55f);

    // Checkmate: ascending fanfare
    add({ { 523.f, 120 }, { 659.f, 120 }, { 784.f, 120 }, { 1047.f, 350 } }, 0.55f);

    // New game: cheerful ascending chime
    add({ { 523.f, 90 }, { 659.f, 90 }, { 784.f, 90 }, { 880.f, 180 } }, 0.40f);

    // Load game: gentle descending chime
    add({ { 880.f, 90 }, { 784.f, 90 }, { 659.f, 90 }, { 523.f, 180 } }, 0.38f);
}

void sfml_GUI::playSound(SoundId id)
{
    int idx = static_cast<int>(id);
    if (idx < 0 || idx >= SND_COUNT)       return;
    if (!m_sounds[idx].has_value())        return;

    sf::Sound& snd = m_sounds[idx].value();
    snd.stop();   // restart from beginning if already playing
    snd.play();
}

// =============================================================================
//  Constructor / Destructor
// =============================================================================

sfml_GUI::sfml_GUI()
    : m_window(sf::VideoMode{ sf::Vector2u{
                    static_cast<unsigned>(WIN_W),
                    static_cast<unsigned>(WIN_H) } },
                    "Chess  \xe2\x80\x93  Game"),
                    m_state(AppState::Menu)
{
    m_window.setFramerateLimit(60);

    if (!tryOpenFont(m_font, {
            "arial.ttf",
            "C:/Windows/Fonts/arial.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            "/usr/share/fonts/dejavu/DejaVuSans.ttf" }))
            throw std::runtime_error(
                "Cannot load UI font. Place arial.ttf next to the .exe.");

    if (!tryOpenFont(m_pieceFont, {
            "DejaVuSans.ttf",
            "FreeSerif.ttf",
            "C:/Windows/Fonts/seguisym.ttf",
            "C:/Windows/Fonts/DejaVuSans.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            "/usr/share/fonts/dejavu/DejaVuSans.ttf",
            "/usr/share/fonts/truetype/freefont/FreeSerif.ttf" }))
    {
        std::cerr << "[sfml_GUI] WARNING: chess-symbol font not found.\n"
            "  Copy DejaVuSans.ttf next to your .exe\n";
        m_pieceFont = m_font;   // fallback to UI font (pieces may look wrong)
    }

    initSounds();

    // ── Menu buttons ─────────────────────────────────────────────────────────
    constexpr float MBX = 375.f;
    constexpr float MBW = 300.f;
    constexpr float MBH = 56.f;
    constexpr float MBG = 18.f;

    m_btnNewGame = new Button(m_font, "New Game",
        { MBX, 310.f }, { MBW, MBH });
    m_btnLoadGame = new Button(m_font, "Load Saved Game",
        { MBX, 310.f + 1 * (MBH + MBG) }, { MBW, MBH });
    m_btnConsole = new Button(m_font, "Play Console",
        { MBX, 310.f + 2 * (MBH + MBG) }, { MBW, MBH });
    m_btnMenuExit = new Button(m_font, "Exit",
        { MBX, 310.f + 3 * (MBH + MBG) }, { MBW, MBH });
    m_btnMenuExit->setColors(C_BTN_EXIT_N, { 255, 120, 120 });

    // ── In-game exit button ───────────────────────────────────────────────────
    m_btnGameExit = new Button(m_font, "Exit Game",
        { SIDEBAR_X + 10.f, 660.f }, { 160.f, 46.f }, 19);
    m_btnGameExit->setColors(C_BTN_EXIT_N, { 255, 110, 110 });
}

sfml_GUI::~sfml_GUI()
{
    for (auto& opt : m_sounds)
        if (opt.has_value()) opt->stop();

    delete m_btnNewGame;
    delete m_btnLoadGame;
    delete m_btnConsole;
    delete m_btnMenuExit;
    delete m_btnGameExit;
    closeSaveFile();
}

// =============================================================================
//  fullBoardReset()
//
//  Board::initialize() does NOT clear mid-game pieces from non-starting
//  squares, so calling it on a board that has pieces scattered around leaves
//  ghost pieces behind.  This helper explicitly deletes and nulls every cell
//  before calling initialize(), giving us a completely clean slate.
//
//  This must be called instead of bare board.initialize() any time we want
//  to start fresh (New Game AND Load Game).
// =============================================================================

void sfml_GUI::fullBoardReset(Board& board)
{
    // Delete every piece still on the board and null the pointers.
    // We access cells via the public accessCell() method.
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
        {
            Piece*& cell = board.accessCell(r, c);
            delete cell;
            cell = nullptr;
        }

    // Now it is safe to call initialize() - it places new pieces on a blank grid.
    board.initialize();
}

// =============================================================================
//  resetGame()   - called on the first board click after a New Game
// =============================================================================

void sfml_GUI::resetGame(Board& board)
{
    fullBoardReset(board);          // clean slate - no ghost pieces

    m_turn = "White";
    m_capturedByWhite = "";
    m_capturedByBlack = "";
    m_selRow = -1;
    m_selCol = -1;
    m_flashActive = false;
    m_gameOver = false;
    m_winnerText = "";
    m_moveNumber = 1;        // move 1 = White's first turn
    m_pendingNewGame = false;
    m_checkSoundPlayed = false;
    m_showNoSaveToast = false;

    // Truncate/create the save file and write the player names header.
    openSaveFile();
}

// =============================================================================
//  run()
// =============================================================================

bool sfml_GUI::run(Board& board)
{
    while (m_window.isOpen())
    {
        processEvents(board);
        if (m_state == AppState::ConsoleMode)
        {
            m_window.close();
            return false;
        }
        render(board);
    }
    return true;
}

// =============================================================================
//  processEvents()
// =============================================================================

void sfml_GUI::processEvents(Board& board)
{
    while (std::optional<sf::Event> ev = m_window.pollEvent())
    {
        if (ev->is<sf::Event::Closed>())
        {
            m_window.close();
            return;
        }

        // ── Hover ─────────────────────────────────────────────────────────────
        if (const auto* mm = ev->getIf<sf::Event::MouseMoved>())
        {
            sf::Vector2f mp{ float(mm->position.x), float(mm->position.y) };
            if (m_state == AppState::Menu)
            {
                m_btnNewGame->setHovered(m_btnNewGame->contains(mp));
                m_btnLoadGame->setHovered(m_btnLoadGame->contains(mp));
                m_btnConsole->setHovered(m_btnConsole->contains(mp));
                m_btnMenuExit->shape.setFillColor(
                    m_btnMenuExit->contains(mp) ? C_BTN_EXIT_H : C_BTN_EXIT_N);
            }
            if (m_state == AppState::Game)
            {
                m_btnGameExit->shape.setFillColor(
                    m_btnGameExit->contains(mp) ? C_BTN_EXIT_H : C_BTN_EXIT_N);
            }
        }

        // ── Click ─────────────────────────────────────────────────────────────
        if (const auto* mb = ev->getIf<sf::Event::MouseButtonPressed>())
        {
            if (mb->button == sf::Mouse::Button::Left)
            {
                sf::Vector2f mp{ float(mb->position.x), float(mb->position.y) };
                if (m_state == AppState::Menu)
                    handleMenuClick(mp, board);
                else if (m_state == AppState::Game)
                    handleGameClick(mp, board);
            }
        }

        // ── Text input (name screens) ─────────────────────────────────────────
        if (const auto* te = ev->getIf<sf::Event::TextEntered>())
        {
            if (m_state == AppState::NameInputWhite ||
                m_state == AppState::NameInputBlack)
                handleNameInputText(te->unicode);
        }

        // ── Keyboard shortcuts ────────────────────────────────────────────────
        if (const auto* kp = ev->getIf<sf::Event::KeyPressed>())
        {
            // ESC in-game -> back to menu (save file already contains moves)
            if (kp->code == sf::Keyboard::Key::Escape &&
                m_state == AppState::Game)
            {
                playSound(SoundId::ButtonClick);
                closeSaveFile();
                m_state = AppState::Menu;
                m_selRow = m_selCol = -1;
                m_flashActive = false;
                m_gameOver = false;
                m_checkSoundPlayed = false;
            }
            // Backspace in name input
            if (kp->code == sf::Keyboard::Key::Backspace &&
                !m_inputBuffer.empty() &&
                (m_state == AppState::NameInputWhite ||
                    m_state == AppState::NameInputBlack))
                m_inputBuffer.pop_back();
        }
    }
}

// =============================================================================
//  render()
// =============================================================================

void sfml_GUI::render(Board& board)
{
    m_window.clear(C_BG);
    switch (m_state)
    {
    case AppState::Menu:
    case AppState::NameInputWhite:
    case AppState::NameInputBlack:
        renderMenu();
        if (m_state == AppState::NameInputWhite ||
            m_state == AppState::NameInputBlack)
            renderNameInput();
        break;
    case AppState::Game:
        renderGame(board);
        break;
    default:
        break;
    }
    m_window.display();
}

// =============================================================================
//  renderMenu()
// =============================================================================

void sfml_GUI::renderMenu()
{
    // Title
    sf::Text title(m_font, "CHESS", 82);
    title.setStyle(sf::Text::Bold);
    title.setFillColor(C_GOLD);
    centreText(title, WIN_W / 2.f, 160.f);
    m_window.draw(title);

    sf::Text sub(m_font, "C++ Edition  |  SFML 3", 20);
    sub.setFillColor({ 150, 150, 180 });
    centreText(sub, WIN_W / 2.f, 230.f);
    m_window.draw(sub);

    sf::RectangleShape rule({ 340.f, 2.f });
    rule.setPosition({ (WIN_W - 340.f) / 2.f, 265.f });
    rule.setFillColor({ 160, 140, 80, 160 });
    m_window.draw(rule);

    m_btnNewGame->draw(m_window);
    m_btnLoadGame->draw(m_window);
    m_btnConsole->draw(m_window);
    m_btnMenuExit->draw(m_window);

    // "No saved game" toast (auto-hides after TOAST_DURATION seconds)
    if (m_showNoSaveToast)
    {
        if (m_toastClock.getElapsedTime().asSeconds() < TOAST_DURATION)
        {
            sf::RectangleShape bg({ 380.f, 50.f });
            bg.setPosition({ (WIN_W - 380.f) / 2.f, 588.f });
            bg.setFillColor({ 160, 30, 30, 220 });
            bg.setOutlineColor({ 255, 90, 90 });
            bg.setOutlineThickness(1.5f);
            m_window.draw(bg);

            sf::Text t(m_font, "No saved game found! Start a New Game first.", 17);
            t.setFillColor(sf::Color::White);
            centreText(t, WIN_W / 2.f, 613.f);
            m_window.draw(t);
        }
        else
        {
            m_showNoSaveToast = false;
        }
    }

    // Footer
    sf::Text footer(m_font,
        "Muhammad Taha  |  Hiba Eman  |  FAST-NUCES Faisalabad", 14);
    footer.setFillColor({ 80, 80, 105 });
    centreText(footer, WIN_W / 2.f, WIN_H - 22.f);
    m_window.draw(footer);
}

// =============================================================================
//  renderNameInput()  - overlay on top of menu
// =============================================================================

void sfml_GUI::renderNameInput()
{
    // Dim background
    sf::RectangleShape overlay({ WIN_W, WIN_H });
    overlay.setFillColor({ 0, 0, 0, 170 });
    m_window.draw(overlay);

    bool isWhite = (m_state == AppState::NameInputWhite);

    sf::Text prompt(m_font,
        isWhite ? "Enter White Player's Name:"
        : "Enter Black Player's Name:", 30);
    prompt.setFillColor(isWhite ? sf::Color::White : sf::Color{ 160, 160, 190 });
    centreText(prompt, WIN_W / 2.f, 260.f);
    m_window.draw(prompt);

    // Input box
    sf::RectangleShape box({ 440.f, 58.f });
    box.setPosition({ (WIN_W - 440.f) / 2.f, 310.f });
    box.setFillColor({ 28, 28, 54 });
    box.setOutlineColor(C_GOLD);
    box.setOutlineThickness(2.f);
    m_window.draw(box);

    sf::Text typed(m_font, m_inputBuffer + "|", 28);
    typed.setPosition({ (WIN_W - 440.f) / 2.f + 10.f, 318.f });
    typed.setFillColor({ 230, 210, 140 });
    m_window.draw(typed);

    sf::Text hint(m_font, "Press Enter to confirm  |  Backspace to delete", 17);
    hint.setFillColor({ 120, 120, 155 });
    centreText(hint, WIN_W / 2.f, 400.f);
    m_window.draw(hint);

    // Show confirmed white name when asking for black name
    if (!isWhite && !m_whiteName.empty())
    {
        sf::Text prev(m_font, "White player:  " + m_whiteName, 19);
        prev.setFillColor({ 150, 200, 150 });
        centreText(prev, WIN_W / 2.f, 450.f);
        m_window.draw(prev);
    }
}

// =============================================================================
//  renderGame()
// =============================================================================

void sfml_GUI::renderGame(Board& board)
{
    // ── New Game: reset board on the very first frame of the game screen ──────
    // Doing this here (every-frame render path) rather than on first click
    // guarantees the board is wiped IMMEDIATELY when the screen appears,
    // regardless of whether the user clicks quickly, slowly, or on the Exit
    // button as their first action.  loadSavedGame() always sets this to
    // false, so it never fires after a Load.
    if (m_pendingNewGame)
        resetGame(board);

    // ── Rank / file labels ────────────────────────────────────────────────────
    for (int i = 0; i < 8; i++)
    {
        sf::Text rankLbl(m_font,
            std::string(1, static_cast<char>('8' - i)), 15);
        rankLbl.setFillColor({ 170, 160, 120 });
        rankLbl.setPosition({
            BOARD_X - 18.f,
            BOARD_Y + i * CELL + CELL / 2.f - 10.f });
        m_window.draw(rankLbl);

        sf::Text fileLbl(m_font,
            std::string(1, static_cast<char>('a' + i)), 15);
        fileLbl.setFillColor({ 170, 160, 120 });
        fileLbl.setPosition({
            BOARD_X + i * CELL + CELL / 2.f - 6.f,
            BOARD_Y + BOARD_PX + 6.f });
        m_window.draw(fileLbl);
    }

    // ── Flash timer ───────────────────────────────────────────────────────────
    if (m_flashActive &&
        m_flashClock.getElapsedTime().asSeconds() >= FLASH_DURATION)
        m_flashActive = false;

    // ── Find king square for check highlight ──────────────────────────────────
    int checkKingRow = -1, checkKingCol = -1;
    if (!m_gameOver && board.isCheck(m_turn))
    {
        for (int r = 0; r < 8 && checkKingRow == -1; ++r)
            for (int c = 0; c < 8 && checkKingRow == -1; ++c)
            {
                Piece* p = board.getPiece(r, c);
                if (p && p->getColor() == m_turn &&
                    dynamic_cast<King*>(p) != nullptr)
                {
                    checkKingRow = r;
                    checkKingCol = c;
                }
            }
    }

    // ── Board squares + pieces ────────────────────────────────────────────────
    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            sf::RectangleShape cell({ CELL, CELL });
            cell.setPosition(cellToScreen(row, col));

            bool light = (row + col) % 2 == 0;

            if (m_flashActive && row == m_flashRow && col == m_flashCol)
                cell.setFillColor(C_FLASH_RED);
            else if (row == checkKingRow && col == checkKingCol)
                cell.setFillColor(C_CHECK_SQ);
            else if (row == m_selRow && col == m_selCol)
                cell.setFillColor(C_SELECTED);
            else
                cell.setFillColor(light ? C_LIGHT_SQ : C_DARK_SQ);

            m_window.draw(cell);

            Piece* p = board.getPiece(row, col);
            if (p) drawPiece(p, row, col);
        }
    }

    // ── Board border ──────────────────────────────────────────────────────────
    sf::RectangleShape border({ BOARD_PX, BOARD_PX });
    border.setPosition({ BOARD_X, BOARD_Y });
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(C_GOLD);
    border.setOutlineThickness(3.f);
    m_window.draw(border);

    // ── Separator between board and sidebar ───────────────────────────────────
    sf::RectangleShape gapLine({ 2.f, WIN_H - 20.f });
    gapLine.setPosition({ SIDEBAR_X - 15.f, 10.f });
    gapLine.setFillColor({ 55, 55, 80 });
    m_window.draw(gapLine);

    // =========================================================================
    //  SIDEBAR
    // =========================================================================
    sf::RectangleShape panel({ SIDEBAR_W, WIN_H - 20.f });
    panel.setPosition({ SIDEBAR_X, 10.f });
    panel.setFillColor(C_PANEL_BG);
    panel.setOutlineColor(C_PANEL_BORD);
    panel.setOutlineThickness(1.5f);
    m_window.draw(panel);

    const float sx = SIDEBAR_X + 12.f;
    const float sw = SIDEBAR_W - 24.f;

    // ── Turn banner ───────────────────────────────────────────────────────────
    {
        bool wt = (m_turn == "White");
        sf::RectangleShape badge({ sw, 54.f });
        badge.setPosition({ sx, 20.f });
        badge.setFillColor(wt ? sf::Color{ 210, 205, 165 }
        : sf::Color{ 32,  32,  58 });
        badge.setOutlineColor(wt ? sf::Color{ 255, 250, 200 } : C_BLACK_TURN);
        badge.setOutlineThickness(2.f);
        m_window.draw(badge);

        sf::Text t(m_font,
            wt ? (m_whiteName + "'s Turn  (White)")
            : (m_blackName + "'s Turn  (Black)"), 18);
        t.setStyle(sf::Text::Bold);
        t.setFillColor(wt ? sf::Color{ 25, 25, 0 } : C_WHITE_TURN);
        centreText(t, sx + sw / 2.f, 47.f);
        m_window.draw(t);
    }

    // Helper: horizontal divider
    auto drawDiv = [&](float y)
        {
            sf::RectangleShape d({ sw, 1.f });
            d.setPosition({ sx, y });
            d.setFillColor({ 70, 70, 100 });
            m_window.draw(d);
        };

    drawDiv(86.f);

    // ── Player names ──────────────────────────────────────────────────────────
    {
        sf::Text wn(m_font, "White:  " + m_whiteName, 17);
        wn.setPosition({ sx, 92.f });
        wn.setFillColor({ 230, 220, 175 });
        m_window.draw(wn);

        sf::Text bn(m_font, "Black:  " + m_blackName, 17);
        bn.setPosition({ sx, 116.f });
        bn.setFillColor({ 160, 180, 235 });
        m_window.draw(bn);
    }

    drawDiv(146.f);

    // ── Captured by White ─────────────────────────────────────────────────────
    {
        sf::Text hdr(m_font, "Captured by White:", 15);
        hdr.setPosition({ sx, 153.f });
        hdr.setFillColor({ 200, 190, 145 });
        m_window.draw(hdr);

        sf::Text cap(m_pieceFont,
            sf::String::fromUtf8(
                m_capturedByWhite.begin(), m_capturedByWhite.end()), 26);
        cap.setPosition({ sx, 174.f });
        cap.setFillColor({ 245, 230, 175 });
        m_window.draw(cap);
    }

    drawDiv(255.f);

    // ── Captured by Black ─────────────────────────────────────────────────────
    {
        sf::Text hdr(m_font, "Captured by Black:", 15);
        hdr.setPosition({ sx, 262.f });
        hdr.setFillColor({ 170, 185, 220 });
        m_window.draw(hdr);

        sf::Text cap(m_pieceFont,
            sf::String::fromUtf8(
                m_capturedByBlack.begin(), m_capturedByBlack.end()), 26);
        cap.setPosition({ sx, 283.f });
        cap.setFillColor({ 180, 200, 245 });
        m_window.draw(cap);
    }

    drawDiv(365.f);

    // ── Move counter ──────────────────────────────────────────────────────────
    //  Rule: starts at 1, increments after Black's move (when turn = White).
    //  So move N covers White's action and Black's action.
    {
        sf::Text mc(m_font, "Move:  " + std::to_string(m_moveNumber), 17);
        mc.setPosition({ sx, 373.f });
        mc.setFillColor({ 160, 160, 190 });
        m_window.draw(mc);
    }

    drawDiv(404.f);

    // ── Check notice (one-shot sound) ─────────────────────────────────────────
    bool inCheck = !m_gameOver && board.isCheck(m_turn);
    if (inCheck)
    {
        if (!m_checkSoundPlayed)
        {
            playSound(SoundId::Check);
            m_checkSoundPlayed = true;
        }
        sf::Text chk(m_font, "  CHECK!", 22);
        chk.setFillColor({ 255, 80, 80 });
        chk.setStyle(sf::Text::Bold);
        centreText(chk, sx + sw / 2.f, 430.f);
        m_window.draw(chk);
    }
    else
    {
        m_checkSoundPlayed = false;
    }

    drawDiv(455.f);

    // ── Instructions hint ─────────────────────────────────────────────────────
    {
        sf::Text hint(m_font, "Click a piece, then click", 14);
        hint.setPosition({ sx, 463.f });
        hint.setFillColor({ 100, 100, 130 });
        m_window.draw(hint);

        sf::Text hint2(m_font, "its destination square.", 14);
        hint2.setPosition({ sx, 482.f });
        hint2.setFillColor({ 100, 100, 130 });
        m_window.draw(hint2);

        sf::Text hint3(m_font, "ESC = return to menu", 14);
        hint3.setPosition({ sx, 501.f });
        hint3.setFillColor({ 80, 80, 110 });
        m_window.draw(hint3);
    }

    // ── Exit button ───────────────────────────────────────────────────────────
    m_btnGameExit->draw(m_window);

    // ── Checkmate overlay (drawn on top of the board) ─────────────────────────
    if (m_gameOver)
    {
        sf::RectangleShape ovl({ BOARD_PX + 6.f, BOARD_PX + 6.f });
        ovl.setPosition({ BOARD_X - 3.f, BOARD_Y - 3.f });
        ovl.setFillColor({ 0, 0, 0, 175 });
        m_window.draw(ovl);

        sf::Text msg(m_font, m_winnerText, 28);
        msg.setStyle(sf::Text::Bold);
        msg.setFillColor(sf::Color::Yellow);
        centreText(msg,
            BOARD_X + BOARD_PX / 2.f,
            BOARD_Y + BOARD_PX / 2.f - 30.f);
        m_window.draw(msg);

        sf::Text sub2(m_font, "Press ESC or click Exit to return to menu", 17);
        sub2.setFillColor({ 220, 220, 220 });
        centreText(sub2,
            BOARD_X + BOARD_PX / 2.f,
            BOARD_Y + BOARD_PX / 2.f + 30.f);
        m_window.draw(sub2);
    }
}

// =============================================================================
//  drawPiece()
// =============================================================================

void sfml_GUI::drawPiece(Piece* piece, int row, int col)
{
    sf::String sym = pieceGlyph(piece);
    if (sym.isEmpty()) return;

    sf::Text glyph(m_pieceFont, sym,
        static_cast<unsigned>(CELL * 0.72f));

    bool white = (piece->getColor() == "White");
    glyph.setFillColor(white ? sf::Color{ 255, 252, 215 }
    : sf::Color{ 12,   8,   4 });
    glyph.setOutlineColor(white ? sf::Color{ 75,  45,   0 }
    : sf::Color{ 215, 205, 185 });
    glyph.setOutlineThickness(1.6f);

    sf::FloatRect gb = glyph.getLocalBounds();
    glyph.setOrigin({ gb.position.x + gb.size.x / 2.f,
                      gb.position.y + gb.size.y / 2.f });
    glyph.setPosition({
        BOARD_X + col * CELL + CELL / 2.f,
        BOARD_Y + row * CELL + CELL / 2.f });

    m_window.draw(glyph);
}

// =============================================================================
//  handleMenuClick()
// =============================================================================

void sfml_GUI::handleMenuClick(sf::Vector2f mp, Board& board)
{
    if (m_btnNewGame->contains(mp))
    {
        playSound(SoundId::ButtonClick);
        m_inputBuffer = "";
        m_pendingNewGame = true;   // resetGame() fires on first board click
        m_state = AppState::NameInputWhite;
    }
    else if (m_btnLoadGame->contains(mp))
    {
        playSound(SoundId::ButtonClick);
        if (loadSavedGame(board))
        {
            playSound(SoundId::LoadGame);
            m_state = AppState::Game;
        }
        else
        {
            playSound(SoundId::InvalidMove);
            m_showNoSaveToast = true;
            m_toastClock.restart();
        }
    }
    else if (m_btnConsole->contains(mp))
    {
        playSound(SoundId::ButtonClick);
        m_state = AppState::ConsoleMode;
    }
    else if (m_btnMenuExit->contains(mp))
    {
        playSound(SoundId::ButtonClick);
        m_window.close();
    }
}

// =============================================================================
//  handleNameInputText()
// =============================================================================

void sfml_GUI::handleNameInputText(char32_t unicode)
{
    if (unicode == 13)  // Enter
    {
        if (m_inputBuffer.empty()) return;

        playSound(SoundId::ButtonClick);

        if (m_state == AppState::NameInputWhite)
        {
            m_whiteName = m_inputBuffer;
            m_inputBuffer = "";
            m_state = AppState::NameInputBlack;
        }
        else // NameInputBlack
        {
            m_blackName = m_inputBuffer;
            m_inputBuffer = "";
            m_pendingNewGame = true;   // resetGame() fires in renderGame() before first draw
            m_state = AppState::Game;
            playSound(SoundId::NewGame);
        }
        return;
    }

    if (unicode == 8 && !m_inputBuffer.empty())  // Backspace
    {
        m_inputBuffer.pop_back();
        return;
    }

    if (unicode >= 32 && unicode < 127 && m_inputBuffer.size() < 20)
        m_inputBuffer += static_cast<char>(unicode);
}

// =============================================================================
//  handleGameClick()
// =============================================================================

void sfml_GUI::handleGameClick(sf::Vector2f mp, Board& board)
{
    // Exit button
    if (m_btnGameExit->contains(mp))
    {
        playSound(SoundId::ButtonClick);
        closeSaveFile();            // flush and close; file still exists on disk
        m_state = AppState::Menu;
        m_selRow = m_selCol = -1;
        m_flashActive = false;
        m_gameOver = false;
        m_checkSoundPlayed = false;
        return;
    }

    if (m_gameOver) return;         // no moves allowed after checkmate

    // Map click to board square
    int row, col;
    boardCoordsFromMouse(mp, row, col);
    if (row < 0) { m_selRow = m_selCol = -1; return; }

    Piece* clicked = board.getPiece(row, col);

    // ── Nothing selected yet ──────────────────────────────────────────────────
    if (m_selRow == -1)
    {
        if (!clicked) return;
        if (clicked->getColor() == m_turn)
        {
            m_selRow = row;
            m_selCol = col;
            m_flashActive = false;
        }
        else
        {
            // Wrong colour piece clicked
            playSound(SoundId::InvalidMove);
            m_flashRow = row;
            m_flashCol = col;
            m_flashActive = true;
            m_flashClock.restart();
        }
        return;
    }

    // ── Same square clicked -> deselect ───────────────────────────────────────
    if (row == m_selRow && col == m_selCol)
    {
        m_selRow = m_selCol = -1;
        return;
    }

    // ── Another friendly piece -> re-select ───────────────────────────────────
    if (clicked && clicked->getColor() == m_turn)
    {
        m_selRow = row;
        m_selCol = col;
        m_flashActive = false;
        return;
    }

    // ── Attempt the move ──────────────────────────────────────────────────────
    bool ok = board.movePiece(
        m_selRow, m_selCol, row, col,
        m_turn,
        m_capturedByWhite,
        m_capturedByBlack);

    if (ok)
    {
        appendMove(m_selRow, m_selCol, row, col);

        // Switch turn
        m_turn = (m_turn == "White") ? "Black" : "White";

        // ----------------------------------------------------------------
        //  Move counter rule  (matches console behaviour):
        //    moves starts at 1.
        //    Increment AFTER Black completes a move, i.e. when it is now
        //    White's turn again.  This way move 1 = White+Black pair,
        //    move 2 = next White+Black pair, etc.
        // ----------------------------------------------------------------
        if (m_turn == "White")
            m_moveNumber++;

        m_selRow = m_selCol = -1;
        m_flashActive = false;

        playSound(SoundId::PieceMove);

        // ── Checkmate check ───────────────────────────────────────────────
        if (board.isCheckmate(m_turn))
        {
            // The player whose turn it IS has been mated - the OTHER player wins
            std::string winner = (m_turn == "White") ? m_blackName : m_whiteName;
            std::string loser = (m_turn == "White") ? "White" : "Black";
            m_winnerText = winner + " wins!\n" + loser + " is in Checkmate!";
            m_gameOver = true;

            // Close (but do NOT truncate) the save file so the game history
            // is preserved on disk and can be loaded in a later session.
            closeSaveFile();

            playSound(SoundId::Checkmate);
        }
    }
    else
    {
        // Invalid move - flash the selected source square
        playSound(SoundId::InvalidMove);
        m_flashRow = m_selRow;
        m_flashCol = m_selCol;
        m_flashActive = true;
        m_flashClock.restart();
    }
}

// =============================================================================
//  loadSavedGame()
//
//  Completely independent of any prior New Game session.
//
//  Algorithm:
//   1. Open and validate savedGame.txt (needs names + at least one move).
//   2. fullBoardReset() -> clears ALL 64 squares then places starting pieces.
//   3. Reset every GUI state variable.
//   4. Replay every saved move in order, tracking turn and move counter.
//   5. Reopen the file in APPEND mode so future moves extend the history.
//
//  Why fullBoardReset() and not bare board.initialize():
//   Board::initialize() only writes to the standard starting squares; it
//   never nulls other squares.  If any pieces moved during a prior session
//   remain in the grid, they stay there as ghosts.  fullBoardReset()
//   explicitly deletes and nulls all 64 cells first.
//
//  Why m_pendingNewGame = false is critical:
//   If a New Game was started but then the user went back to menu and then
//   chose Load Game, m_pendingNewGame could still be true.  If it were left
//   true, the very first click on the board would call resetGame() and
//   destroy the freshly loaded position.
// =============================================================================

bool sfml_GUI::loadSavedGame(Board& board)
{
    std::ifstream fin("savedGame.txt");
    if (!fin.is_open()) return false;

    // Read player names from the first two lines
    std::string wName, bName;
    if (!std::getline(fin, wName)) return false;
    if (!std::getline(fin, bName)) return false;

    // Consider it "no save" if both names are empty (freshly truncated file)
    if (wName.empty() && bName.empty()) return false;

    // Read all moves to verify there is at least something to load.
    // We store them so we can replay below.
    std::vector<std::array<int, 4>> moves;
    {
        int sr, sc, dr, dc;
        while (fin >> sr >> sc >> dr >> dc)
            moves.push_back({ sr, sc, dr, dc });
    }
    fin.close();

    // A valid save must have player names; moves are optional (you can load
    // right after entering names even if no moves were played yet).
    // But if only names with zero moves exist it means a game was set up but
    // never started - still valid to resume.
    // We only reject a completely empty file (caught by the name check above).

    // ── Step 1: Clean slate ──────────────────────────────────────────────────
    // Delete every current piece and re-place the standard starting position.
    fullBoardReset(board);

    // ── Step 2: Reset all GUI state ──────────────────────────────────────────
    m_whiteName = wName;
    m_blackName = bName;
    m_turn = "White";
    m_capturedByWhite = "";
    m_capturedByBlack = "";
    m_selRow = -1;
    m_selCol = -1;
    m_flashActive = false;
    m_gameOver = false;
    m_winnerText = "";
    m_moveNumber = 1;       // start counting from move 1

    // CRITICAL: prevent resetGame() from firing on first board click.
    m_pendingNewGame = false;

    m_checkSoundPlayed = false;
    m_showNoSaveToast = false;

    // ── Step 3: Replay moves ─────────────────────────────────────────────────
    for (auto& mv : moves)
    {
        int sr = mv[0], sc = mv[1], dr = mv[2], dc = mv[3];

        board.movePiece(sr, sc, dr, dc,
            m_turn,
            m_capturedByWhite,
            m_capturedByBlack);

        // Switch turn
        m_turn = (m_turn == "White") ? "Black" : "White";

        // Increment counter exactly as during live play:
        // after Black completes a move (turn flips back to White).
        if (m_turn == "White")
            m_moveNumber++;
    }

    // ── Step 4: Reopen in append mode ────────────────────────────────────────
    // New moves are written after the replayed history; the file is NOT truncated.
    if (m_saveFile.is_open()) m_saveFile.close();
    m_saveFile.open("savedGame.txt", std::ios::app);

    return true;
}

// =============================================================================
//  Save-file helpers
// =============================================================================

void sfml_GUI::openSaveFile()
{
    // Truncates / creates the file and writes the name header.
    // Called ONLY when starting a brand-new game.
    if (m_saveFile.is_open()) m_saveFile.close();
    m_saveFile.open("savedGame.txt");          // truncate mode
    if (m_saveFile.is_open())
    {
        m_saveFile << m_whiteName << "\n" << m_blackName << "\n";
        m_saveFile.flush();
    }
}

void sfml_GUI::closeSaveFile()
{
    // Simply closes the handle.  The file and its contents are preserved.
    if (m_saveFile.is_open()) m_saveFile.close();
}

void sfml_GUI::appendMove(int sr, int sc, int dr, int dc)
{
    if (m_saveFile.is_open())
    {
        m_saveFile << sr << " " << sc << " " << dr << " " << dc << "\n";
        m_saveFile.flush();
    }
}

// =============================================================================
//  Utility
// =============================================================================

sf::Vector2f sfml_GUI::cellToScreen(int row, int col) const
{
    return { BOARD_X + col * CELL, BOARD_Y + row * CELL };
}

void sfml_GUI::boardCoordsFromMouse(sf::Vector2f m, int& row, int& col) const
{
    col = static_cast<int>((m.x - BOARD_X) / CELL);
    row = static_cast<int>((m.y - BOARD_Y) / CELL);
    if (col < 0 || col >= 8 || row < 0 || row >= 8)
        row = col = -1;
}

void sfml_GUI::centreText(sf::Text& t, float cx, float cy)
{
    sf::FloatRect b = t.getLocalBounds();
    t.setOrigin({ b.position.x + b.size.x / 2.f,
                  b.position.y + b.size.y / 2.f });
    t.setPosition({ cx, cy });
}
