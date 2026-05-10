#include "Game.h"
#include "taha_functions.h"
#include "save_and_load_Game.h"

Game::Game()
{
    //isBlackUndoUsed = false;
    //isWhiteUndoUsed = false;
    turn = "White";
}


void Game::startNewGame()
{

    int moves = 1;

    ofstream fout;
    fout.open("savedGame.txt");
    //bool current = true;
    bool isExited = false;
    int prev_sc = -1, prev_sr = -1, prev_dc = -1, prev_dr = -1;
    int n = 0;
    capturedByWhite = "Captured By White: ";
    capturedByBlack = "Captured By Black: ";
    // Player Names:
    verticlePadding();
    string whiteName, blackName;
    color(lightRed, BLACK);
    horizontalPadding();
    cout << "╰┈➤ Enter White Name: ";
    color(lightYellow, BLACK);
    getline(cin, whiteName);
    horizontalPadding();
    color(lightRed, BLACK);
    cout << "╰┈➤ Enter Black Name: ";
    color(lightYellow, BLACK);

    getline(cin, blackName);
    fout << whiteName << endl << blackName << endl;
    //bool isGameOver = false;
    turnName = whiteName;
    while (true)
    {
        system("cls");

        //n++;
        //moves++;
        

        if (board.isCheckmate(turn)&& turn=="Black") {
            //SetConsoleOutputCP(CP_UTF8);
            moves--;
            board.display(moves, prev_sc, prev_sr, prev_dc, prev_dr, capturedByWhite, capturedByBlack);
            cout << "\n\n\n";
            color(lightBlue, BLACK);
            drawDecoration('*', '-', '*');
            printCentered("  _____ _    _ ______ _____ _  __ __  __       _______ ______ ", '|');
            printCentered("| |    | |__| | |__ | |    | ' / | \\  / |  /  \\  | |  | |__   ", '|');
            printCentered("| |    |  __  |  __|| |    |  <  | |\\/| | / /\\ \\ | |  |  __|  ", '|');
            printCentered("| |____| |  | | |___| |____| . \\ | |  | |/ ____ \\| |  | |____ ", '|');
            printCentered(" \\_____|_|  |_|______\\_____|_|\\_\\|_|  |_/_/    \\_\\_|  |______|", '|');
            printCentered(" ", '|');
            color(brightWhite, BLACK);
            printCentered("By  " +whiteName+" (White)", '|');
            color(lightBlue, BLACK);
            printCentered(" ", '|');
            drawDecoration('*', '-', '*');
            cout << "\n\n";
            cout << "\t";
            color(black, AQUA);
            Beep(4500, 1000);

            system("pause");
            Beep(500, 80);
            break;
        }
        if (board.isCheckmate(turn) && turn == "White") {
            //SetConsoleOutputCP(CP_UTF8);
            moves--;
            board.display(moves, prev_sc, prev_sr, prev_dc, prev_dr, capturedByWhite, capturedByBlack);
            cout << "\n\n\n";
            color(lightBlue, BLACK);
            drawDecoration('*', '-', '*');
            printCentered("  _____ _    _ ______ _____ _  __ __  __       _______ ______ ", '|');
            printCentered(" / ____| |  | |  ____/ ____| |/ /|  \\/  |   /\\|__   __|  ____|", '|');
            printCentered("| |    | |__| | |__ | |    | ' / | \\  / |  /  \\  | |  | |__   ", '|');
            printCentered("| |    |  __  |  __|| |    |  <  | |\\/| | / /\\ \\ | |  |  __|  ", '|');
            printCentered("| |____| |  | | |___| |____| . \\ | |  | |/ ____ \\| |  | |____ ", '|');
            printCentered(" \\_____|_|  |_|______\\_____|_|\\_\\|_|  |_/_/    \\_\\_|  |______|", '|');
            printCentered(" ", '|');
            color(brightWhite, BLACK);
            printCentered("By  " + blackName + " (Black)", '|');
            color(lightBlue, BLACK);
            printCentered(" ", '|');
            drawDecoration('*', '-', '*');



            cout << "\n\n";
            cout << "\t";
            color(black, AQUA);
            Beep(4500, 1000);

            system("pause");
            Beep(500, 80);

            break;
        }
        board.display(moves, prev_sc, prev_sr, prev_dc, prev_dr, capturedByWhite, capturedByBlack);
        if (turn == "Black")
            moves++;
        color(aqua, BLACK);
        cout << "\n";
        horizontalPadding();
        color(lightYellow, BLACK);
        cout << "▶   ";
        
        color(aqua, BLACK);

        cout << turn << " ";
        color(lightRed, BLACK);
        cout << turnName<<"'s ";
        color(aqua, BLACK);
        cout<< "Turn\n";

        horizontalPadding();
        color(lightYellow, BLACK);

        cout << "⟥-- ";
        color(lightAqua, BLACK);
        cout << "Enter move(e2 e4) Or Exit: ";
        color(lightYellow, BLACK);
        string temp;
        string from="  ", to="  ";
        getline(cin, temp);
        
        for (int i = 0;i < temp.length();i++) {
            temp[i] = tolower(temp[i]);
        }
        if (temp == "exit") {
            isExited = true;
            Beep(1000, 80);

            color(brightWhite, BLACK);
            loadingScreen("SAVING GAME");
            break;
        }
        if (temp.length() < 5)  // to avoid "string subscript out of range" error which occurs when i.e. we are accessing the element of string which is out of range of string length
        {
            if (turn == "Black") {
                moves--;
            }
            horizontalPadding();

            color(red, BLACK);    // Red Text with black bg
            cout << "Invalid Move!\n";
            Beep(1400, 200);

            horizontalPadding();
            system("pause");
            color(brightWhite, BLACK); // bright white text with black bg
            continue;
        }
        //if (temp == "undo"&&isPrevMovedSuccessfull) {

        //}
        //cin >> from >> to;
        from[0] = tolower(temp[0]);
        from[1] = temp[1];

        to[0] = tolower(temp[3]);
        to[1] = temp[4];
        int sc = from[0] - 'a';
        int sr = 8 - (from[1] - '0');

        int dc = to[0] - 'a';
        int dr = 8 - (to[1] - '0');
        //prev_sc = sc;
        //prev_sr = sr;
        //prev_dc = dc;
        //prev_dr = dr;
        
        if (board.movePiece(sr, sc, dr, dc, turn,capturedByWhite,capturedByBlack))
        {

            prev_sc = sc;
            prev_sr = sr;
            prev_dc = dc;
            prev_dr = dr;
            
            color(brightWhite, BLACK);
            turn = (turn == "White") ? "Black" : "White";
            turnName = (turnName == whiteName) ? blackName : whiteName;
            saveMove(fout,sr, sc, dr, dc, turn);
            Beep(15000, 125);

            //isPrevMovedSuccessfull = true;

        }
        else
        {
            horizontalPadding();

            color(red, BLACK);    // Red Text with black bg
            cout << "Invalid Move!\n";
            Beep(1400, 200);
            horizontalPadding();
            system("pause");
            color(brightWhite, BLACK); // bright white text with black bg
        }

    }
    fout.close();
    if (!isExited) {
        ofstream fout2;
        fout2.open("savedGame.txt");
        fout2.close();
    }
}

