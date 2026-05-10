#include<iostream>
#include<string>
#include<fstream>
#ifdef _WIN32
#include<windows.h>
#endif
using namespace std;

// replacing cctype and cmath with our own simple functions
// replaces isupper()
bool myIsUpper(char c) { return c >= 'A' && c <= 'Z'; }
// replaces islower()
bool myIsLower(char c) { return c >= 'a' && c <= 'z'; }
// replaces tolower()
char myToLower(char c) { if (c >= 'A' && c <= 'Z') return c + 32; return c; }
// replaces toupper()
char myToUpper(char c) { if (c >= 'a' && c <= 'z') return c - 32; return c; }
// replaces abs()
int myAbs(int n) { if (n < 0) return -n; return n; }

//  COLORS
namespace color {
    const string reset = "\033[0m";
    const string bold = "\033[1m";
    const string red = "\033[31m";
    const string green = "\033[32m";
    const string cyan = "\033[36m";
    const string white_piece = "\033[1;97m";
    const string black_piece = "\033[1;90m";
    const string check_clr = "\033[1;31m";
    const string win_clr = "\033[1;33m";
    const string capture_clr = "\033[1;35m";
    const string info_clr = "\033[36m";
    const string bg_dark = "\033[48;5;240m";
    const string bg_light = "\033[48;5;250m";
    void enable() {
#ifdef _WIN32
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode = 0;
        GetConsoleMode(h, &mode);
        SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
    }
}

// forward declaration so Piece can use Board
class Board;

// ============================================================
//  BASE CLASS - Piece
//  All chess pieces inherit from this class (OOP: Inheritance)
//  isValidMove is a virtual function (OOP: Polymorphism)
// ============================================================
class Piece {
public:
    bool isWhitePiece; // true = white, false = black
    char symbol;       // character representing this piece

    Piece(bool white, char sym) {
        isWhitePiece = white;
        symbol = sym;
    }

    virtual ~Piece() {}

    // virtual function - each piece overrides this with its own movement rules
    // (OOP: Polymorphism + virtual functions)
    virtual bool isValidMove(Board& b, int sr, int sc, int dr, int dc) {
        return false;
    }
};

