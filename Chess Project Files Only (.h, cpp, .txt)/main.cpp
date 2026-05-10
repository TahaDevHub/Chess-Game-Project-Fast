//#include "SFML_GUI.h"
//#include "Game.h"
//#include "taha_functions.h"
//
//int main()
//{
//    Board board;
//    sfml_GUI gui;
//
//    gui.run(board); // This starts the SFML window
//
//    if (gui.getState() == GUIState::CONSOLE_MODE) {
//        // Start your existing Game::startNewGame() console logic here
//    }
//
//    //SetConsoleOutputCP(CP_UTF8);
//
//    //char choice;
//    //do {
//    //    choice = printMainMenu();
//    //    callFunctions(choice);
//    //} while (choice != '5');
//
//    return 0;
//}

#include "SFML_GUI.h"
#include "Game.h"
#include"taha_functions.h"

int main()
{
    SetConsoleOutputCP(CP_UTF8);

    sfml_GUI gui;
    Board board;                    // GUI reads this
    bool useGUI = gui.run(board);   // false = user chose console

    if (!useGUI)
    {
        // Your existing console menu loop
        char choice;
        do {
            choice = printMainMenu();
            callFunctions(choice);
        } while (choice != '5');
    }
    return 0;
}