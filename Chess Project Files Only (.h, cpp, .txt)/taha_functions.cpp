#include "taha_functions.h"
// Definitions of functions for Graphical Decorations:
// Definitions of functions for Graphical Decorations:
const int WIDTH = 100; //Almost 100% width of the screen
const int HEIGHT_PAD = 8;
const int WIDTH_PAD = 3;



//Display Content in the centre of the screen
void printCentered(string text, char border)
{
	int len = text.length();
	int padding = (WIDTH - 2 - len) / 2; //Appropriate Padding for content
	int remaining = WIDTH - 2 - padding - len; //Remaining space after padding

	cout << "\t" << border << string(padding, ' ') << text << string(remaining, ' ') << border << endl;

}

//Print some blank lines for centring the content
void verticlePadding()
{
	for (int i = 0; i < HEIGHT_PAD; i++)
	{
		cout << endl;
	}
}
//Print some blank lines for centring the content
void horizontalPadding()
{
	for (int i = 0; i < WIDTH_PAD; i++)
	{
		cout << "\t";
	}
}

//Function to add delay in displaying content
void delay(int milliseconds) {
	clock_t goal = milliseconds * (CLOCKS_PER_SEC / 1000);
	clock_t start = clock();
	while ((clock() - start) < goal);
}
// sleep helper function
void pauseMs(int ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
void loadingScreen(string title)
{
	system("cls");
	verticlePadding();
	cout << endl << endl << endl;
	printCentered(title, ' ');
	cout << endl;

	//Print bars
	cout << "\t   " << string(WIDTH, '_') << "\n";
	cout << "\t  |";

	//Fill the bar with delay

	for (int i = 0; i < WIDTH; i++)
	{
		cout << "#";
		delay(5);
		//pauseMs(1);
	}
	cout << "| 100%" << endl << endl;

	//Add delay before screen comes

	//delay(500);
	pauseMs(500);
}
void EndingScreen() {
	system("cls");
	verticlePadding();
	cout << endl << endl << endl;
	printCentered("A L L A H     H A F I Z   ...", ' ');
	//cout << endl;
	//printCentered("W E   M I S S   Y  O U   ...", ' ');
	cout << endl;

	//Print bars
	cout << "\t   " << string(WIDTH, '_') << "\n";
	cout << "\t  |";

	//Fill the bar with delay

	for (int i = 0; i < WIDTH; i++)
	{
		cout << "#";
		delay(25);
		//pauseMs(1);
	}
	cout << "| 100%" << endl << endl;

	//Add delay before screen comes
	pauseMs(1500);
	cout << endl << endl << endl << endl;
}

void drawDecoration(char left, char fill, char right)
{
	cout << "\t" << left << string(WIDTH - 2, fill) << right << endl;
}
// ==================================================
// ==================================================

void color(int text_color, int bg_color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), text_color + bg_color);
}

// check

//bool isCheck() {
//	
//}

// Main Menu
// ================= INSTRUCTIONS SCREEN =================

void displayInstructions()
{
	system("cls");

	color(lightAqua, BLACK);

	cout << "\n\n";

	cout << "\t\t╔═══════════════════════════════════════════╗\n";
	cout << "\t\t║         ♜ GAME INSTRUCTIONS ♜             ║\n";
	cout << "\t\t╠═══════════════════════════════════════════╣\n";

	color(lightYellow, BLACK);

	cout << "\t\t║ 1. White always plays first.              ║\n";
	cout << "\t\t║                                           ║\n";

	cout << "\t\t║ 2. Enter moves in this format:            ║\n";
	cout << "\t\t║        e2 e4                              ║\n";
	cout << "\t\t║                                           ║\n";

	cout << "\t\t║ 3. First position = source square         ║\n";
	cout << "\t\t║    Second position = destination square   ║\n";
	cout << "\t\t║                                           ║\n";

	cout << "\t\t║ 4. Invalid moves are rejected.            ║\n";
	cout << "\t\t║                                           ║\n";

	color(lightGreen, BLACK);

	cout << "\t\t║ 5. Features Included:                     ║\n";
	cout << "\t\t║      • Check Detection                    ║\n";
	cout << "\t\t║      • Checkmate Detection                ║\n";
	cout << "\t\t║      • Save / Load Game                   ║\n";
	cout << "\t\t║      • Move Highlighting                  ║\n";
	cout << "\t\t║                                           ║\n";

	color(lightRed, BLACK);

	cout << "\t\t║ 6. Special Command:                       ║\n";
	cout << "\t\t║      • exit -> Save and Exit              ║\n";
	cout << "\t\t║                                           ║\n";

	color(lightAqua, BLACK);

	cout << "\t\t║ 7. Input Rules:                           ║\n";
	cout << "\t\t║      • Use spaces between moves           ║\n";
	cout << "\t\t║      • Example: a2 a4                     ║\n";
	cout << "\t\t║                                           ║\n";

	color(lightPurple, BLACK);

	cout << "\t\t║      ♚ Protect Your King & Enjoy! ♚       ║\n";

	color(lightAqua, BLACK);

	cout << "\t\t╚═══════════════════════════════════════════╝\n";

	cout << "\n\n\t\t";

	color(brightWhite, BLACK);

	system("pause");
	Beep(500, 80);
}
void displayCredits(){
	system("cls");
	cout << "\n\n";

	cout << "\t\t╔════════════════════════════════════════════╗\n";
	cout << "\t\t║                ♔ Credits ♕                 ║\n";
	cout << "\t\t╠════════════════════════════════════════════╣\n";

	color(lightYellow, BLACK);

	cout << "\t\t║                                            ║\n";
	cout << "\t\t║   Developed By:                            ║\n";
	cout << "\t\t║                                            ║\n";

	color(lightAqua, BLACK);

	cout << "\t\t║      • Muhammad Taha             (25F-0755)║\n";
	cout << "\t\t║         ╰┈➤ f250755@cfd.nu.edu.pk          ║\n";
	cout << "\t\t║         ╰┈➤ https://github.com/TahaDevHub  ║\n";
	cout << "\t\t║                                            ║\n";

	cout << "\t\t║      • Hiba Eman                 (25F-0596)║\n";
	cout << "\t\t║         ╰┈➤ f250596@cfd.nu.edu.pk          ║\n";

	color(lightYellow, BLACK);

	cout << "\t\t║                                            ║\n";
	cout << "\t\t║   Project: Console Chess Game              ║\n";
	cout << "\t\t║             ╰┈➤ GUI --> Bonus              ║\n";
	cout << "\t\t║                                            ║\n";

	color(lightBlue, BLACK);

	cout << "\t\t║   Developed Using:                         ║\n";
	cout << "\t\t║      • Object Oriented Programming         ║\n";
	cout << "\t\t║      • C++                                 ║\n";
	cout << "\t\t║      • Windows Console API                 ║\n";
	cout << "\t\t║                                            ║\n";

	color(lightPurple, BLACK);

	cout << "\t\t║   OOP Concepts Implemented:                ║\n";
	cout << "\t\t║      • Inheritance                         ║\n";
	cout << "\t\t║      • Polymorphism                        ║\n";
	cout << "\t\t║      • Abstraction                         ║\n";
	cout << "\t\t║      • Encapsulation                       ║\n";
	cout << "\t\t║                                            ║\n";

	color(lightRed, BLACK);

	cout << "\t\t║   Special Thanks:                          ║\n";
	cout << "\t\t║      • FAST-NUCES Faisalabad Campus        ║\n";
	cout << "\t\t║         ╰┈➤ Sir Rizwan-Ul-Haq              ║\n";
	cout << "\t\t║             ★ rizwan.haq@nu.edu.pk         ║\n";
	cout << "\t\t║         ╰┈➤ Mam Amna Waheed                ║\n";
	cout << "\t\t║             ★ amna.waheed@nu.edu.pk        ║\n";

	cout << "\t\t║                                            ║\n";

	color(brightWhite, BLACK);

	cout << "\t\t║         ♜ Thank You For Playing ♜          ║\n";

	color(lightAqua, BLACK);

	cout << "\t\t╚════════════════════════════════════════════╝\n";

	cout << "\n\n\t\t";

	system("pause");
	Beep(500, 80);

}

