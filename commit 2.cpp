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