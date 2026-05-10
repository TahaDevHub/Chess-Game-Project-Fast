#include "Board.h"
#include"taha_functions.h"

Board::Board()
{
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            grid[i][j] = NULL;

    initialize();
}

void Board::initialize()
{
    // Pawns
    for (int i = 0; i < 8; i++)
    {
        grid[1][i] = new Pawn("Black");
        grid[6][i] = new Pawn("White");
    }

    // Rooks
    grid[0][0] = new Rook("Black");
    grid[0][7] = new Rook("Black");
    grid[7][0] = new Rook("White");
    grid[7][7] = new Rook("White");

    // Knights
    grid[0][1] = new Knight("Black");
    grid[0][6] = new Knight("Black");
    grid[7][1] = new Knight("White");
    grid[7][6] = new Knight("White");

    // Bishops
    grid[0][2] = new Bishop("Black");
    grid[0][5] = new Bishop("Black");
    grid[7][2] = new Bishop("White");
    grid[7][5] = new Bishop("White");

    // Queens
    grid[0][3] = new Queen("Black");
    grid[7][3] = new Queen("White");

    // Kings
    grid[0][4] = new King("Black");
    grid[7][4] = new King("White");
}

void Board::display(const int moves, const int sc, const int sr, const int dc, const int dr,string capturedByWhite, string capturedByBlack)
{
    
    verticlePadding();
    color(lightGray, BLACK);
    horizontalPadding(); cout << capturedByWhite << endl;
    horizontalPadding(); cout << capturedByBlack << endl;
    color(lightRed, BLACK);
    horizontalPadding(); cout << "╭───────────────────────.★..─────╮" << endl;
    horizontalPadding(); cout << "|           MOVE # "<<moves<<"             |" << endl;
    horizontalPadding(); cout << "╰─────..★.───────────────────────╯" << endl;
    horizontalPadding();
    color(brightWhite, BLACK); // bright white text with black bg

    cout << "╔════════════════════════════════╗\n";

    horizontalPadding();
    cout << "║                                ║\n";

    horizontalPadding();
    cout << "║     a  b  c  d  e  f  g  h     ║\n";

    for (int i = 0; i < 8; i++)
    {
        horizontalPadding();

        cout << "║  ";
        cout << 8 - i << " ";

        for (int j = 0; j < 8; j++)
        {
            if ((i + j) % 2 == 0)
                color(black, LIGHTGRAY);
            else
                color(black, BLUE);
            if (i == sr && j == sc)
                color(black, LIGHTGREEN);
                //color(0, 224);
            if (i == dr && j == dc)
                color(black, GREEN);
                //color(0, 96);
            
            if (grid[i][j] == NULL)
                cout << "   ";
            else
                cout << " " << grid[i][j]->getSymbol() << " ";
        }
        color(15, 0);

        cout << " " << 8 - i;
        cout << "  ║\n";
    }
    horizontalPadding();
    cout << "║     a  b  c  d  e  f  g  h     ║\n";
    horizontalPadding();
    cout << "╚════════════════════════════════╝\n";

}

bool Board::movePiece(int sr, int sc, int dr, int dc, string turn, string &capturedByWhite, string &capturedByBlack)
{
    //temp4Undo = grid[sr][sc]->getSymbol();
    //  Bounds check
    if (sr < 0 || sr >= 8 || sc < 0 || sc >= 8 ||
        dr < 0 || dr >= 8 || dc < 0 || dc >= 8)
        return false;

    // No piece at source
    if (grid[sr][sc] == NULL)
        return false;

    //  Turn check
    if (grid[sr][sc]->getColor() != turn)
        return false;

    //  Prevent capturing own piece 
    if (grid[dr][dc] != NULL &&
        grid[dr][dc]->getColor() == turn)
        return false;

    //  Move validation 
    if (!grid[sr][sc]->isValidMove(sr, sc, dr, dc, this))
        return false;

    //  PREVENT MOVE INTO CHECK

    Piece* temp = grid[dr][dc];
    Piece* moving = grid[sr][sc];

    // simulate
    grid[dr][dc] = moving;
    grid[sr][sc] = NULL;

    if (isCheck(turn))
    {
        // undo
        grid[sr][sc] = moving;
        grid[dr][dc] = temp;
        return false;
    }

    // undo (because we will do actual move below)
    grid[sr][sc] = moving;
    grid[dr][dc] = temp;
    if(turn=="White"&&grid[dr][dc]!=NULL)
        capturedByWhite += " "+ grid[dr][dc]->getSymbol();
    if(turn=="Black" && grid[dr][dc] != NULL)
        capturedByBlack += " "+grid[dr][dc]->getSymbol();
    //  Move piece
    grid[dr][dc] = grid[sr][sc];
    grid[sr][sc] = NULL;

    //if (isCheckmate("White"))
    //    cout << "White is CHECKMATED!\n";

    //if (isCheckmate("Black"))
    //    cout << "Black is CHECKMATED!\n";
    // mark king moved
    King* k = dynamic_cast<King*>(grid[dr][dc]);
    if (k) k->setMoved();

    // mark rook moved
    Rook* r = dynamic_cast<Rook*>(grid[dr][dc]);
    if (r) r->setMoved();

    if (k && abs(dc - sc) == 2)
    {
        if (dc > sc) // kingside
        {
            grid[dr][5] = grid[dr][7];
            grid[dr][7] = NULL;
        }
        else // queenside
        {
            grid[dr][3] = grid[dr][0];
            grid[dr][0] = NULL;
        }
    }


    return true;
}