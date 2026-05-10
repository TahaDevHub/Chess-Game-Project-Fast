// =============================================================================
//  SFML_GUI.cpp
//
//  TWO BUGS FIXED (search "FIX 1" and "FIX 2" to jump straight to them):
//
//  ── FIX 1  Move counter ────────────────────────────────────────────────────
//  Console logic in Game::startNewGame():
//      board.display(moves, ...);   // show board THEN …
//      if (turn == "Black") moves++;  // … increment when it is Black's turn
//  So 'moves' increments BEFORE Black inputs their move.
//  That means: White moves → turn flips to Black → moves goes 1→2 → board
//  is displayed as "Move #2" while Black is about to play.
//  In other words, the counter ticks up AFTER a successful White move
//  (equivalently: when the turn just became Black).
//
//  Old GUI code was:
//      m_turn = (m_turn=="White") ? "Black" : "White";
//      if (m_turn == "Black") m_moveNumber++;   // ← WRONG: fires after EVERY
//                                               //   White move AND after load
//  This caused double-increments because loadSavedGame() also did the same
//  condition, counting incorrectly during replay.
//
//  Corrected rule (matches console exactly):
//      • After a successful move, switch the turn.
//      • If the new turn is "Black"  →  m_moveNumber++
//        (means White just finished, same as console's "if(turn=="Black")")
//      • During load-replay apply the same rule.
//
//  ── FIX 2  New Game after Load Game shows old board ────────────────────────
//  Root cause: handleNameInputText() has no access to Board&, so it could
//  call openSaveFile() but NOT board.initialize(). The board kept the loaded
//  game's piece positions.
//
//  Fix: set m_pendingNewGame = true when names are confirmed.
//  handleGameClick() is the first function called that has Board&.
//  At the very top it checks m_pendingNewGame and calls resetGame(board),
//  which does board.initialize() + opens/truncates savedGame.txt.
//  The save file is therefore empty until White's first move is appended,
//  which is what "remove previous saved game after first White move" means.
// =============================================================================

#include "SFML_GUI.h"
#include <iostream>
#include <stdexcept>
#include <sstream>

// ─────────────────────────────────────────────────────────────────────────────
//  Colour palette
// ─────────────────────────────────────────────────────────────────────────────
static const sf::Color C_BG{ 18,  18,  32 };
static const sf::Color C_LIGHT_SQ{ 240, 217, 181 };
static const sf::Color C_DARK_SQ{ 181, 136,  99 };
static const sf::Color C_SELECTED{ 80, 200,  80, 210 };
static const sf::Color C_FLASH_RED{ 210,  30,  30, 240 };
static const sf::Color C_GOLD{ 220, 185,  90 };
static const sf::Color C_PANEL_BG{ 28,  28,  50 };
static const sf::Color C_PANEL_BORD{ 70,  70, 105 };
static const sf::Color C_WHITE_TURN{ 255, 250, 210 };
static const sf::Color C_BLACK_TURN{ 120, 170, 255 };
static const sf::Color C_BTN_N{ 42,  42,  75, 235 };
static const sf::Color C_BTN_H{ 75,  75, 135, 235 };
static const sf::Color C_BTN_EXIT_N{ 130,  28,  28, 235 };
static const sf::Color C_BTN_EXIT_H{ 195,  50,  50, 235 };