// ============================================================
//  BOARD CLASS
//  Contains Piece objects (OOP: Composition)
//  The board is an 8x8 grid of Piece pointers
//  Uppercase letters = white pieces, lowercase = black pieces
//  A dot '.' means the square is empty
//  P = white pawn      p = black pawn
//  R = white rook      r = black rook
//  N = white knight    n = black knight
//  B = white bishop    b = black bishop
//  Q = white queen     q = black queen
//  K = white king      k = black king
// ============================================================
class Board {
public:
    char grid[8][8];   // the 8x8 board
    bool moved[8][8];  // true if a piece at this square has moved before
    int  epRow, epCol; // the square where en passant capture is possible (unused)
    int  halfClock;    // (unused after removing 50-move rule)
    int  whiteScore, blackScore;
    string whiteCaps, blackCaps; // pieces each player has captured
    Board()
    {
        reset();
    }
    // put all pieces back to starting positions
    void reset() {
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 8; j++) {
                grid[i][j] = '.';    // empty
                moved[i][j] = false; // nothing has moved yet
            }
        // black back row
        grid[0][0] = 'r'; grid[0][1] = 'n'; grid[0][2] = 'b'; grid[0][3] = 'q';
        grid[0][4] = 'k'; grid[0][5] = 'b'; grid[0][6] = 'n'; grid[0][7] = 'r';
        for (int j = 0; j < 8; j++) grid[1][j] = 'p'; // black pawns
        // white back row
        grid[7][0] = 'R'; grid[7][1] = 'N'; grid[7][2] = 'B'; grid[7][3] = 'Q';
        grid[7][4] = 'K'; grid[7][5] = 'B'; grid[7][6] = 'N'; grid[7][7] = 'R';
        for (int j = 0; j < 8; j++) grid[6][j] = 'P'; // white pawns
        epRow = -1; epCol = -1; halfClock = 0;
        whiteScore = 0; blackScore = 0;
        whiteCaps = ""; blackCaps = "";
    }
    // print the board on screen
    void print() {
        cout << "\n";
        cout << color::cyan << color::bold
            << "      a   b   c   d   e   f   g   h\n"
            << "    +---+---+---+---+---+---+---+---+\n" << color::reset;
        for (int i = 0; i < 8; i++) {
            cout << color::cyan << color::bold << "  " << 8 - i << " " << color::reset;
            cout << color::cyan << "|" << color::reset;
            for (int j = 0; j < 8; j++) {
                //symbols on each square
                char sym = grid[i][j];
                // alternate light and dark squares like a real chessboard
                string bg = ((i + j) % 2 == 0) ? color::bg_light : color::bg_dark;
                string fg = (sym == '.') ? "" : (myIsUpper(sym) ? color::white_piece : color::black_piece);
                char  show = (sym == '.') ? ' ' : sym;
                cout << bg << fg << " " << show << " " << color::reset;
                cout << color::cyan << "|" << color::reset;
            }
            cout << color::cyan << color::bold << " " << 8 - i << color::reset << "\n";
            cout << color::cyan << "    +---+---+---+---+---+---+---+---+\n" << color::reset;
        }
        cout << color::cyan << color::bold
            << "      a   b   c   d   e   f   g   h\n\n" << color::reset;
    }
    void showScores() {
        cout << "\n" << color::bold << color::win_clr << "=== score board ===\n" << color::reset;
        cout << color::white_piece << "white: " << whiteScore << " pts\n" << color::reset;
        cout << color::black_piece << "black: " << blackScore << " pts\n" << color::reset;
        cout << color::win_clr << "==================\n\n" << color::reset;
    }
    void showCaptures() {
        cout << "\n" << color::capture_clr << color::bold << "=== capture history ===\n" << color::reset;
        cout << color::white_piece << "white captured: " << color::reset << (whiteCaps.empty() ? "none" : whiteCaps) << "\n";
        cout << color::black_piece << "black captured: " << color::reset << (blackCaps.empty() ? "none" : blackCaps) << "\n";
        cout << color::capture_clr << "=======================\n" << color::reset;
    }
};

//one


//  SIMPLE HELPER FUNCTIONS
// white pieces are uppercase letters
bool isWhite(char c) { return myIsUpper(c) && c != '.'; }
// black pieces are lowercase letters
bool isBlack(char c) { return myIsLower(c) && c != '.'; }
// a square is empty if it has a dot
bool isEmpty(Board& b, int r, int c) { return b.grid[r][c] == '.'; }
// check if row and column are inside the board (0 to 7)
bool inBounds(int r, int c) { return r >= 0 && r < 8 && c >= 0 && c < 8; }
// returns the point value of a piece
int pieceValue(char p) {
    char low = myToLower(p);
    if (low == 'p') return 1;
    if (low == 'n') return 3;
    if (low == 'b') return 3;
    if (low == 'r') return 5;
    if (low == 'q') return 9;
    return 0;
}
// checks that no piece is blocking the path between two squares
// used by rook, bishop and queen
bool pathClear(Board& b, int sr, int sc, int dr, int dc) {
    // find which direction to step (either -1, 0, or +1)
    //if steprow == 1 then down
    //if steprow == -1 then up
    //if stepcol == 1 then right
    //if stepcol == -1 then left
    int stepRow = (dr == sr) ? 0 : (dr > sr ? 1 : -1);
    int stepCol = (dc == sc) ? 0 : (dc > sc ? 1 : -1);
    // start one step ahead of source square
    int r = sr + stepRow;
    int c = sc + stepCol;
    // keep walking until we reach the destination
    while (r != dr || c != dc) {
        if (!isEmpty(b, r, c)) return false; // something is blocking
        r += stepRow;
        c += stepCol;
    }
    return true; // nothing blocking
}
// finds the row and column of the king
// stores result in the row and col variables passed in
void findKing(Board& b, bool white, int& row, int& col) {
    char king = white ? 'K' : 'k';
    row = -1; col = -1;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (b.grid[i][j] == king) { row = i; col = j; }
}

// ============================================================
//  DERIVED CLASSES - each piece inherits from Piece
//  Each one overrides isValidMove() with its own rules
//  (OOP: Inheritance + Polymorphism)
// ============================================================

