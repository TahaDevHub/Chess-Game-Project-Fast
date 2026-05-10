#pragma once
#include "libraries.h"
#include <windows.h>
#include "Game.h"
#include <string>
#include<thread>	//  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
#include<chrono>	//  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
// Enums for color
enum TEXT_COLOR {
	black, blue, green, aqua, red, purple, yellow, lightGray, gray, lightBlue,
	lightGreen, lightAqua, lightRed, lightPurple, lightYellow, brightWhite
};

enum BG_COLOR {
	BLACK, BLUE = 16, GREEN = 32, AQUA = 48, RED = 64, PURPLE = 80, YELLOW = 96, LIGHTGRAY = 112, GRAY = 128,
	LIGHTBLUE = 144, LIGHTGREEN = 160, LIGHTAQUA = 176, LIGHTRED = 192, LIGHTPURPLE = 208, LIGHTYELLOW = 224, BRIGHTWHITE = 240
};


// Decalarations for Graphical Decorations:
// Decalarations for Graphical Decorations:
void printCentered(std::string text, char border = '|');
void verticlePadding();
void horizontalPadding();
void delay(int milliseconds);
void loadingScreen(std::string title);
void drawDecoration(char left, char fill, char right);
void EndingScreen();

//
//void color(int text_color, int bg_color);
void displayInstructions();
void displayCredits();
char printMainMenu();
void callFunctions(const char choice);
