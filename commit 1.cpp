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