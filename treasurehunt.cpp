#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

const int ROWS = 5;
const int COLS = 5;

// Hidden board values:
// 0 = empty, 1 = trap, 2 = treasure
// Visible board values: '*' hidden, '-' empty, 'X' trap, 'T' treasure

struct Move {
    int number;
    string who;   // "Player" or "Computer"
    int row;
    int col;
    string result; // "Empty", "Trap", "Treasure"
};

void printHeader(const string& name, const string& euid, const string& email) {
    cout << "+-------------------------------------------------+\n";
    cout << "|          Computer Science and Engineering       |\n";
    cout << "|         " << name << " " << email << "       |\n";
    cout << "+-------------------------------------------------+\n\n";
}

void initializeBoard(int hidden[ROWS][COLS], char visible[ROWS][COLS]) {
    // Set all hidden to 0 and visible to '*'
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            hidden[r][c] = 0;
            visible[r][c] = '*';
        }
    }

    // Place exactly 3 traps (value = 1) in unique cells
    int trapsPlaced = 0;
    while (trapsPlaced < 3) {
        int r = rand() % ROWS;
        int c = rand() % COLS;
        if (hidden[r][c] == 0) {
            hidden[r][c] = 1;
            trapsPlaced++;
        }
    }

    // Place exactly 1 treasure (value = 2) in a cell that is not a trap
    bool treasurePlaced = false;
    while (!treasurePlaced) {
        int r = rand() % ROWS;
        int c = rand() % COLS;
        if (hidden[r][c] == 0) {
            hidden[r][c] = 2;
            treasurePlaced = true;
        }
    }
}