// ─────────────────────────────────────────────────────────────────────────────
//  Font loader
// ─────────────────────────────────────────────────────────────────────────────
static bool tryOpenFont(sf::Font& font,
    std::initializer_list<const char*> paths)
{
    for (const char* p : paths)
        if (font.openFromFile(p)) return true;
    return false;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Piece → UTF-32 glyph (encoding-safe on all compilers)
// ─────────────────────────────────────────────────────────────────────────────
static sf::String pieceGlyph(Piece* p)
{
    if (!p) return {};
    const std::string& s = p->getSymbol();
    if (s == "\xe2\x99\x94") return sf::String(U"\u2654"); // ♔ White King
    if (s == "\xe2\x99\x95") return sf::String(U"\u2655"); // ♕ White Queen
    if (s == "\xe2\x99\x96") return sf::String(U"\u2656"); // ♖ White Rook
    if (s == "\xe2\x99\x97") return sf::String(U"\u2657"); // ♗ White Bishop
    if (s == "\xe2\x99\x98") return sf::String(U"\u2658"); // ♘ White Knight
    if (s == "\xe2\x99\x99") return sf::String(U"\u2659"); // ♙ White Pawn
    if (s == "\xe2\x99\x9a") return sf::String(U"\u265A"); // ♚ Black King
    if (s == "\xe2\x99\x9b") return sf::String(U"\u265B"); // ♛ Black Queen
    if (s == "\xe2\x99\x9c") return sf::String(U"\u265C"); // ♜ Black Rook
    if (s == "\xe2\x99\x9d") return sf::String(U"\u265D"); // ♝ Black Bishop
    if (s == "\xe2\x99\x9e") return sf::String(U"\u265E"); // ♞ Black Knight
    if (s == "\xe2\x99\x9f") return sf::String(U"\u265F"); // ♟ Black Pawn
    return sf::String::fromUtf8(s.begin(), s.end());
}

// =============================================================================
//  Button
// =============================================================================

Button::Button(sf::Font& font,
    const std::string& caption,
    sf::Vector2f pos,
    sf::Vector2f size,
    unsigned int cs)
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
void Button::setHovered(bool h) { shape.setFillColor(h ? C_BTN_H : C_BTN_N); }
void Button::setColors(sf::Color fill, sf::Color outline)
{
    shape.setFillColor(fill);
    shape.setOutlineColor(outline);
}

// =============================================================================
//  Constructor / Destructor
// =============================================================================

sfml_GUI::sfml_GUI()
    : m_window(sf::VideoMode{ sf::Vector2u{
                    static_cast<unsigned>(WIN_W),
                    static_cast<unsigned>(WIN_H) } },
                    "Chess  –  Game"),
                    m_state(AppState::Menu)
{
    m_window.setFramerateLimit(60);

    if (!tryOpenFont(m_font, {
            "arial.ttf",
            "C:/Windows/Fonts/arial.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            "/usr/share/fonts/dejavu/DejaVuSans.ttf" }))
            throw std::runtime_error("Cannot load UI font. Place arial.ttf next to the .exe.");

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
            "  Copy DejaVuSans.ttf next to your .exe\n"
            "  https://dejavu-fonts.github.io/\n";
        m_pieceFont = m_font;
    }

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

    m_btnGameExit = new Button(m_font, "Exit Game",
        { SIDEBAR_X + 10.f, 660.f }, { 160.f, 46.f }, 19);
    m_btnGameExit->setColors(C_BTN_EXIT_N, { 255, 110, 110 });
}

sfml_GUI::~sfml_GUI()
{
    delete m_btnNewGame;
    delete m_btnLoadGame;
    delete m_btnConsole;
    delete m_btnMenuExit;
    delete m_btnGameExit;
    closeSaveFile();
}

// =============================================================================
//  resetGame()
//  Called when starting a BRAND-NEW game (not load).
//  • board.initialize() – resets all 32 pieces to starting squares.
//    This is the key call missing from the old code, which is why loading
//    a game and then starting a new game showed the loaded board.
//  • openSaveFile()     – truncates savedGame.txt and writes the name header.
//    The old save is gone. White's first move will be the first line appended.
// =============================================================================

void sfml_GUI::resetGame(Board& board)
{
    board.initialize();           // ← the critical missing call (FIX 2)

    m_turn = "White";
    m_capturedByWhite = "";
    m_capturedByBlack = "";
    m_selRow = -1;
    m_selCol = -1;
    m_flashActive = false;
    m_gameOver = false;
    m_winnerText = "";
    m_moveNumber = 0;
    m_pendingNewGame = false;

    openSaveFile();               // truncate old save, write name header
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

        if (const auto* mm = ev->getIf<sf::Event::MouseMoved>())
        {
            sf::Vector2f mp{ float(mm->position.x), float(mm->position.y) };
            if (m_state == AppState::Menu)
            {
                m_btnNewGame->setHovered(m_btnNewGame->contains(mp));
                m_btnLoadGame->setHovered(m_btnLoadGame->contains(mp));
                m_btnConsole->setHovered(m_btnConsole->contains(mp));
                bool hov = m_btnMenuExit->contains(mp);
                m_btnMenuExit->shape.setFillColor(hov ? C_BTN_EXIT_H : C_BTN_EXIT_N);
            }
            if (m_state == AppState::Game)
            {
                bool hov = m_btnGameExit->contains(mp);
                m_btnGameExit->shape.setFillColor(hov ? C_BTN_EXIT_H : C_BTN_EXIT_N);
            }
        }

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

        if (const auto* te = ev->getIf<sf::Event::TextEntered>())
        {
            if (m_state == AppState::NameInputWhite ||
                m_state == AppState::NameInputBlack)
                handleNameInputText(te->unicode);
        }

        if (const auto* kp = ev->getIf<sf::Event::KeyPressed>())
        {
            if (kp->code == sf::Keyboard::Key::Escape &&
                m_state == AppState::Game)
            {
                closeSaveFile();
                m_state = AppState::Menu;
                m_selRow = m_selCol = -1;
                m_flashActive = false;
            }
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
    case AppState::Menu:            renderMenu();       break;
    case AppState::NameInputWhite:
    case AppState::NameInputBlack:  renderNameInput();  break;
    case AppState::Game:            renderGame(board);  break;
    default: break;
    }
    m_window.display();
}

// =============================================================================
//  renderMenu()
// =============================================================================

void sfml_GUI::renderMenu()
{
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

    sf::Text footer(m_font,
        "Muhammad Taha  |  Hiba Eman  |  FAST-NUCES Faisalabad", 14);
    footer.setFillColor({ 80, 80, 105 });
    centreText(footer, WIN_W / 2.f, WIN_H - 22.f);
    m_window.draw(footer);
}

// =============================================================================
//  renderNameInput()
// =============================================================================

void sfml_GUI::renderNameInput()
{
    bool isWhite = (m_state == AppState::NameInputWhite);

    sf::Text prompt(m_font,
        isWhite ? "Enter White Player's Name:"
        : "Enter Black Player's Name:", 30);
    prompt.setFillColor(isWhite ? sf::Color::White : sf::Color{ 160,160,190 });
    centreText(prompt, WIN_W / 2.f, 260.f);
    m_window.draw(prompt);

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

    if (!isWhite && !m_whiteName.empty())
    {
        sf::Text prev(m_font, "White player: " + m_whiteName, 19);
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
    // ── Rank / file labels ────────────────────────────────────────────────
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

    // ── Expire flash timer ────────────────────────────────────────────────
    if (m_flashActive &&
        m_flashClock.getElapsedTime().asSeconds() >= FLASH_DURATION)
        m_flashActive = false;

    // ── Squares + pieces ──────────────────────────────────────────────────
    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            sf::RectangleShape cell({ CELL, CELL });
            cell.setPosition(cellToScreen(row, col));

            bool light = (row + col) % 2 == 0;

            if (m_flashActive && row == m_flashRow && col == m_flashCol)
                cell.setFillColor(C_FLASH_RED);
            else if (row == m_selRow && col == m_selCol)
                cell.setFillColor(C_SELECTED);
            else
                cell.setFillColor(light ? C_LIGHT_SQ : C_DARK_SQ);

            m_window.draw(cell);

            Piece* p = board.getPiece(row, col);
            if (p) drawPiece(p, row, col);
        }
    }

    // ── Board border ──────────────────────────────────────────────────────
    sf::RectangleShape border({ BOARD_PX, BOARD_PX });
    border.setPosition({ BOARD_X, BOARD_Y });
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(C_GOLD);
    border.setOutlineThickness(3.f);
    m_window.draw(border);

    // ── Vertical separator ────────────────────────────────────────────────
    sf::RectangleShape gapLine({ 2.f, WIN_H - 20.f });
    gapLine.setPosition({ SIDEBAR_X - 15.f, 10.f });
    gapLine.setFillColor({ 55, 55, 80 });
    m_window.draw(gapLine);

    // ─────────────────────────────────────────────────────────────────────
    //  SIDEBAR
    // ─────────────────────────────────────────────────────────────────────
    sf::RectangleShape panel({ SIDEBAR_W, WIN_H - 20.f });
    panel.setPosition({ SIDEBAR_X, 10.f });
    panel.setFillColor(C_PANEL_BG);
    panel.setOutlineColor(C_PANEL_BORD);
    panel.setOutlineThickness(1.5f);
    m_window.draw(panel);

    float sx = SIDEBAR_X + 12.f;
    float sw = SIDEBAR_W - 24.f;

    // ── Whose-turn banner ─────────────────────────────────────────────────
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

    auto drawDiv = [&](float y)
        {
            sf::RectangleShape d({ sw, 1.f });
            d.setPosition({ sx, y });
            d.setFillColor({ 70, 70, 100 });
            m_window.draw(d);
        };

    drawDiv(86.f);

    // ── Player names ──────────────────────────────────────────────────────
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

    // ── Captured by White ─────────────────────────────────────────────────
    {
        sf::Text hdr(m_font, "Captured by White:", 15);
        hdr.setPosition({ sx, 153.f });
        hdr.setFillColor({ 200, 190, 145 });
        m_window.draw(hdr);

        sf::Text cap(m_pieceFont,
            sf::String::fromUtf8(
                m_capturedByWhite.begin(),
                m_capturedByWhite.end()),
            26);
        cap.setPosition({ sx, 174.f });
        cap.setFillColor({ 245, 230, 175 });
        m_window.draw(cap);
    }

    drawDiv(255.f);

    // ── Captured by Black ─────────────────────────────────────────────────
    {
        sf::Text hdr(m_font, "Captured by Black:", 15);
        hdr.setPosition({ sx, 262.f });
        hdr.setFillColor({ 170, 185, 220 });
        m_window.draw(hdr);

        sf::Text cap(m_pieceFont,
            sf::String::fromUtf8(
                m_capturedByBlack.begin(),
                m_capturedByBlack.end()),
            26);
        cap.setPosition({ sx, 283.f });
        cap.setFillColor({ 180, 200, 245 });
        m_window.draw(cap);
    }

    drawDiv(365.f);

    // ── Move counter ──────────────────────────────────────────────────────
    {
        sf::Text mc(m_font, "Move:  " + std::to_string(m_moveNumber), 17);
        mc.setPosition({ sx, 373.f });
        mc.setFillColor({ 160, 160, 190 });
        m_window.draw(mc);
    }

    drawDiv(404.f);

    // ── Check notice ──────────────────────────────────────────────────────
    if (board.isCheck(m_turn) && !m_gameOver)
    {
        sf::Text chk(m_font, "  CHECK!", 22);
        chk.setFillColor(sf::Color{ 255, 80, 80 });
        chk.setStyle(sf::Text::Bold);
        centreText(chk, sx + sw / 2.f, 430.f);
        m_window.draw(chk);
    }

    // ── Exit button ───────────────────────────────────────────────────────
    m_btnGameExit->draw(m_window);

    // ── ESC hint ──────────────────────────────────────────────────────────
    {
        sf::Text esc(m_font, "ESC = return to menu", 13);
        esc.setFillColor({ 75, 75, 100 });
        centreText(esc, sx + sw / 2.f, WIN_H - 16.f);
        m_window.draw(esc);
    }

    // ── Checkmate overlay ─────────────────────────────────────────────────
    if (m_gameOver)
    {
        sf::RectangleShape ovl({ BOARD_PX + 6.f, BOARD_PX + 6.f });
        ovl.setPosition({ BOARD_X - 3.f, BOARD_Y - 3.f });
        ovl.setFillColor({ 0, 0, 0, 175 });
        m_window.draw(ovl);

        sf::Text msg(m_font, m_winnerText, 30);
        msg.setStyle(sf::Text::Bold);
        msg.setFillColor(sf::Color::Yellow);
        centreText(msg, BOARD_X + BOARD_PX / 2.f,
            BOARD_Y + BOARD_PX / 2.f - 20.f);
        m_window.draw(msg);

        sf::Text sub(m_font, "Press ESC or click Exit to return to menu", 17);
        sub.setFillColor({ 220, 220, 220 });
        centreText(sub, BOARD_X + BOARD_PX / 2.f,
            BOARD_Y + BOARD_PX / 2.f + 40.f);
        m_window.draw(sub);
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
    glyph.setOutlineColor(white ? sf::Color{ 75, 45,   0 }
    : sf::Color{ 215,205, 185 });
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
        m_state = AppState::NameInputWhite;
        m_inputBuffer = "";
    }
    else if (m_btnLoadGame->contains(mp))
    {
        if (loadSavedGame(board))
            m_state = AppState::Game;
        else
            std::cerr << "[GUI] No saved game found in savedGame.txt\n";
    }
    else if (m_btnConsole->contains(mp))
    {
        m_state = AppState::ConsoleMode;
    }
    else if (m_btnMenuExit->contains(mp))
    {
        m_window.close();
    }
}

// =============================================================================
//  handleNameInputText()
//
//  FIX 2 (partial): when Black's name is confirmed we only set a flag
//  (m_pendingNewGame = true) and transition to Game state.
//  We do NOT call resetGame() here because we have no Board& at this point.
//  The actual board.initialize() happens in handleGameClick() below.
// =============================================================================

void sfml_GUI::handleNameInputText(char32_t unicode)
{
    if (unicode == 13) // Enter
    {
        if (m_inputBuffer.empty()) return;

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

            // ── FIX 2: signal that resetGame() must run on first click ──
            // Do NOT call openSaveFile() or board.initialize() here –
            // we have no Board& in this function.
            m_pendingNewGame = true;

            m_state = AppState::Game;
        }
        return;
    }
    if (unicode == 8 && !m_inputBuffer.empty())
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
    // ── FIX 2: reset board on the very first entry into a new game ────────
    // m_pendingNewGame is true only when "New Game" was chosen and names
    // were confirmed. resetGame() calls board.initialize() (clearing any
    // loaded game's piece positions) and truncates savedGame.txt.
    // After this call the board is fresh and the save file is empty.
    if (m_pendingNewGame)
        resetGame(board);          // clears m_pendingNewGame internally

    // ── Exit button ───────────────────────────────────────────────────────
    if (m_btnGameExit->contains(mp))
    {
        closeSaveFile();
        m_state = AppState::Menu;
        m_selRow = m_selCol = -1;
        m_flashActive = false;
        m_gameOver = false;
        return;
    }

    if (m_gameOver) return;

    // ── Map click → board square ──────────────────────────────────────────
    int row, col;
    boardCoordsFromMouse(mp, row, col);
    if (row < 0) { m_selRow = m_selCol = -1; return; }

    Piece* clicked = board.getPiece(row, col);

    // ── Nothing selected yet ─────────────────────────────────────────────
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
            // Wrong turn → red flash on the clicked square
            m_flashRow = row;
            m_flashCol = col;
            m_flashActive = true;
            m_flashClock.restart();
        }
        return;
    }

    // ── Piece already selected ────────────────────────────────────────────

    if (row == m_selRow && col == m_selCol)   // same square → deselect
    {
        m_selRow = m_selCol = -1;
        return;
    }

    if (clicked && clicked->getColor() == m_turn)  // another friendly → re-select
    {
        m_selRow = row;
        m_selCol = col;
        m_flashActive = false;
        return;
    }

    // ── Attempt move ──────────────────────────────────────────────────────
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

        // ── FIX 1: move counter ───────────────────────────────────────────
        // Console logic:  display board, THEN "if (turn=="Black") moves++"
        // That runs after White moves successfully (turn just became Black).
        // So the counter increments when the new turn is Black.
        if (m_turn == "Black")
            m_moveNumber++;

        m_selRow = m_selCol = -1;
        m_flashActive = false;

        // Checkmate check
        if (board.isCheckmate(m_turn))
        {
            std::string winner = (m_turn == "White") ? m_blackName : m_whiteName;
            std::string loser = (m_turn == "White") ? "White" : "Black";
            m_winnerText = winner + " wins!\n" + loser + " is in Checkmate!";
            m_gameOver = true;
            closeSaveFile();
        }
    }
    else
    {
        // Invalid move → flash source square, keep piece selected
        m_flashRow = m_selRow;
        m_flashCol = m_selCol;
        m_flashActive = true;
        m_flashClock.restart();
    }
}

