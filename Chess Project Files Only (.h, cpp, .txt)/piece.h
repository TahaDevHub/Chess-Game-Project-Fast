#pragma once
#ifndef PIECE_H
#define PIECE_H

#include"libraries.h"

using namespace std;
class Board;
class Piece
{
protected:
    string color;   // color means White or Black
    string symbol;  // unicode symbol of piece

public:
    Piece(string c, string s)
    {
        color = c;
        symbol = s;
    }

    virtual bool isValidMove(int sr, int sc, int dr, int dc, Board* board) = 0;

    string getColor()
    {
        return color;
    }

    string getSymbol()
    {
        return symbol;
    }

    virtual ~Piece() {}
};

#endif

//taha_functions.h