void displayBoard(const char visible[ROWS][COLS]) {
    cout << "   ";
    for (int c = 0; c < COLS; ++c) {
        cout << c << " ";
    }
    cout << "\n";

    for (int r = 0; r < ROWS; ++r) {
        cout << r << "  ";
        for (int c = 0; c < COLS; ++c) {
            cout << visible[r][c] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}

void showHiddenBoard(const int hidden[ROWS][COLS]) {
    cout << "   ";
    for (int c = 0; c < COLS; ++c) {
        cout << c << " ";
    }
    cout << "\n";
    for (int r = 0; r < ROWS; ++r) {
        cout << r << "  ";
        for (int c = 0; c < COLS; ++c) {
            cout << hidden[r][c] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
    cout << "Legend:\n";
    cout << "2 = Treasure\n1 = Trap\n0 = Empty\n* = Hidden/unrevealed\n";
}

bool allCellsRevealed(const char visible[ROWS][COLS]) {
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            if (visible[r][c] == '*') return false;
        }
    }
    return true;
}

void addHistory(vector<Move>& history, int num, const string& who, int row, int col, const string& result) {
    Move m;
    m.number = num;
    m.who = who;
    m.row = row;
    m.col = col;
    m.result = result;
    history.push_back(m);
}

void showHistory(const vector<Move>& history) {
    cout << "Move History:\n";
    cout << "-------------------------------------\n";
    cout << "No. | Player     | Row | Col | Result\n";
    cout << "-------------------------------------\n";
    for (size_t i = 0; i < history.size(); ++i) {
        cout << setw(3) << history[i].number << " | "
             << left << setw(10) << history[i].who << right << " | "
             << setw(3) << history[i].row << " | "
             << setw(3) << history[i].col << " | "
             << history[i].result << "\n";
    }
    cout << "-------------------------------------\n\n";
}

bool playerMove(int hidden[ROWS][COLS], char visible[ROWS][COLS],
                int& playerScore, vector<Move>& history, int& moveNum) {
    cout << "Your Turn:\n\n";
    displayBoard(visible);

    int row, col;
    while (true) {
        cout << "Enter row (0-4): ";
        if (!(cin >> row)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Please enter a number 0-4.\n";
            continue;
        }
        cout << "Enter column (0-4): ";
        if (!(cin >> col)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Please enter a number 0-4.\n";
            continue;
        }

        if (row < 0 || row >= ROWS || col < 0 || col >= COLS) {
            cout << "\nInvalid move!\n\n";
            continue;
        }
        if (visible[row][col] != '*') {
            cout << "\nAlready chosen!\n\n";
            continue;
        }
        break; // valid
    }

    string result;
    if (hidden[row][col] == 0) {
        visible[row][col] = '-';
        result = "Empty";
        cout << "\nEmpty.\n";
        cout << "Your score: " << playerScore << "\n\n";
    } else if (hidden[row][col] == 1) {
        visible[row][col] = 'X';
        playerScore -= 3;
        result = "Trap";
        cout << "\nTrap! (-3)\n";
        cout << "Your score: " << playerScore << "\n\n";
    } else { // treasure
        visible[row][col] = 'T';
        playerScore += 10;
        result = "Treasure";
        cout << "\nTreasure! (+10)\n";
        cout << "Your score: " << playerScore << "\n\n";
        addHistory(history, moveNum++, "Player", row, col, result);
        return true; // treasure found
    }

    addHistory(history, moveNum++, "Player", row, col, result);
    return false;
}

bool computerMove(int hidden[ROWS][COLS], char visible[ROWS][COLS],
                  int& compScore, vector<Move>& history, int& moveNum) {
    cout << "Computer's Turn:\n";

    // Choose a random unvisited '*' cell
    int row, col;
    int attempts = 0;
    do {
        row = rand() % ROWS;
        col = rand() % COLS;
        attempts++;
        // Just in case board is nearly full, cap attempts and scan
        if (attempts > 200) {
            bool found = false;
            for (int r = 0; r < ROWS && !found; ++r) {
                for (int c = 0; c < COLS && !found; ++c) {
                    if (visible[r][c] == '*') {
                        row = r; col = c; found = true;
                    }
                }
            }
            break;
        }
    } while (visible[row][col] != '*');

    cout << "Computer chose (" << row << ", " << col << ")\n\n";

    string result;
    if (hidden[row][col] == 0) {
        visible[row][col] = '-';
        result = "Empty";
        cout << "Empty.\n";
        cout << "Computer score: " << compScore << "\n\n";
    } else if (hidden[row][col] == 1) {
        visible[row][col] = 'X';
        compScore -= 3;
        result = "Trap";
        cout << "Trap! (-3)\n";
        cout << "Computer score: " << compScore << "\n\n";
    } else { // treasure
        visible[row][col] = 'T';
        compScore += 10;
        result = "Treasure";
        cout << "Treasure! (+10)\n";
        cout << "Computer score: " << compScore << "\n\n";
        addHistory(history, moveNum++, "Computer", row, col, result);
        return true; // treasure found
    }

    addHistory(history, moveNum++, "Computer", row, col, result);
    return false;
}

int main() {
    srand(static_cast<unsigned int>(time(0)));

    // Replace with your actual info
    string studentName = "Student Name";
    string euid = "EUID";
    string email = "euid@my.unt.edu";

    printHeader(studentName, euid, email);

    cout << "TREASURE HUNT: You vs Computer\n\n";
    cout << "Treasure = +10 | Trap = -3 | Empty = 0\n";
    cout << "Both play on the same board. Highest score wins!\n\n";

    int hidden[ROWS][COLS];
    char visible[ROWS][COLS];
    int playerScore = 0;
    int computerScore = 0;
    vector<Move> history;
    int moveNumber = 1;

    initializeBoard(hidden, visible);

    bool gameOver = false;
    int turn = 1;

    while (!gameOver) {
        cout << "========== TURN " << turn << " ==========\n\n";

        // Player turn
        bool playerFoundTreasure = playerMove(hidden, visible, playerScore, history, moveNumber);
        if (playerFoundTreasure) {
            gameOver = true;
        } else if (allCellsRevealed(visible)) {
            gameOver = true;
        }

        if (gameOver) break;

        // Computer turn
        bool compFoundTreasure = computerMove(hidden, visible, computerScore, history, moveNumber);
        if (compFoundTreasure) {
            gameOver = true;
        } else if (allCellsRevealed(visible)) {
            gameOver = true;
        }

        turn++;
    }

    // Game over
    cout << "========== GAME OVER ==========\n\n";
    displayBoard(visible);

    cout << "Final Scores:\n";
    cout << "You: " << playerScore << "\n";
    cout << "Computer: " << computerScore << "\n";

    if (playerScore > computerScore) {
        cout << "You win!\n\n";
    } else if (playerScore < computerScore) {
        cout << "Computer wins!\n\n";
    } else {
        cout << "It's a tie!\n\n";
    }

    showHistory(history);

    cout << "Hidden Board (for reference):\n";
    showHiddenBoard(hidden);

    return 0;
}
