
#include <ncursesw/ncurses.h>
#include <string.h>
#include <stdio.h>
#include <locale.h>
#include <wchar.h>

#define MAX_LEN 100

int game_menu() 
{
    erase();
    setlocale(LC_ALL, "");
    start_color();
    init_color(10, 700, 700, 700);
    init_pair(1, COLOR_BLACK, 10);
    bkgd(COLOR_PAIR(1));
    refresh();
    
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    const wchar_t *options[] = {L"New game", L"Load game", L"Score board", L"Setting", L"Log out"};
    int num_options = sizeof(options) / sizeof(options[0]);
    int choice = 0;
    int width = 30, height = num_options * 3; // افزایش فاصله بین گزینه‌ها
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;
    WINDOW *menu_win = newwin(height+3, width, start_y, start_x);
    wbkgd(menu_win, COLOR_PAIR(1));
    wattron(menu_win, COLOR_PAIR(1));
    box(menu_win, 0, 0);
    wattroff(menu_win, COLOR_PAIR(1));

    while (1) 
    {
        werase(menu_win);
        box(menu_win, 0, 0);
        for (int i = 0; i < num_options; i++) 
        {
            if (i == choice)
                mvwprintw(menu_win, i * 3 + 2, (width - wcslen(options[i])) / 2 - 2, "▶ %ls ◀", options[i]);
            else
                mvwaddwstr(menu_win, i * 3 + 2, (width - wcslen(options[i])) / 2, options[i]);
        }
        wrefresh(menu_win);
        
        int input = getch();
        if (input == 'w' || input == KEY_UP || input == 'W')
            choice = (choice - 1 + num_options) % num_options;
        else if (input == 's' || input == KEY_DOWN || input == 'S')
            choice = (choice + 1) % num_options;
        else if (input == '\n') {
            delwin(menu_win);
            endwin();
            return choice;
        }
    }
}