// --- PAWN (derived from Piece) ---
class Pawn : public Piece {
public:
    Pawn(bool white) : Piece(white, white ? 'P' : 'p') {}

    bool isValidMove(Board& board, int sourceRow, int sourceCol, int destRow, int destCol) override {
        // White moves upward (row decreases), Black moves downward (row increases)
        int direction = isWhitePiece ? -1 : 1;
        int startingRank = isWhitePiece ? 6 : 1;   // Pawns start on row 6 (white) or row 1 (black)
        int rowChange = destRow - sourceRow;
        int colChangeAbs = myAbs(destCol - sourceCol);
        // 1) Move one square straight forward
        if (colChangeAbs == 0 && rowChange == direction && isEmpty(board, destRow, destCol))
            return true;
        // 2) Move two squares forward (only from starting rank, with empty path)
        if (colChangeAbs == 0 && rowChange == 2 * direction && sourceRow == startingRank &&
            isEmpty(board, sourceRow + direction, sourceCol) && isEmpty(board, destRow, destCol))
            return true;
        // 3) Diagonal capture - an enemy piece must be there
        if (colChangeAbs == 1 && rowChange == direction && !isEmpty(board, destRow, destCol) &&
            (isWhitePiece ? isBlack(board.grid[destRow][destCol]) : isWhite(board.grid[destRow][destCol])))
            return true;
        // (en passant removed)
        return false;
    }
};

// --- ROOK (derived from Piece) ---
class Rook : public Piece {
public:
    Rook(bool white) : Piece(white, white ? 'R' : 'r') {}

    bool isValidMove(Board& b, int sr, int sc, int dr, int dc) override {
        // rook can only move in straight lines (same row OR same column)
        if (sr != dr && sc != dc) return false;
        // no piece must be in the way
        return pathClear(b, sr, sc, dr, dc);
    }
};

// --- KNIGHT (derived from Piece) ---
class Knight : public Piece {
public:
    Knight(bool white) : Piece(white, white ? 'N' : 'n') {}

    bool isValidMove(Board& b, int sr, int sc, int dr, int dc) override {
        // knight moves in an L-shape: 2 squares in one direction, 1 in the other
        int dRow = myAbs(dr - sr);
        int dCol = myAbs(dc - sc);
        return (dRow == 2 && dCol == 1) || (dRow == 1 && dCol == 2);
    }
};

// --- BISHOP (derived from Piece) ---
class Bishop : public Piece {
public:
    Bishop(bool white) : Piece(white, white ? 'B' : 'b') {}

    bool isValidMove(Board& b, int sr, int sc, int dr, int dc) override {
        // bishop can only move diagonally (row change must equal column change)
        if (myAbs(dr - sr) != myAbs(dc - sc)) return false;
        return pathClear(b, sr, sc, dr, dc);
    }
};

// --- QUEEN (derived from Piece) ---
class Queen : public Piece {
public:
    Queen(bool white) : Piece(white, white ? 'Q' : 'q') {}

    bool isValidMove(Board& b, int sr, int sc, int dr, int dc) override {
        // queen combines rook + bishop (straight lines OR diagonals)
        bool straight = (sr == dr || sc == dc);            // same row or column = straight
        bool diagonal = (myAbs(dr - sr) == myAbs(dc - sc)); // equal row+col change = diagonal
        if (!straight && !diagonal) return false;
        return pathClear(b, sr, sc, dr, dc);
    }
};

// forward declaration: kingCanMove needs isAttacked, and isAttacked needs to exist first
bool isAttacked(Board& b, int r, int c, bool byWhite);

// --- KING (derived from Piece) ---
// Castling removed
class King : public Piece {
public:
    King(bool white) : Piece(white, white ? 'K' : 'k') {}

    bool isValidMove(Board& b, int sr, int sc, int dr, int dc) override {
        // normal move: king moves 1 square in any direction
        if (myAbs(dr - sr) <= 1 && myAbs(dc - sc) <= 1) return true;
        // castling removed
        return false;
    }
};



//two




