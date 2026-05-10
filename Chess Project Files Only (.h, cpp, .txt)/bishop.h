#pragma once
#ifndef BISHOP_H
#define BISHOP_H

#include "Piece.h"

class Bishop : public Piece
{
public:
    Bishop(string c) : Piece(c, c == "White" ? "♗" : "♝") {}

    bool isValidMove(int sr, int sc, int dr, int dc, Board* board) override;
};

#endif