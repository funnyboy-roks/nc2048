#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int grid[4][4] = { 0 };

#define CELL_SIZE_ROWS 7
#define CELL_SIZE_COLS 13

void draw_grid(void)
{
    int height = 4 * (CELL_SIZE_ROWS) - 3;
    int width = 4 * (CELL_SIZE_COLS) - 3;
    int sr = (LINES - height) / 2;
    int sc = (COLS - width) / 2;

    WINDOW *win = newwin(CELL_SIZE_ROWS, CELL_SIZE_COLS, sr, sc);
    wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');

    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            mvwin(win, sr + r * (CELL_SIZE_ROWS - 1), sc + c * (CELL_SIZE_COLS - 1));
            int n = grid[r][c];
            mvwprintw(win, CELL_SIZE_ROWS / 2, 1, "%.*s", CELL_SIZE_COLS - 2, "                   ");
            if (n > 0) {
                char buf[CELL_SIZE_ROWS];
                int len = sprintf(buf, "%d", n);
                wattron(win, COLOR_PAIR(n));
                mvwprintw(win, CELL_SIZE_ROWS / 2, (CELL_SIZE_COLS - len) / 2, "%s", buf);
                wattroff(win, COLOR_PAIR(n));
            }            refresh();
            wrefresh(win);
        }
    }
}

void clear_grid(void)
{
    memset(grid, 0, sizeof(grid));
}

void add_randoms(void)
{
    int open_cells[2][16] = { 0 };
    size_t count = 0;


    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            if (grid[r][c] == 0) {
                open_cells[count  ][0] = r;
                open_cells[count++][1] = c;
            }
        }
    }

    for (int i = (rand() % 2) + 1; i-- && count > 0;) {
        int n = rand() % count;
        grid[open_cells[n][0]][open_cells[n][1]] = 2;

        count--;
        open_cells[n][0] = open_cells[count][0];
        open_cells[n][1] = open_cells[count][1];
    }
}

bool check_valid(void)
{
    for (int r = 4; r-- > 0;) {
        for (int c = 4; c-- > 0;) {
            if (!grid[r][c]) return true;
            if (r >= 1 && grid[r][c] == grid[r - 1][c]) return true;
            if (r < 3 && grid[r][c] == grid[r + 1][c]) return true;
            if (c >= 1 && grid[r][c] == grid[r][c - 1]) return true;
            if (c < 3 && grid[r][c] == grid[r][c + 1]) return true;
        }
    }
    return false;
}

bool check_win(void)
{
    for (int r = 4; r-- > 0;) {
        for (int c = 4; c-- > 0;) {
            if (grid[r][c] >= 64) return true;
        }
    }
    return false;
}

void die(void)
{
    attron(COLOR_PAIR(2));
    char str[100] = " Game Over. Press any key to close ";
    mvprintw(LINES / 2 - 1, COLS / 2 - strlen(str) / 2, "%s", str);
    attroff(COLOR_PAIR(2));
}

void win(void)
{
    attron(COLOR_PAIR(8));
    char str[100] = " You Win. Press any key to close or 'g' to keep going ";
    mvprintw(LINES / 2 - 1, COLS / 2 - strlen(str) / 2, "%s", str);
    attroff(COLOR_PAIR(8));
}

bool compacted[4][4] = { 0 };

