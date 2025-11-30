#include <iostream>

using namespace std;

void clearScreen();
void placeMines(int** board, int rows, int cols, int mineCount);
void calculateNumbers(int** board, int rows, int cols);
void printBoard(int** board, int** shown, int rows, int cols, int flags, int minesCount, bool revealAll = false);
void revealZeros(int** board, int** shown, int sr, int sc, int rows, int cols);
bool revealCell(int** board, int** shown, int r, int c, int rows, int cols);
bool checkWin(int** board, int** shown, int rows, int cols, int mineCount);

int main() {
    srand((unsigned)time(nullptr));

    int rows, cols, mineCount;
    do {
        cout << "=== MINESWEEPER ===\n";
        cout << "Rows (10 <= x <= 20): "; cin >> rows;
        cout << "Columns (10 <= x <= 20): "; cin >> cols;
        cout << "Mines: "; cin >> mineCount;

        if (rows < 10 || rows > 20 || cols < 10 || cols > 20) {
            cout << "Incorrect numbers!\n";
            system("pause");
            continue;
        }
        if (mineCount <= 0 || mineCount >= rows * cols) {
            cout << "Incorrect mine count!\n";
            system("pause");
            continue;
        }

        int R = rows + 2;
        int C = cols + 2;
        int** board = new int* [R];
        int** shown = new int* [R];
        for (int i = 0; i < R; i++) {
            board[i] = new int[C];
            shown[i] = new int[C];
            for (int j = 0; j < C; j++) {
                board[i][j] = 0;
                shown[i][j] = 0;
            }
        }

        placeMines(board, rows, cols, mineCount);
        calculateNumbers(board, rows, cols);

        bool alive = true;
        int flags = 0;

        while (true) {
            clearScreen();
            printBoard(board, shown, rows, cols, flags, mineCount);

            if (!alive) {
                cout << "\nYou explode!\n";
                printBoard(board, shown, rows, cols, flags, mineCount, true);
                break;
            }

            if (checkWin(board, shown, rows, cols, mineCount)) {
                cout << "\nYou win!\n";
                printBoard(board, shown, rows, cols, flags, mineCount, true);
                break;
            }

            cout << "\nCommands:\n";
            cout << "r R C - open\n";
            cout << "f R C - place/remove flag\n";
            cout << "q - quit\n\n";

            cout << "Enter command: ";
            char cmd;
            cin >> cmd;

            if (cmd == 'q') break;

            int r, c;
            cin >> r >> c;
            if (r < 1 || r > rows || c < 1 || c > cols) continue;

            int ir = r;
            int ic = c;

            switch (cmd) {
            case 'f': {
                if (shown[ir][ic] == 1) continue;
                if (shown[ir][ic] == 2) {
                    shown[ir][ic] = 0;
                    flags--;
                }
                else {
                    shown[ir][ic] = 2;
                    flags++;
                }
                break;
            }
            case 'r': {
                if (shown[ir][ic] != 2) {
                    bool ok = revealCell(board, shown, ir, ic, rows, cols);
                    if (!ok) alive = false;
                }
                break;
            }
            default: continue;
            }
        }

        for (int i = 0; i < R; i++) {
            delete[] board[i];
            delete[] shown[i];
        }
        delete[] board;
        delete[] shown;

        system("pause");
        int ans;
        cout << "Another game? (0 - No; 1 - Yes): ";
        cin >> ans;
        if (ans != 1) break;
        clearScreen();
    } while (true);

    return 0;
}

void placeMines(int** board, int rows, int cols, int mineCount) {
    int placed = 0;
    while (placed < mineCount) {
        int x = rand() % rows + 1;
        int y = rand() % cols + 1;
        if (board[x][y] != -1) {
            board[x][y] = -1;
            placed++;
        }
    }
}

void calculateNumbers(int** board, int rows, int cols) {
    for (int r = 1; r <= rows; r++) {
        for (int c = 1; c <= cols; c++) {
            if (board[r][c] == -1) continue;
            int cnt = 0;
            for (int dr = -1; dr <= 1; dr++) {
                for (int dc = -1; dc <= 1; dc++) {
                    if (dr == 0 && dc == 0) continue;
                    if (board[r + dr][c + dc] == -1) cnt++;
                }
            }
            board[r][c] = cnt;
        }
    }
}

void printBoard(int** board, int** shown, int rows, int cols, int flags, int minesCount, bool revealAll) {
    cout << "\nFlags: " << flags << " / " << minesCount << "\n\n";

    cout << "    ";
    for (int c = 1; c <= cols; c++) {
        if (c < 10) cout << c << "  ";
        else cout << c << " ";
    }
    cout << "\n\n";

    for (int r = 1; r <= rows; r++) {
        if (r < 10) cout << " " << r << " ";
        else cout << r << " ";

        for (int c = 1; c <= cols; c++) {
            if (revealAll) {
                if (board[r][c] == -1) cout << " * ";
                else if (board[r][c] == 0) cout << " . ";
                else cout << " " << board[r][c] << " ";
                continue;
            }

            if (shown[r][c] == 2) {
                cout << " F ";
                continue;
            }
            if (shown[r][c] == 0) {
                cout << " # ";
                continue;
            }
            if (board[r][c] == -1) cout << " * ";
            else if (board[r][c] == 0) cout << " . ";
            else cout << " " << board[r][c] << " ";
        }
        cout << "\n";
    }
}

void revealZeros(int** board, int** shown, int sr, int sc, int rows, int cols) {
    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) continue;
            int nr = sr + dr;
            int nc = sc + dc;
            if (nr < 1 || nr > rows || nc < 1 || nc > cols) continue;
            if (shown[nr][nc] == 1) continue;
            if (shown[nr][nc] == 2) continue;
            revealCell(board, shown, nr, nc, rows, cols);
        }
    }
}

bool revealCell(int** board, int** shown, int r, int c, int rows, int cols) {
    if (r < 1 || r > rows || c < 1 || c > cols) return true;

    if (shown[r][c] == 1) return true;
    if (shown[r][c] == 2) return true;

    if (board[r][c] == -1) {
        shown[r][c] = 1;
        return false;
    }

    shown[r][c] = 1;

    if (board[r][c] == 0) {
        revealZeros(board, shown, r, c, rows, cols);
    }

    return true;
}

bool checkWin(int** board, int** shown, int rows, int cols, int mineCount) {
    int revealed = 0;
    for (int r = 1; r <= rows; r++) {
        for (int c = 1; c <= cols; c++) {
            if (shown[r][c] == 1) revealed++;
        }
    }
    return revealed == (rows * cols - mineCount);
}

void clearScreen() {
    system("cls");
}