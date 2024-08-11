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
                if (n < 64) wattron(win, COLOR_PAIR(n));
                mvwprintw(win, CELL_SIZE_ROWS / 2, (CELL_SIZE_COLS - len) / 2, "%s", buf);
                if (n < 64) wattroff(win, COLOR_PAIR(n));
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

    for (int i = 2; i-- && count > 0;) {
        int n = rand() % count;
        grid[open_cells[n][0]][open_cells[n][1]] = (rand() & 1) + 1;

        count--;
        open_cells[n][0] = open_cells[count][0];
        open_cells[n][1] = open_cells[count][1];
    }
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
    initscr();                        /* Start curses mode     */
    raw();                            /* Line buffering disabled  */
    keypad(stdscr, TRUE);             /* We get F1, F2 etc..    */
    noecho();                         /* Don't echo() while we do getch */
    curs_set(0);
    srand(time(NULL));

    start_color();
	init_pair(1,  COLOR_RED, COLOR_BLACK);
	init_pair(2,  COLOR_YELLOW, COLOR_BLACK);
	init_pair(4,  COLOR_GREEN, COLOR_BLACK);
	init_pair(8,  COLOR_BLUE, COLOR_BLACK);
	init_pair(16, COLOR_CYAN, COLOR_BLACK);
	init_pair(32, COLOR_MAGENTA, COLOR_BLACK);

    add_randoms();
    int c;
    while (c != 'q') {
        draw_grid();
        switch (c = getch()) {
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

    // printw("Type any character to see it in bold\n");
    // ch = getch();
    // if(ch == KEY_DOWN) {
    //     mvprintw(0, 0, "Down");
    // }
    // refresh();      /* Print it on to the real screen */
    // getch();      /* Wait for user input */
    endwin();      /* End curses mode      */

    return 0;
}
