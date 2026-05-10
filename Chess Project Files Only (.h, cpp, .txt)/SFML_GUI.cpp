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
//  Font loader helper
// ─────────────────────────────────────────────────────────────────────────────
static bool tryOpenFont(sf::Font& font,
    std::initializer_list<const char*> paths)
{
    for (const char* p : paths)
        if (font.openFromFile(p)) return true;
    return false;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Piece → UTF-32 glyph  (avoids any encoding ambiguity on Windows/Linux)
// ─────────────────────────────────────────────────────────────────────────────
static sf::String pieceGlyph(Piece* p)
{
    if (!p) return {};
    const std::string& s = p->getSymbol();
    // UTF-8 bytes produced by your Piece constructors on a UTF-8 source file:
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
    return sf::String::fromUtf8(s.begin(), s.end());       // fallback
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
void Button::draw(sf::RenderWindow& w) const
{
    w.draw(shape); w.draw(label);
}
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

    // ── UI font ──────────────────────────────────────────────────────────
    if (!tryOpenFont(m_font, {
            "arial.ttf",
            "C:/Windows/Fonts/arial.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            "/usr/share/fonts/dejavu/DejaVuSans.ttf" }))
            throw std::runtime_error(
                "Cannot load UI font. Place arial.ttf next to the .exe.");

    // ── Piece font (must have chess Unicode glyphs) ───────────────────────
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

    // ─────────────────────────────────────────────────────────────────────
    //  Menu buttons  (centred in 1050-wide window)
    //  Button width 300, centred → x = (1050-300)/2 = 375
    // ─────────────────────────────────────────────────────────────────────
    constexpr float MBX = 375.f;   // left edge of menu buttons
    constexpr float MBW = 300.f;   // button width
    constexpr float MBH = 56.f;   // button height
    constexpr float MBG = 18.f;   // gap between buttons

    m_btnNewGame = new Button(m_font, "New Game",
        { MBX, 310.f }, { MBW, MBH });
    m_btnLoadGame = new Button(m_font, "Load Saved Game",
        { MBX, 310.f + 1 * (MBH + MBG) }, { MBW, MBH });
    m_btnConsole = new Button(m_font, "Play Console",
        { MBX, 310.f + 2 * (MBH + MBG) }, { MBW, MBH });
    m_btnMenuExit = new Button(m_font, "Exit",
        { MBX, 310.f + 3 * (MBH + MBG) }, { MBW, MBH });
    m_btnMenuExit->setColors(C_BTN_EXIT_N, { 255, 120, 120 });

    // ─────────────────────────────────────────────────────────────────────
    //  In-game exit button  – sits inside the right sidebar
    // ─────────────────────────────────────────────────────────────────────
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

        // ── Mouse hover ──────────────────────────────────────────────────
        if (const auto* mm = ev->getIf<sf::Event::MouseMoved>())
        {
            sf::Vector2f mp{ float(mm->position.x), float(mm->position.y) };

            if (m_state == AppState::Menu)
            {
                m_btnNewGame->setHovered(m_btnNewGame->contains(mp));
                m_btnLoadGame->setHovered(m_btnLoadGame->contains(mp));
                m_btnConsole->setHovered(m_btnConsole->contains(mp));
                // Exit btn keeps its own red hues
                bool hov = m_btnMenuExit->contains(mp);
                m_btnMenuExit->shape.setFillColor(hov ? C_BTN_EXIT_H : C_BTN_EXIT_N);
            }
            if (m_state == AppState::Game)
            {
                bool hov = m_btnGameExit->contains(mp);
                m_btnGameExit->shape.setFillColor(hov ? C_BTN_EXIT_H : C_BTN_EXIT_N);
            }
        }

        // ── Mouse click ──────────────────────────────────────────────────
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

        // ── Text input ───────────────────────────────────────────────────
        if (const auto* te = ev->getIf<sf::Event::TextEntered>())
        {
            if (m_state == AppState::NameInputWhite ||
                m_state == AppState::NameInputBlack)
                handleNameInputText(te->unicode);
        }

        // ── Keyboard ────────────────────────────────────────────────────
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
    case AppState::Menu:           renderMenu();        break;
    case AppState::NameInputWhite:
    case AppState::NameInputBlack: renderNameInput();   break;
    case AppState::Game:           renderGame(board);   break;
    default: break;
    }
    m_window.display();
}

// =============================================================================
//  renderMenu()
// =============================================================================

void sfml_GUI::renderMenu()
{
    // ── Title ─────────────────────────────────────────────────────────────
    sf::Text title(m_font, "CHESS", 82);
    title.setStyle(sf::Text::Bold);
    title.setFillColor(C_GOLD);
    centreText(title, WIN_W / 2.f, 160.f);
    m_window.draw(title);

    sf::Text sub(m_font, "C++ Edition  |  SFML 3", 20);
    sub.setFillColor({ 150, 150, 180 });
    centreText(sub, WIN_W / 2.f, 230.f);
    m_window.draw(sub);

    // ── Divider ───────────────────────────────────────────────────────────
    sf::RectangleShape rule({ 340.f, 2.f });
    rule.setPosition({ (WIN_W - 340.f) / 2.f, 265.f });
    rule.setFillColor({ 160, 140, 80, 160 });
    m_window.draw(rule);

    // ── Buttons ───────────────────────────────────────────────────────────
    m_btnNewGame->draw(m_window);
    m_btnLoadGame->draw(m_window);
    m_btnConsole->draw(m_window);
    m_btnMenuExit->draw(m_window);

    // ── Footer ────────────────────────────────────────────────────────────
    sf::Text footer(m_font, "Muhammad Taha  |  Hiba Eman  |  FAST-NUCES Faisalabad", 14);
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
    // ═════════════════════════════════════════════════════════════════════
    //  LEFT SIDE  –  board  (x: 30 … 630,  y: 75 … 675)
    // ═════════════════════════════════════════════════════════════════════

    // ── Rank / file labels ────────────────────────────────────────────────
    for (int i = 0; i < 8; i++)
    {
        // Rank numbers  (left of board)
        sf::Text rankLbl(m_font,
            std::string(1, static_cast<char>('8' - i)), 15);
        rankLbl.setFillColor({ 170, 160, 120 });
        rankLbl.setPosition({
            BOARD_X - 18.f,
            BOARD_Y + i * CELL + CELL / 2.f - 10.f });
        m_window.draw(rankLbl);

        // File letters  (below board)
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

    // ── Squares + pieces ─────────────────────────────────────────────────
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

    // ── Vertical gap line ─────────────────────────────────────────────────
    sf::RectangleShape gapLine({ 2.f, WIN_H - 20.f });
    gapLine.setPosition({ SIDEBAR_X - 15.f, 10.f });
    gapLine.setFillColor({ 55, 55, 80 });
    m_window.draw(gapLine);

    // ═════════════════════════════════════════════════════════════════════
    //  RIGHT SIDE  –  sidebar  (x: 660 … 1030)
    // ═════════════════════════════════════════════════════════════════════

    // ── Sidebar panel ─────────────────────────────────────────────────────
    sf::RectangleShape panel({ SIDEBAR_W, WIN_H - 20.f });
    panel.setPosition({ SIDEBAR_X, 10.f });
    panel.setFillColor(C_PANEL_BG);
    panel.setOutlineColor(C_PANEL_BORD);
    panel.setOutlineThickness(1.5f);
    m_window.draw(panel);

    float sx = SIDEBAR_X + 12.f;   // text left margin inside sidebar
    float sw = SIDEBAR_W - 24.f;   // usable width

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

    // ── Divider helper lambda ─────────────────────────────────────────────
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

        // Render captured glyphs in rows of up to ~8 per line
        // We display them as a single wrapped string using the piece font
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

    // ═════════════════════════════════════════════════════════════════════
    //  Checkmate overlay  (drawn last, over everything)
    // ═════════════════════════════════════════════════════════════════════
    if (m_gameOver)
    {
        sf::RectangleShape ovl({ BOARD_PX + 6.f, BOARD_PX + 6.f });
        ovl.setPosition({ BOARD_X - 3.f, BOARD_Y - 3.f });
        ovl.setFillColor({ 0, 0, 0, 175 });
        m_window.draw(ovl);

        sf::Text msg(m_font, m_winnerText, 30);
        msg.setStyle(sf::Text::Bold);
        msg.setFillColor(sf::Color::Yellow);
        centreText(msg, BOARD_X + BOARD_PX / 2.f, BOARD_Y + BOARD_PX / 2.f - 20.f);
        m_window.draw(msg);

        sf::Text sub(m_font, "Press ESC or click Exit to return to menu", 17);
        sub.setFillColor({ 220, 220, 220 });
        centreText(sub, BOARD_X + BOARD_PX / 2.f, BOARD_Y + BOARD_PX / 2.f + 40.f);
        m_window.draw(sub);

        /*std::ofstream fout;
        fout.open("savedGame.txt");
        fout.close();*/
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
    glyph.setFillColor(white ? sf::Color{ 255, 252, 215 } : sf::Color{ 12, 8, 4 });
    glyph.setOutlineColor(white ? sf::Color{ 75,  45,   0 } : sf::Color{ 215,205,185 });
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
        {
            // Game loaded successfully – go straight into Game state
            m_state = AppState::Game;
        }
        else
        {
            // No saved game – show a quick timed notice (we'll just do nothing
            // and stay on the menu; the user will see nothing happens)
            std::cerr << "[GUI] No saved game found in savedGame.txt\n";
        }
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
        else
        {
            m_blackName = m_inputBuffer;
            m_inputBuffer = "";

            // We need a fresh board – resetGame() does that
            // but we need a Board* .  The board ref comes through run().
            // We'll signal through a flag and reset in handleGameClick's
            // first call.  Simpler: store a pending reset flag.
            m_turn = "White";
            m_capturedByWhite = "";
            m_capturedByBlack = "";
            m_selRow = m_selCol = -1;
            m_flashActive = false;
            m_gameOver = false;
            m_winnerText = "";
            m_moveNumber = 1;

            // Open the save file (overwrite)
            openSaveFile();

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
            // Wrong turn → red flash
            m_flashRow = row; m_flashCol = col;
            m_flashActive = true;
            m_flashClock.restart();
        }
        return;
    }

    // ── Piece already selected ────────────────────────────────────────────

    // Same square → deselect
    if (row == m_selRow && col == m_selCol)
    {
        m_selRow = m_selCol = -1;
        return;
    }

    // Another friendly piece → re-select
    if (clicked && clicked->getColor() == m_turn)
    {
        m_selRow = row; m_selCol = col;
        m_flashActive = false;
        return;
    }

    // ── Attempt move via your existing Board::movePiece() ─────────────────
    //  movePiece() enforces: turn, piece rules, no moving into check, castling.
    //  Captured piece symbols are appended to capturedByWhite/Black strings.
    bool ok = board.movePiece(
        m_selRow, m_selCol, row, col,
        m_turn,
        m_capturedByWhite,
        m_capturedByBlack);

    if (ok)
    {
        // ── Save move to file  (same format as console version) ──────────
        appendMove(m_selRow, m_selCol, row, col);

        // ── Switch turn ───────────────────────────────────────────────────
        m_turn = (m_turn == "White") ? "Black" : "White";
        if (m_turn == "Black") m_moveNumber++;   // increment after white moves

        m_selRow = m_selCol = -1;
        m_flashActive = false;

        // ── Checkmate check ───────────────────────────────────────────────
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
        // Invalid move → flash source square
        m_flashRow = m_selRow; m_flashCol = m_selCol;
        m_flashActive = true;
        m_flashClock.restart();
        // Keep piece selected so player can pick a different destination
    }
}

// =============================================================================
//  loadSavedGame()
//  Reads savedGame.txt, replays every recorded move, sets m_whiteName /
//  m_blackName, m_turn, m_capturedBy*, m_moveNumber.
//  Returns true on success, false if file is absent or empty.
// =============================================================================

bool sfml_GUI::loadSavedGame(Board& board)
{
    std::ifstream fin("savedGame.txt");
    if (!fin.is_open()) return false;

    // First two lines are player names (written by console version)
    std::string wName, bName;
    if (!std::getline(fin, wName)) return false;
    if (!std::getline(fin, bName)) return false;

    if (wName.empty() && bName.empty()) return false;

    // Reset everything for a fresh replay
    // We need to re-initialise the board – call initialize() via the
    // public constructor re-use pattern:  destroy contents and rebuild.
    // Board has no public reset(), so we reconstruct it.
    board.~Board();
    new (&board) Board();   // placement new – safe because Board has no
    // virtual base and its ctor fully initialises

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

    int sr, sc, dr, dc;
    while (fin >> sr >> sc >> dr >> dc)
    {
        board.movePiece(sr, sc, dr, dc, m_turn,
            m_capturedByWhite, m_capturedByBlack);
        m_turn = (m_turn == "White") ? "Black" : "White";
        if (m_turn == "Black") m_moveNumber++;
    }
    fin.close();

    // Re-open save file in append mode so new moves are added
    m_saveFile.open("savedGame.txt", std::ios::app);

    return true;
}

// =============================================================================
//  Save-file helpers
// =============================================================================

void sfml_GUI::openSaveFile()
{
    if (m_saveFile.is_open()) m_saveFile.close();
    m_saveFile.open("savedGame.txt");          // truncate / create
    if (m_saveFile.is_open())
    {
        // Write header lines exactly like Game::startNewGame() does
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
        m_saveFile.flush();   // ensure it's written even if game crashes
    }
}

// =============================================================================
//  resetGame()  – not currently called from outside, but available
// =============================================================================

void sfml_GUI::resetGame(Board& board)
{
    board.~Board();
    new (&board) Board();
    m_turn = "White";
    m_capturedByWhite = "";
    m_capturedByBlack = "";
    m_selRow = m_selCol = -1;
    m_flashActive = false;
    m_gameOver = false;
    m_winnerText = "";
    m_moveNumber = 1;
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
