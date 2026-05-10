#pragma once
#ifndef KNIGHT_H
#define KNIGHT_H
#include "Piece.h"

class Knight : public Piece
{
public:
    Knight(string c) : Piece(c, c == "White" ? "♘" : "♞") {}
   
    bool isValidMove(int sr, int sc, int dr, int dc, Board* board)
    {
        int r = abs(dr - sr);
        int c2 = abs(dc - sc);
        return (r == 2 && c2 == 1) || (r == 1 && c2 == 2);
    }
};

#endif