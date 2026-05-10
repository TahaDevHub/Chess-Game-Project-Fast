#pragma once
#ifndef KING_H
#define KING_H

#include "Piece.h"

class King : public Piece
{
private:
    bool hasMoved;

public:
    King(string c) : Piece(c, c == "White" ? "♔" : "♚")
    {
        hasMoved = false;
    }

    bool isValidMove(int sr, int sc, int dr, int dc, Board* board) override;

    void setMoved() { hasMoved = true; }
    bool getMoved() { return hasMoved; }
};

#endif