#include "Pawn.h"
#include "Rook.h"
#include "Board.h"
#include "Bishop.h"
#include "Queen.h"
#include "King.h"
#include <cmath>
#include <iostream>
using namespace std;

bool Pawn::isValidMove(int sr, int sc, int dr, int dc, Board* board) {
    int direction = (color == "White") ? -1 : 1;
    Piece* destPiece = board->getPiece(dr, dc);
    // Forward move (1 step)
    if (dc == sc && dr == sr + direction) {
        if (destPiece == nullptr) {
            isFirstMove = false;
            return true;
        }
    }
    // First move (2 steps)
    if (isFirstMove && dc == sc && dr == sr + 2 * direction) {
        Piece* middle = board->getPiece(sr + direction, sc);
        if (destPiece == nullptr && middle == nullptr) {
            isFirstMove = false;
            return true;
        }
    }
    // Diagonal capture 
    if (abs(dc - sc) == 1 && dr == sr + direction) {
        if (destPiece != nullptr && destPiece->getColor() != color) {
            isFirstMove = false;
            return true;
        }
    }
    return false;
}

bool Rook::isValidMove(int sr, int sc, int dr, int dc, Board* board)
{
    //  Must move straight
    if (sr != dr && sc != dc)
        return false;
    // Horizontal movement
    if (sr == dr)
    {
        int step = (dc > sc) ? 1 : -1;
        for (int c = sc + step; c != dc; c += step)
        {
            if (board->getPiece(sr, c) != NULL)
                return false; // blocked
        }
    }
    //  Vertical movement
    else
    {
        int step = (dr > sr) ? 1 : -1;
        for (int r = sr + step; r != dr; r += step)
        {
            if (board->getPiece(r, sc) != NULL)
                return false; // blocked
        }
    }
    return true;
}

bool Bishop::isValidMove(int sr, int sc, int dr, int dc, Board* board)
{
    //  Must move diagonally
    if (abs(dr - sr) != abs(dc - sc))
        return false;
    // Determine direction
    int rowStep = (dr > sr) ? 1 : -1;
    int colStep = (dc > sc) ? 1 : -1;
    int r = sr + rowStep;
    int c = sc + colStep;
    // Check path blocking
    while (r != dr && c != dc)
    {
        if (board->getPiece(r, c) != NULL)
            return false;
        r += rowStep;
        c += colStep;
    }
    return true;
}

bool Queen::isValidMove(int sr, int sc, int dr, int dc, Board* board)
{
    //  Rook-like movement
    if (sr == dr || sc == dc)
    {
        // horizontal
        if (sr == dr)
        {
            int step = (dc > sc) ? 1 : -1;
            for (int c = sc + step; c != dc; c += step)
            {
                if (board->getPiece(sr, c) != NULL)
                    return false;
            }
        }
        // vertical
        else
        {
            int step = (dr > sr) ? 1 : -1;
            for (int r = sr + step; r != dr; r += step)
            {
                if (board->getPiece(r, sc) != NULL)
                    return false;
            }
        }
        return true;
    }
    // Bishop-like movement
    if (abs(dr - sr) == abs(dc - sc))
    {
        int rowStep = (dr > sr) ? 1 : -1;
        int colStep = (dc > sc) ? 1 : -1;
        int r = sr + rowStep;
        int c = sc + colStep;
        while (r != dr && c != dc)
        {
            if (board->getPiece(r, c) != NULL)
                return false;
            r += rowStep;
            c += colStep;
        }
        return true;
    }
    return false;
}

// ---------------------------
// FIND KING
// ---------------------------
void Board::findKing(string color, int& kr, int& kc)
{
    kr = -1;
    kc = -1;

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (grid[i][j] != NULL &&
                grid[i][j]->getColor() == color)
            {
                if (dynamic_cast<King*>(grid[i][j]) != NULL)
                {
                    kr = i;
                    kc = j;
                    return;
                }
            }
        }
    }
}

// ---------------------------
// IS CHECK
// ---------------------------
bool Board::isCheck(string color)
{
    int kr, kc;
    findKing(color, kr, kc);

    if (kr == -1 || kc == -1)
        return false;

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (grid[i][j] != NULL &&
                grid[i][j]->getColor() != color)
            {
                // HANDLE ENEMY KING SEPARATELY
                if (dynamic_cast<King*>(grid[i][j]) != NULL)
                {
                    if (abs(i - kr) <= 1 &&
                        abs(j - kc) <= 1)
                        return true;
                }
                else
                {
                    if (grid[i][j]->isValidMove(i, j, kr, kc, this))
                        return true;
                }
            }
        }
    }

    return false;
}

