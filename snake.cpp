#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <cstdlib>

#define WIDTH 40
#define HEIGHT 20

// speeds... slow, fast, super fast
#define EASY_SPEED 250000
#define MEDIUM_SPEED 150000
#define HARD_SPEED 50000

using namespace std;

bool fatherless;
int x, y, frX, frY, score;
int tailX[100], tailY[100];
int nTail;
string tailed;
enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
eDirection dir;

int speed;

void inits() {
    fatherless = false;
    dir = STOP;
    x = WIDTH / 2;
    y = HEIGHT / 2;
    frX = rand() % WIDTH;
    frY = rand() % HEIGHT;
    score = 0;
    nTail = 0;
    tailed = "\033[101m\033[30m \033[100m\033[30m";
    
}

void disp_snek() {
    // ansi escape to set cursor on the top instead of clearing the whole shits
    cout << "\033[H";
    cout << "\033[100m\033[30m";
    for (int i = 0; i < WIDTH + 2; i++)
        if (i == 0) {
            cout << "╔";
        } else if (i == (WIDTH + 1)) {
            cout << "╗";
        } else {
            cout << "═";
        }

    cout << endl;
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (j == 0)
                cout << "║";
            if (i == y && j == x)
                cout << tailed;
            else if (i == frY && j == frX)
                // cout << "•";
                cout << "\033[102m\033[30m \033[100m\033[30m";
            else {
                bool print = false;
                for (int k = 0; k < nTail; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        //cout << "•";
                        cout << "\033[102m\033[30m \033[100m\033[30m";
                        print = true;
                    }
                }
                if (!print)
                    cout << " ";
            }
            if (j == WIDTH - 1)
                cout << "║";
        }
        cout << endl;
    }
    for (int i = 0; i < WIDTH + 2; i++)
        if (i == 0) {
            cout << "╚";
        } else if (i == (WIDTH + 1)) {
            cout << "╝"; 
        } else {
            cout << "═";
        }
    cout << endl;
    cout << "\033[0m";
    cout << "Score: " << score << endl;
}

void chng_dir_snek(char key) {
	switch (key) {
    case 'a':
        if (dir != RIGHT) dir = LEFT;
        break;
    case 'd':
        if (dir != LEFT) dir = RIGHT;
        break;
    case 'w':
        if (dir != DOWN) dir = UP;
        break;
    case 's':
        if (dir != UP) dir = DOWN;
        break;
    case 'x':
        fatherless = true;
        break;
    }
}

void shenanigans() {
    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;
    tailX[0] = x;
    tailY[0] = y;
    for (int i = 1; i < nTail; i++) {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }
    switch (dir) {
    case LEFT:
        x--;
        break;
    case RIGHT:
        x++;
        break;
    case UP:
        y--;
        break;
    case DOWN:
        y++;
        break;
    default:
        break;
    }
    if (x >= WIDTH) x = 0;
    else if (x < 0) x = WIDTH - 1;
    if (y >= HEIGHT) y = 0;
    else if (y < 0) y = HEIGHT - 1;
    for (int i = 0; i < nTail; i++)
        if (tailX[i] == x && tailY[i] == y)
            fatherless = true;
    if (x == frX && y == frY) {
        score += 10;
        frX = rand() % WIDTH;
        frY = rand() % HEIGHT;
        nTail++;
    }
}

// this shit controls terminal modes
void rw_termOn() {
    termios term;
    tcgetattr(0, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSANOW, &term);
}

void rw_termOff() {
    termios term;
    tcgetattr(0, &term);
    term.c_lflag |= (ICANON | ECHO);
    tcsetattr(0, TCSANOW, &term);
}

// using my own version of kbhit, since conio is unused here
bool kbhit() {
    termios term;
    tcgetattr(0, &term);

    termios term2 = term;
    term2.c_lflag &= ~ICANON;
    tcsetattr(0, TCSANOW, &term2);

    int bytesWaiting;
    ioctl(0, FIONREAD, &bytesWaiting);

    tcsetattr(0, TCSANOW, &term);
    return bytesWaiting > 0;
}

int main() {
	// hides the cursor first.
	cout << "\033[?25l";
	// initialize the screen
	system("clear");
    rw_termOff();
    
    int difficulty;
    // adds difficulty based on speed
    cout << "Select difficulty level: 1 (Easy), 2 (Medium), 3 (Hard): ";
    cin >> difficulty;
    switch (difficulty) {
        case 1: speed = EASY_SPEED; break;
        case 2: speed = MEDIUM_SPEED; break;
        case 3: speed = HARD_SPEED; break;
        default: speed = MEDIUM_SPEED; break;
    }
    system("clear");
    rw_termOn();
    inits();
    while (!fatherless) {
        disp_snek();
        usleep(speed);

        if (kbhit()) {
            char key;
            read(STDIN_FILENO, &key, 1);
            chng_dir_snek(key);
        }
        if (dir != STOP) {
            shenanigans();
        }
    }
    // show the cursor
    cout << "\033[?25h";
    rw_termOff();
    cout << "GAME OVER!!" << endl;
    return 0;
}
