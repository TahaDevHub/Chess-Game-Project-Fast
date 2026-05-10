#pragma once
#include"libraries.h"
#include<iostream>
#include <fstream>


void saveMove(ofstream& fout, int sr, int sc, int dr, int dc, string turn);
void saveLoadedMove(ofstream& fout, int sr, int sc, int dr, int dc, string turn);
