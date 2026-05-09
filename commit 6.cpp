//  PLAY GAME
void playGame(Board& b) {
    bool whiteTurn = true;
    string from, to;
    while (true) {
        system("cls");
        b.print();
        b.showCaptures();
        b.showScores();
        // check all game-ending conditions
        //checkmate
        if (isCheckmate(b, whiteTurn)) {
            string winner = whiteTurn ? "black" : "white";
            cout << color::check_clr << color::bold << "*** checkmate! " << winner << " wins! ***\n" << color::reset;
            saveScore(b.whiteScore, b.blackScore, winner + " wins");
            break;
        }
        //stalemate
        if (isStalemate(b, whiteTurn)) {
            cout << color::win_clr << "*** stalemate! draw! ***\n" << color::reset;
            saveScore(b.whiteScore, b.blackScore, "draw - stalemate");
            break;
        }
        // (50-move rule and threefold repetition removed)
        // warn if king is in check
        if (isInCheck(b, whiteTurn))
            cout << color::check_clr << color::bold << "*** check! ***\n" << color::reset;
        // show whose turn it is
        if (whiteTurn) cout << color::white_piece << "white" << color::reset;
        else           cout << color::black_piece << "black" << color::reset;
        cout << "'s move (e.g. e2 e4, or 'menu', 'exit'): ";
        cin >> from;
        if (from == "exit") {
            char c; cout << "save scores? (y/n): "; cin >> c;
            if (c == 'y' || c == 'Y') saveScore(b.whiteScore, b.blackScore, "abandoned");
            break;
        }
        if (from == "menu") {
            char c; cout << "save scores? (y/n): "; cin >> c;
            if (c == 'y' || c == 'Y') saveScore(b.whiteScore, b.blackScore, "incomplete");
            break;
        }
        cin >> to;
        // validate the format (must be like "e2")
        //for moving the pieces
        if (from.length() != 2 || to.length() != 2 ||
            from[0] < 'a' || from[0]>'h' || to[0] < 'a' || to[0]>'h' ||
            from[1] < '1' || from[1]>'8' || to[1] < '1' || to[1]>'8') {
            cout << color::red << "error: use format like 'e2 e4'.\n" << color::reset;
            continue;
        }
        // convert chess notation to array index
        // e.g. "e2" -> column 4, row 6
        int sr = 8 - (from[1] - '0'), sc = from[0] - 'a';
        int dr = 8 - (to[1] - '0'), dc = to[0] - 'a';
        // if a pawn is reaching the last row, ask what to promote to
        char promo = 'q';
        char p = b.grid[sr][sc];
        if (myToLower(p) == 'p' && ((whiteTurn && dr == 0) || (!whiteTurn && dr == 7))) {
            cout << color::win_clr << "promote to (q/r/b/n): " << color::reset;
            cin >> promo;
        }
        // try to make the move
        if (makeMove(b, sr, sc, dr, dc, whiteTurn, promo)) {
            whiteTurn = !whiteTurn; // switch turns
        }
    }
}

//five

//  MAIN
int main() {
    //for colours
    color::enable();
    Board brd;
    int choice;
    do {
        system("cls");
        showMenu();
        if (!(cin >> choice)) {
            cin.clear(); cin.ignore(10000, '\n');
            choice = -1;
        }
        switch (choice) {
        case 1:
            system("cls");
            brd.reset();
            playGame(brd);
            cout << "\npress enter to continue...";
            cin.ignore(); cin.get();
            break;
        case 2:
            system("cls");
            brd.showScores();
            cout << "press enter to continue...";
            cin.ignore(); cin.get();
            break;
        case 3:
            system("cls");
            brd.showCaptures();
            cout << "press enter to continue...";
            cin.ignore(); cin.get();
            break;
        case 4:
            showHighScores();
            cout << "\npress enter to continue...";
            cin.ignore(); cin.get();
            break;
        case 5:
            system("cls");
            clearHighScores();
            cout << "press enter to continue...";
            cin.ignore(); cin.get();
            break;
        case 6:
            cout << color::win_clr << "Thank You for playing!\n" << color::reset;
            break;
        default:
            cout << color::red << "invalid choice.\n" << color::reset;
            cout << "press enter to continue...";
            cin.ignore(); cin.get();
        }
    } while (choice != 6);
    system("pause");
    return 0;
}