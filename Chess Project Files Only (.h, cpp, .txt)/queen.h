#pragma once
#ifndef QUEEN_H
#define QUEEN_H
#include "Piece.h"

class Queen : public Piece
{
public:
    Queen(string c) : Piece(c, c == "White" ? "♕" : "♛") {}
    
    bool isValidMove(int sr, int sc, int dr, int dc, Board* board) override;
};

#endif