// ---------------------------
// CAN ESCAPE CHECK
// ---------------------------
bool Board::canEscape(string color)
{
    for (int sr = 0; sr < 8; sr++)
    {
        for (int sc = 0; sc < 8; sc++)
        {
            if (grid[sr][sc] != NULL &&
                grid[sr][sc]->getColor() == color)
            {
                Piece* moving = grid[sr][sc];

                for (int dr = 0; dr < 8; dr++)
                {
                    for (int dc = 0; dc < 8; dc++)
                    {
                        // skip same square
                        if (sr == dr && sc == dc)
                            continue;

                        // cannot capture own piece
                        if (grid[dr][dc] != NULL &&
                            grid[dr][dc]->getColor() == color)
                            continue;

                        // valid move?
                        if (moving->isValidMove(sr, sc, dr, dc, this))
                        {
                            // SAVE
                            Piece* temp = grid[dr][dc];

                            // MOVE
                            grid[dr][dc] = moving;
                            grid[sr][sc] = NULL;

                            bool stillCheck = isCheck(color);

                            // UNDO
                            grid[sr][sc] = moving;
                            grid[dr][dc] = temp;

                            if (!stillCheck)
                                return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

// ---------------------------
// IS CHECKMATE
// ---------------------------
bool Board::isCheckmate(string color)
{
    if (!isCheck(color))
        return false;

    if (canEscape(color))
        return false;

    return true;
}

bool King::isValidMove(int sr, int sc, int dr, int dc, Board* board)
{
    int r = abs(dr - sr);
    int c = abs(dc - sc);

    // -------------------
    // NORMAL KING MOVE
    // -------------------
    if (r <= 1 && c <= 1)
    {
        Piece* temp = board->accessCell(dr, dc);
        Piece* king = board->accessCell(sr, sc);

        board->accessCell(dr, dc) = king;
        board->accessCell(sr, sc) = NULL;

        bool inCheck = board->isCheck(color);

        board->accessCell(sr, sc) = king;
        board->accessCell(dr, dc) = temp;

        return !inCheck;
    }

    // -------------------
    // CASTLING
    // -------------------
    if (r == 0 && c == 2 && !hasMoved)
    {
        int row = sr;

        if (board->isCheck(color))
            return false;

        // KINGSIDE
        if (dc > sc)
        {
            Piece* rook = board->getPiece(row, 7);
            Rook* rPtr = dynamic_cast<Rook*>(rook);

            if (rPtr &&
                rook->getColor() == color &&
                !rPtr->getMoved())
            {
                for (int col = sc + 1; col < 7; col++)
                {
                    if (board->getPiece(row, col) != NULL)
                        return false;
                }

                for (int col = sc + 1; col <= sc + 2; col++)
                {
                    Piece* temp = board->accessCell(row, col);
                    Piece* king = board->accessCell(sr, sc);

                    board->accessCell(row, col) = king;
                    board->accessCell(sr, sc) = NULL;

                    bool chk = board->isCheck(color);

                    board->accessCell(sr, sc) = king;
                    board->accessCell(row, col) = temp;

                    if (chk)
                        return false;
                }

                return true;
            }
        }
        else // QUEENSIDE
        {
            Piece* rook = board->getPiece(row, 0);
            Rook* rPtr = dynamic_cast<Rook*>(rook);

            if (rPtr &&
                rook->getColor() == color &&
                !rPtr->getMoved())
            {
                for (int col = sc - 1; col > 0; col--)
                {
                    if (board->getPiece(row, col) != NULL)
                        return false;
                }

                for (int col = sc - 1; col >= sc - 2; col--)
                {
                    Piece* temp = board->accessCell(row, col);
                    Piece* king = board->accessCell(sr, sc);

                    board->accessCell(row, col) = king;
                    board->accessCell(sr, sc) = NULL;

                    bool chk = board->isCheck(color);

                    board->accessCell(sr, sc) = king;
                    board->accessCell(row, col) = temp;

                    if (chk)
                        return false;
                }

                return true;
            }
        }
    }

    return false;
}