// ============================================================
//  IS A SQUARE BEING ATTACKED?
//  Checks if any enemy piece can reach square (r,c)
// ============================================================
bool isAttacked(Board& b, int r, int c, bool byWhite) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            char attack = b.grid[i][j];
            if (attack == '.') continue;
            // skip pieces that are not the attacking side
            //bywhite is the function that shows who is the attacker side
            //bywhite -> true (white is attacker)
            if (byWhite && !isWhite(attack)) continue;
            if (!byWhite && !isBlack(attack)) continue;
            char low = myToLower(attack);
            // pawn only attacks diagonally (NOT straight forward)
            if (low == 'p') {
                int dir = byWhite ? -1 : 1;
                if (myAbs(c - j) == 1 && r == i + dir) return true;
            }
            // use piece objects to check attack (polymorphism)
            else if (low == 'r') { Rook   tmp(byWhite); if (tmp.isValidMove(b, i, j, r, c)) return true; }
            else if (low == 'n') { Knight tmp(byWhite); if (tmp.isValidMove(b, i, j, r, c)) return true; }
            else if (low == 'b') { Bishop tmp(byWhite); if (tmp.isValidMove(b, i, j, r, c)) return true; }
            else if (low == 'q') { Queen  tmp(byWhite); if (tmp.isValidMove(b, i, j, r, c)) return true; }
            else if (low == 'k' && myAbs(r - i) <= 1 && myAbs(c - j) <= 1) return true;
        }
    }
    return false;
}

//  CHECK, CHECKMATE, STALEMATE
// is the given side's king currently under attack?
bool isInCheck(Board& b, bool white) {
    int kr, kc;
    findKing(b, white, kr, kc);
    if (kr == -1) return false;           // king not found (should not happen)
    return isAttacked(b, kr, kc, !white); // is king's square attacked by enemy?
}
// tries a move on a COPY of the board, checks if king is still safe
// we do this before applying any real move
bool moveSafe(Board& b, int sr, int sc, int dr, int dc, bool white) {
    Board copy = b; // copy the entire board (works because Board only has arrays)
    char low = myToLower(copy.grid[sr][sc]);
    // en passant and castling handling removed
    // make the move on the copy
    copy.grid[dr][dc] = copy.grid[sr][sc];
    copy.grid[sr][sc] = '.';
    // the move is safe only if king is NOT in check after it
    return !isInCheck(copy, white);
}
// checks if the given side has any legal move available
bool hasAnyMove(Board& b, bool white) {
    for (int sr = 0; sr < 8; sr++) {
        for (int sc = 0; sc < 8; sc++) {
            char p = b.grid[sr][sc];
            if (p == '.' || isWhite(p) != white) continue; // skip empty or wrong color
            // try every possible destination square
            for (int dr = 0; dr < 8; dr++) {
                for (int dc = 0; dc < 8; dc++) {
                    if (sr == dr && sc == dc) continue; // can't stay in same place
                    char low = myToLower(p);
                    bool valid = false;
                    // create piece object and call isValidMove (polymorphism)
                    if (low == 'p') { Pawn   tmp(white); valid = tmp.isValidMove(b, sr, sc, dr, dc); }
                    if (low == 'r') { Rook   tmp(white); valid = tmp.isValidMove(b, sr, sc, dr, dc); }
                    if (low == 'n') { Knight tmp(white); valid = tmp.isValidMove(b, sr, sc, dr, dc); }
                    if (low == 'b') { Bishop tmp(white); valid = tmp.isValidMove(b, sr, sc, dr, dc); }
                    if (low == 'q') { Queen  tmp(white); valid = tmp.isValidMove(b, sr, sc, dr, dc); }
                    if (low == 'k') { King   tmp(white); valid = tmp.isValidMove(b, sr, sc, dr, dc); }
                    // also: can't capture your own piece
                    if (b.grid[dr][dc] != '.' && isWhite(b.grid[dr][dc]) == white)
                        valid = false;
                    if (valid && moveSafe(b, sr, sc, dr, dc, white))
                        return true; // found at least one legal move
                }
            }
        }
    }
    return false; // no legal move found
}

//three