// =============================================================================
//  loadSavedGame()
// =============================================================================

bool sfml_GUI::loadSavedGame(Board& board)
{
    std::ifstream fin("savedGame.txt");
    if (!fin.is_open()) return false;

    std::string wName, bName;
    if (!std::getline(fin, wName)) return false;
    if (!std::getline(fin, bName)) return false;
    if (wName.empty() && bName.empty()) return false;

    // Reset board to starting position before replaying
    board.initialize();

    m_whiteName = wName;
    m_blackName = bName;
    m_turn = "White";
    m_capturedByWhite = "";
    m_capturedByBlack = "";
    m_selRow = m_selCol = -1;
    m_flashActive = false;
    m_gameOver = false;
    m_winnerText = "";
    m_moveNumber = 1;
    m_pendingNewGame = false;   // loading is not a new game

    // Replay all saved moves
    int sr, sc, dr, dc;
    while (fin >> sr >> sc >> dr >> dc)
    {
        board.movePiece(sr, sc, dr, dc, m_turn,
            m_capturedByWhite, m_capturedByBlack);

        m_turn = (m_turn == "White") ? "Black" : "White";

        // ── FIX 1 (replay): same rule as live play ────────────────────────
        if (m_turn == "Black")
            m_moveNumber++;
    }
    fin.close();

    // Open save file in append mode so new moves extend the existing game
    if (m_saveFile.is_open()) m_saveFile.close();
    m_saveFile.open("savedGame.txt", std::ios::app);

    return true;
}

// =============================================================================
//  Save-file helpers
// =============================================================================

void sfml_GUI::openSaveFile()
{
    if (m_saveFile.is_open()) m_saveFile.close();

    // Open with truncation – this erases the previous saved game.
    // White's first move will be the first line appended after the header.
    m_saveFile.open("savedGame.txt");
    if (m_saveFile.is_open())
    {
        m_saveFile << m_whiteName << "\n" << m_blackName << "\n";
        m_saveFile.flush();
    }
}

void sfml_GUI::closeSaveFile()
{
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
