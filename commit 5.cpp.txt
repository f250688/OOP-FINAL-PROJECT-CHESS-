//  HIGH SCORES
//stored scores in the file
string score_file = "chess_scores.txt";
void saveScore(int ws, int bs, string result) {
    ofstream f(score_file, ios::app);
    if (f.is_open()) {
        f << "white: " << ws << " | black: " << bs << " | result: " << result << "\n";
        f << "------------------------\n";
        f.close();
    }
}
//showing scores
void showHighScores() {
    system("cls");
    cout << color::win_clr << color::bold << "=== chess high scores ===\n\n" << color::reset;
    ifstream f(score_file);
    if (!f.is_open()) { cout << "no scores yet.\n"; return; }
    string line;
    bool any = false;
    while (getline(f, line)) { cout << color::info_clr << line << color::reset << "\n"; any = true; }
    if (!any) cout << "no scores yet.\n";
    f.close();
}
//clearing the scores
void clearHighScores() {
    ofstream f(score_file, ios::trunc);
    f.close();
    cout << color::green << "scores cleared!\n" << color::reset;
}

//  MAIN MENU
void showMenu() {
    cout << color::cyan << color::bold
        << "==============================================\n"
        << "           WELCOME TO CHESS GAME            \n"
        << "==============================================\n\n" << color::reset;
    cout << color::win_clr << color::bold
        << "==============================================\n"
        << "              Game Instructions              \n"
        << "==============================================\n" << color::reset;
    cout << color::info_clr
        << "1. move format: enter moves as 'e2 e4' (from to)\n"
        << "2. special moves:\n"
        << "   - promotion: pawn reaches last rank -> choose piece\n"
        << "3. game rules:\n"
        << "   - checkmate: king in check with no escape\n"
        << "   - stalemate: no legal moves, king not in check\n"
        << "4. scoring: pawn=1, knight/bishop=3, rook=5, queen=9\n"
        << "5. type 'menu' during game to return here\n"
        << "6. type 'exit' during game to end current game\n" << color::reset;
    cout << color::bold << color::green << "\nmain menu:\n" << color::reset;
    cout << " 1. start new game\n"
        << "  2. view current scores\n"
        << "  3. view capture history\n"
        << "  4. view high scores\n"
        << "  5. clear high scores\n"
        << "  6. exit\n";
    cout << color::bold << "Enter your choice (1-6): " << color::reset;
}