void make_move(move)
    int move;
{
    memset(compacted, 0, sizeof(compacted));
    switch (move) {
        case KEY_UP: {
            for (size_t c = 4; c-- > 0;) {
                for (size_t r = 1; r < 4; ++r) {
                    int n = grid[r][c];
                    if (!n) continue;

                    int nonzero = r - 1;
                    for (; nonzero > 0 && !grid[nonzero][c]; nonzero--);

                    grid[r][c] = 0;
                    if (grid[nonzero][c] == n && !compacted[nonzero][c]) {
                        grid[nonzero][c] = n * 2;
                        compacted[nonzero][c] = true;
                    } else if (grid[nonzero][c]) {
                        grid[nonzero + 1][c] = n;
                    } else {
                        grid[nonzero][c] = n;
                    }
                }
            }
        } break;
        case KEY_DOWN: {
            for (size_t c = 4; c-- > 0;) {
                for (size_t r = 3; r-- > 0;) {
                    int n = grid[r][c];
                    if (!n) continue;

                    int nonzero = r + 1;
                    for (; nonzero < 3 && !grid[nonzero][c]; nonzero++);

                    grid[r][c] = 0;
                    if (grid[nonzero][c] == n && !compacted[nonzero][c]) {
                        grid[nonzero][c] = n * 2;
                        compacted[nonzero][c] = true;
                    } else if (grid[nonzero][c]) {
                        grid[nonzero - 1][c] = n;
                    } else {
                        grid[nonzero][c] = n;
                    }
                }
            }
        } break;
        case KEY_LEFT: {
            for (size_t r = 4; r-- > 0;) {
                int *row = grid[r];
                for (size_t c = 1; c < 4; ++c) {
                    int n = row[c];
                    if (!n) continue;

                    int nonzero = c - 1;
                    for (; nonzero > 0 && !row[nonzero]; nonzero--);

                    row[c] = 0;
                    if (row[nonzero] == n && !compacted[r][nonzero]) {
                        row[nonzero] = n * 2;
                        compacted[r][nonzero] = true;
                    } else if (row[nonzero]) {
                        row[nonzero + 1] = n;
                    } else {
                        row[nonzero] = n;
                    }
                }
            }
        } break;
        case KEY_RIGHT: {
            for (size_t r = 4; r-- > 0;) {
                int *row = grid[r];
                for (size_t c = 3; c-- > 0;) {
                    int n = row[c];
                    if (!n) continue;

                    int nonzero = c + 1;
                    for (; nonzero < 3 && !row[nonzero]; nonzero++);

                    row[c] = 0;
                    if (row[nonzero] == n && !compacted[r][nonzero]) {
                        row[nonzero] = n * 2;
                        compacted[r][nonzero] = true;
                    } else if (row[nonzero]) {
                        row[nonzero - 1] = n;
                    } else {
                        row[nonzero] = n;
                    }
                }
            }
        } break;

    }
}

int main(void)
{
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);
    srand(time(NULL));

    start_color();
	init_pair(2,  COLOR_RED, COLOR_BLACK);
	init_pair(4,  COLOR_YELLOW, COLOR_BLACK);
	init_pair(8,  COLOR_GREEN, COLOR_BLACK);
	init_pair(16,  COLOR_BLUE, COLOR_BLACK);
	init_pair(32, COLOR_CYAN, COLOR_BLACK);
	init_pair(64, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(128, COLOR_RED, COLOR_WHITE);
	init_pair(256, COLOR_YELLOW, COLOR_WHITE);
	init_pair(512, COLOR_GREEN, COLOR_WHITE);
	init_pair(1024, COLOR_BLUE, COLOR_WHITE);
	init_pair(2048, COLOR_CYAN, COLOR_WHITE);

    add_randoms();
    int c;
    bool dead = false;
    bool won = false;
    bool cont = false;
    while (c != 'q') {
        clear();
        draw_grid();
        if (!check_valid()) {
            die();
            dead = true;
        }
        if (check_win() && !won) {
            win();
            won = true;
        }

        c = getch();

        if (dead) break;
        if (won && !cont) {
             if (c != 'g') {
                 break;
             }
             cont = true;
        }
        switch (c) {
            case 'c': clear_grid(); break;
            case KEY_UP:
            case KEY_DOWN:
            case KEY_LEFT:
            case KEY_RIGHT:
                make_move(c);
            case 'r':
                add_randoms();
                break;
        }
    }

    endwin();

    return 0;
}