char printMainMenu() {
	color(yellow, BLACK);
	loadingScreen("Loading Chess Game.....");
	char choice = 0;
	bool correctInput = true;

	system("cls");
	color(brightWhite, BLACK);
	verticlePadding();
	drawDecoration('.', '=', '.');
	drawDecoration('|', ' ', '|');
	//color(14, 0);
	color(lightYellow, BLACK);
	printCentered("  _____ _    _ ______  _____ _____ ", '|');
	printCentered(" / ____| |  | |  ____|/ ____/ ____|", '|');
	printCentered("| |    | |__| | |__  | (___| (___  ", '|');
	printCentered("| |    |  __  |  __|  \\___ \\\\___ \\ ", '|');
	printCentered("| |____| |  | | |____ ____) |___) |", '|');
	printCentered(" \\_____|_|  |_|______|_____/_____/ ", '|');
	//color(7, 0);
	color(brightWhite, BLACK);
	printCentered(" ", '|');
	color(14, 0);
	printCentered("            G A M E               ", '|');
	color(brightWhite, BLACK);
	printCentered(" ", '|');

	printCentered("C++ Edition", '|');

	printCentered(" ", '|');
	printCentered(" ", '|');
	printCentered(" ", '|');
	color(brightWhite, 0);
	printCentered("   [1] Start New Game ", '|');
	printCentered("   [2] Load Saved Game", '|');
	printCentered("   [3] Instructions   ", '|');
	printCentered("   [4] Credits        ", '|');
	printCentered("   [5] Exit Game      ", '|');
	printCentered(" ", '|');
	printCentered(" ", '|');
	printCentered(" ", '|');
	drawDecoration('.', '=', '.');
	// 𝟮 𝟯 𝟱 𝟲 𝟳 𝟴 𝟵
	// Input Line:
	cout << endl;

	color(lightRed, BLACK);
	cout << "\t╰┈➤ ";
	color(brightWhite, BLACK);

	cout << "Enter Choice (𝟏-𝟱): ";
	while (true) {
		if (_kbhit()) {
			choice = _getch();
			if (choice >= '1' && choice <= '5') {
				cout << choice;
				Beep(500, 80);
				break;
			}
		}
	}
	while (_kbhit()) _getch();   // clear extra typed chars i.e. discard extra keys
	pauseMs(700);
	return choice;
}

 //Call Functions Definition
void callFunctions(const char choice) {
	switch (choice) {
	case '1': {
		system("cls");
		loadingScreen("Starting New Game ⏳⏳⏳");
		system("cls");

		Game g;
		g.startNewGame();
		break;
	}
	case '2': {
		system("cls");
		loadingScreen("Loading Game ⏳⏳⏳");
		system("cls");
		Game g;
		g.loadGame();
		break;
	}
	case '3':
	{
		system("cls");
		loadingScreen("Instructions...");
		displayInstructions();
		break;
	}
	case '4':
	{
		system("cls");
		loadingScreen("Credits...");
		displayCredits();
		break;
	}
	case '5':
	{
		EndingScreen();
	}
	}

	return;
}