// checkmate = king is in check AND no legal moves exist
bool isCheckmate(Board& b, bool white) {
    return isInCheck(b, white) && !hasAnyMove(b, white);
}
// stalemate = king is NOT in check BUT still no legal moves exist
bool isStalemate(Board& b, bool white) {
    return !isInCheck(b, white) && !hasAnyMove(b, white);
}
// ============================================================
//  MAKE A MOVE
//  Validates and applies a move to the board
//  Returns true if move was successful, false if not allowed
// ============================================================
bool makeMove(Board& b, int sr, int sc, int dr, int dc, bool whiteTurn, char promo = 'q') {
    char p = b.grid[sr][sc];
    // --- basic error checks ---
    if (!inBounds(sr, sc) || !inBounds(dr, dc)) {
        cout << color::red << "error: out of bounds.\n" << color::reset;
        return false;
    }
    if (p == '.') {
        cout << color::red << "error: no piece there.\n" << color::reset;
        return false;
    }
    if (isWhite(p) != whiteTurn) {
        cout << color::red << "error: not your piece.\n" << color::reset;
        return false;
    }
    // can't capture your own piece
    if (b.grid[dr][dc] != '.' && isWhite(b.grid[dr][dc]) == whiteTurn) {
        cout << color::red << "error: can't capture your own piece.\n" << color::reset;
        return false;
    }
    // --- check if the piece can physically make this move ---
    // create the correct piece object and call isValidMove (polymorphism)
    char low = myToLower(p);
    bool valid = false;
    if (low == 'p') { Pawn   tmp(whiteTurn); valid = tmp.isValidMove(b, sr, sc, dr, dc); }
    if (low == 'r') { Rook   tmp(whiteTurn); valid = tmp.isValidMove(b, sr, sc, dr, dc); }
    if (low == 'n') { Knight tmp(whiteTurn); valid = tmp.isValidMove(b, sr, sc, dr, dc); }
    if (low == 'b') { Bishop tmp(whiteTurn); valid = tmp.isValidMove(b, sr, sc, dr, dc); }
    if (low == 'q') { Queen  tmp(whiteTurn); valid = tmp.isValidMove(b, sr, sc, dr, dc); }
    if (low == 'k') { King   tmp(whiteTurn); valid = tmp.isValidMove(b, sr, sc, dr, dc); }
    if (!valid) {
        cout << color::red << "error: invalid move for that piece.\n" << color::reset;
        return false;
    }
    // --- check that this move doesn't leave our king in check ---
    if (!moveSafe(b, sr, sc, dr, dc, whiteTurn)) {
        cout << color::check_clr << "error: that leaves your king in check!\n" << color::reset;
        return false;
    }
    char captured = b.grid[dr][dc];
    // --- en passant and castling handled removed ---

    // --- handle normal capture ---
    if (captured != '.') {
        if (whiteTurn) { b.whiteScore += pieceValue(captured); b.whiteCaps += captured; }
        else { b.blackScore += pieceValue(captured); b.blackCaps += captured; }
        cout << color::capture_clr << "*** " << (whiteTurn ? "white" : "black")
            << " captured " << captured << "! ***\n" << color::reset;
    }
    // --- move the piece ---
    b.grid[dr][dc] = p;
    b.grid[sr][sc] = '.';
    b.moved[dr][dc] = true;
    // --- handle pawn promotion (pawn reaches the other end) ---
    bool isPromo = (low == 'p' && ((whiteTurn && dr == 0) || (!whiteTurn && dr == 7)));
    if (isPromo) {
        char pc = myToLower(promo);
        if (pc != 'q' && pc != 'r' && pc != 'b' && pc != 'n') pc = 'q'; // default to queen
        b.grid[dr][dc] = whiteTurn ? myToUpper(pc) : pc;
        cout << color::win_clr << "*** promoted to " << pc << "! ***\n" << color::reset;
    }
    // --- update en passant square (unused) ---
    b.epRow = -1; b.epCol = -1;
    // (50-move rule completely removed - no halfClock update)
    cout << color::green << "moved: "
        << char('a' + sc) << (8 - sr) << " -> "
        << char('a' + dc) << (8 - dr) << "\n" << color::reset;
    return true;
}


//four



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

//five




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






//six