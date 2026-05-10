#pragma once
#ifndef ROOK_H
#define ROOK_H
#include "Piece.h"

class Rook : public Piece
{
private:
    bool hasMoved;
public:
    Rook(string c) : Piece(c, c == "White" ? "♖" : "♜") { hasMoved = false; }

    bool isValidMove(int sr, int sc, int dr, int dc, Board* board) override;
    void setMoved() { hasMoved = true; }
    bool getMoved() { return hasMoved; }
};

#endif
