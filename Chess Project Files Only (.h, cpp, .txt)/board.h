#pragma once
#ifndef BOARD_H
#define BOARD_H

#include "Pawn.h"
#include "Rook.h"
#include "Knight.h"
#include "Bishop.h"
#include "Queen.h"
#include "King.h"

//#include "functions.h"
class Board
{
private:
    Piece* grid[8][8];  // made pointer of Piece because as we know tha we ca point to any child class then.
    //string temp4Undo;

public:
    Board();
    void initialize();
    void display(const int moves, const int sc, const int sr, const int dc, const int dr, string capturedByWhite, string capturedByBlack);
    bool movePiece(int sr, int sc, int dr, int dc, string turn, string &capturedByWhite, string &capturedByBlack);
    Piece* getPiece(int r, int c) { return grid[r][c]; }
    // CHECK & CHECKMATE FUNCTIONS
    void findKing(string color, int& kr, int& kc);
    bool isCheck(string color);
    bool canEscape(string color);
    bool isCheckmate(string color);
    Piece*& accessCell(int r, int c) { return grid[r][c]; }
};
// Move piece from (sr, sc) to (dr, dc)
// sr = source row
// sc = source column
//
// dr = destination row
// dc = destination column
#endif