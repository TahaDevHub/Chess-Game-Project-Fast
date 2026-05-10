#pragma once
#ifndef PAWN_H
#define PAWN_H

#include "Piece.h"
#include <string>

using namespace std;

class Board;

class Pawn : public Piece {
protected:
    bool isFirstMove; 

public:
    Pawn(string c) : Piece(c, c == "White" ? "♙" : "♟") {
        isFirstMove = true;
    }

    bool isValidMove(int sr, int sc, int dr, int dc, Board* board) override;
};

#endif