// Load previous Game

void Game::loadGame()
{
    int moves = 1;
    int prev_sc = -1, prev_sr = -1, prev_dc = -1, prev_dr = -1;
    ifstream fin;
    fin.open("savedGame.txt");
    int sr, sc, dr, dc;

    turn = "White";
    capturedByWhite = "Captured By White: ";
    capturedByBlack = "Captured By Black: ";
    
    // Player Names:
    string whiteName, blackName;
    getline(fin, whiteName);
    getline(fin, blackName);
    turnName = whiteName;
    // loading previous game
    bool isLoadedSuccssfully = false;   // to avoid playing new game in load game i.e. we return early if game not loaded
    while (fin >> sr >> sc >> dr >> dc)
    {
        //cin >> sc >> dr >> dc;
        //moves++;

        isLoadedSuccssfully = true;

        prev_sc = sc;
        prev_sr = sr;
        prev_dc = dc;
        prev_dr = dr;
        board.display(moves, prev_sc, prev_sr, prev_dc, prev_dr, capturedByWhite, capturedByBlack);
        if (turn == "Black") moves++;
        //color(3, 0);
        cout << "\n";
        horizontalPadding();
        color(red, BLACK);
        cout << "▶   ";
        color(aqua, BLACK);

        cout << turn << " Turn\n";

        horizontalPadding();
        color(lightYellow, BLACK);

        cout << "⟥-- ";
        color(lightAqua, BLACK);
        cout << "Enter move(e2 e4) or Exit: ";
        color(lightYellow, BLACK);
        // ==
        string from, to;
        from = to = "  ";
        char temp = sc + 'a';
        from[0] = temp;
        temp = sr + 8 - '0';
        from[1] = temp;

        temp = dc + 'a';
        to[0] = temp;
        temp = dr + 8 - '0';
        to[1] = temp;

        // ==
        cout << from << " " << to << endl;
        board.movePiece(sr, sc, dr, dc, turn, capturedByWhite, capturedByBlack);

        color(brightWhite, BLACK);
        turn = (turn == "White") ? "Black" : "White";
        turnName = (turnName == whiteName) ? blackName : whiteName;
        system("cls");
    }
    fin.close();

    if (!isLoadedSuccssfully) {
        verticlePadding();
        color(lightRed, BLACK);
        drawDecoration('*', '=', '*');

        printCentered("", '|');
        printCentered("", '|');
        printCentered("", '|');
        color(gray, BLACK);
        printCentered("NO GAME SAVED YET", '|');
        //color(brightWhite, BLACK);
        color(lightRed, BLACK);

        printCentered("", '|');
        printCentered("", '|');
        printCentered("", '|');
        drawDecoration('*', '=', '*');
        cout << "\n\n";
        cout << "\t";
        color(black, AQUA);

        system("pause");
        Beep(500, 80);

        return; // this is the early return if no game saved;
    }

    // now continue playing existing (loaded) game and also saving (appending) savedGame.txt

    ofstream fout;
    fout.open("savedGame.txt", ios::app);
    //if (turn == "Black") moves--;
    //int prev_sc = 0, prev_sr = 0, prev_dc = 0, prev_dr = 0;
    while (true)
    {
        system("cls");

        //verticlePadding();
        //cout << "Move # " << moves << endl;
        if (board.isCheckmate(turn) && turn == "Black") {
            SetConsoleOutputCP(CP_UTF8);
            moves--;
            board.display(moves, prev_sc, prev_sr, prev_dc, prev_dr, capturedByWhite, capturedByBlack);
            cout << "\n\n\n";
            color(lightBlue, BLACK);
            drawDecoration('*', '-', '*');
            printCentered("  _____ _    _ ______ _____ _  __ __  __       _______ ______ ", '|');
            printCentered(" / ____| |  | |  ____/ ____| |/ /|  \\/  |   /\\|__   __|  ____|", '|');
            printCentered("| |    | |__| | |__ | |    | ' / | \\  / |  /  \\  | |  | |__   ", '|');
            printCentered("| |    |  __  |  __|| |    |  <  | |\\/| | / /\\ \\ | |  |  __|  ", '|');
            printCentered("| |____| |  | | |___| |____| . \\ | |  | |/ ____ \\| |  | |____ ", '|');
            printCentered(" \\_____|_|  |_|______\\_____|_|\\_\\|_|  |_/_/    \\_\\_|  |______|", '|');
            printCentered(" ", '|');
            color(brightWhite, BLACK);
            printCentered("By  " + whiteName + " (White)", '|');

            color(lightBlue, BLACK);
            printCentered(" ", '|');
            drawDecoration('*', '-', '*');



            cout << "\n\n";
            cout << "\t";
            color(black, AQUA);
            Beep(4500, 1000);

            system("pause");
            Beep(500, 80);

            break;
        }
        if (board.isCheckmate(turn) && turn == "White") {
            SetConsoleOutputCP(CP_UTF8);
            moves--;
            board.display(moves, prev_sc, prev_sr, prev_dc, prev_dr, capturedByWhite, capturedByBlack);
            cout << "\n\n\n";
            color(lightBlue, BLACK);
            drawDecoration('*', '-', '*');
            printCentered("  _____ _    _ ______ _____ _  __ __  __       _______ ______ ", '|');
            printCentered(" / ____| |  | |  ____/ ____| |/ /|  \\/  |   /\\|__   __|  ____|", '|');
            printCentered("| |    | |__| | |__ | |    | ' / | \\  / |  /  \\  | |  | |__   ", '|');
            printCentered("| |    |  __  |  __|| |    |  <  | |\\/| | / /\\ \\ | |  |  __|  ", '|');
            printCentered("| |____| |  | | |___| |____| . \\ | |  | |/ ____ \\| |  | |____ ", '|');
            printCentered(" \\_____|_|  |_|______\\_____|_|\\_\\|_|  |_/_/    \\_\\_|  |______|", '|');
            printCentered(" ", '|');
            color(brightWhite, BLACK);
            printCentered("By  " + blackName + " (Black)", '|');

            color(lightBlue, BLACK);
            printCentered(" ", '|');
            drawDecoration('*', '-', '*');



            cout << "\n\n";
            cout << "\t";
            color(black, AQUA);
            Beep(4500, 1000);

            system("pause");
            Beep(500, 80);

            break;
        }
        board.display(moves, prev_sc, prev_sr, prev_dc, prev_dr, capturedByWhite, capturedByBlack);
        //moves++;
        if (turn == "Black") moves++;
        color(aqua, BLACK);
        cout << "\n";
        horizontalPadding();
        color(lightYellow, BLACK);
        cout << "▶   ";

        color(aqua, BLACK);

        cout << turn << " ";
        color(lightRed, BLACK);
        cout << turnName << "'s ";
        color(aqua, BLACK);
        cout << "Turn\n";

        horizontalPadding();
        color(lightYellow, BLACK);

        cout << "⟥-- ";
        color(lightAqua, BLACK);
        cout << "Enter move(e2 e4) or Exit: ";
        color(lightYellow, BLACK);
        string temp;
        string from = "  ", to = "  ";
        getline(cin, temp);
        
        for (int i = 0;i < temp.length();i++) {
            temp[i] = tolower(temp[i]);
        }
        if (temp == "exit") {
            Beep(1000, 80);
            color(brightWhite, BLACK);
            loadingScreen("SAVING GAME");
            break;
        }
        if (temp.length() < 5)  // to avoid "string subscript out of range" error which occurs when i.e. we are accessing the element of string which is out of range of string length
        {
            horizontalPadding();

            color(red, BLACK);    // Red Text with black bg
            cout << "Invalid Move!\n";
            Beep(1400, 200);

            horizontalPadding();
            system("pause");
            color(brightWhite, BLACK); // bright white text with black bg
            continue;
        }
        //cin >> from >> to;
        from[0] = tolower(temp[0]);
        from[1] = temp[1];

        to[0] = tolower(temp[3]);
        to[1] = temp[4];
        int sc = from[0] - 'a';
        int sr = 8 - (from[1] - '0');

        int dc = to[0] - 'a';
        int dr = 8 - (to[1] - '0');
        //prev_sc = sc;
        //prev_sr = sr;
        //prev_dc = dc;
        //prev_dr = dr;
        if (board.movePiece(sr, sc, dr, dc, turn, capturedByWhite, capturedByBlack))
        {
            prev_sc = sc;
            prev_sr = sr;
            prev_dc = dc;
            prev_dr = dr;
            color(brightWhite, BLACK);
            turn = (turn == "White") ? "Black" : "White";
            saveMove(fout, sr, sc, dr, dc, turn);
            Beep(15000, 125);

        }
        else
        {
            horizontalPadding();

            color(red, BLACK);    // Red Text with black bg
            cout << "Invalid Move!\n";
            Beep(1400, 200);

            horizontalPadding();
            system("pause");
            color(brightWhite, BLACK); // bright white text with black bg
        }

        system("cls");
    }
}
