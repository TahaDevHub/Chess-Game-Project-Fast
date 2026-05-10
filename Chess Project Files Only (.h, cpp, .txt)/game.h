#pragma once
#ifndef GAME_H
#define GAME_H

#include "Board.h"

class Game
{
private:
    Board board;
    string turn;
    string turnName;
    string capturedByWhite = "Captured By White: ";
    string capturedByBlack = "Captured By Black: ";

    bool isBlackUndoUsed;
    bool isWhiteUndoUsed;
public:
    Game();
    void startNewGame();
    void loadGame();
};

#endif