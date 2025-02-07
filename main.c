#include "sign_up.c"
#include "login.c"
#include "game_menu.c"
#include "game.c"
#include "setting.c"
#include <ncursesw/ncurses.h>
#include <string.h>
#include <stdio.h>
#include <locale.h>
#include <wchar.h>

#define MAX_LEN 100

int signup_login_menu() {
    setlocale(LC_ALL, "");
    initscr();
    erase();
    start_color();
    init_color(10, 700, 700, 700); 
    init_pair(1, COLOR_BLACK, 10);
    bkgd(COLOR_PAIR(1));
    refresh();
    
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    const wchar_t *options[] = {L"Login", L"Sign Up", L"Login as Guest"};
    int num_options = sizeof(options) / sizeof(options[0]);
    int choice = 0;
    int width = 30, height = num_options * 3; 
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;
    WINDOW *menu_win = newwin(height+3, width, start_y, start_x);
    wbkgd(menu_win, COLOR_PAIR(1));
    wattron(menu_win, COLOR_PAIR(1));
    box(menu_win, 0, 0);
    wattroff(menu_win, COLOR_PAIR(1));

    while (1) {
        werase(menu_win);
        box(menu_win, 0, 0);
        for (int i = 0; i < num_options; i++) {
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

int main() {
    int choice = signup_login_menu();
    if (choice == 0) {
        if(login_page() == -1)
        {
            choice = signup_login_menu();
        }
        else
        {
            game_menu();
        }
    } else  if (choice == 1) {
        sign_up_page();
        main();
    } else if (choice == 2) {
        int a = game_menu();
        if(a == 0)
        {
            int a = game();
            game_menu();
        }
        else if(a == 3)
        {
            if(setting() == 0)
                game_level();
            else
                color_pick();
            main();
        }
        else
        {
            main();
        }
    } else if (choice == 3) {
        return 0;
    }
    return 0;
}
