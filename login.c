#include <ncursesw/ncurses.h>
#include <string.h>
#include <stdio.h>
#include <locale.h>
#include <wchar.h>

#define MAX_LEN 100

int login(const char* username, const char* password)
{
    FILE *file = fopen("users.txt", "r");
    if (file == NULL) 
        return -1;
    char line[MAX_LEN + 2];
    char stored_username[MAX_LEN + 2];
    char stored_password[MAX_LEN + 2];

    while (fgets(line, sizeof(line), file) != NULL) 
    {    
        if (sscanf(line, "Username: %[^,], Password: %[^,],", stored_username, stored_password) == 2) 
        {
            if (strcmp(username, stored_username) == 0) 
            {
                fclose(file);
                if(strcmp(password, stored_password) == 0)
                {
                    return 1; 
                }
                return 0;
            }
        }
    }
    return -1;
}

void show_message(const wchar_t* message) {
    int width = 30, height = 7;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;
    
    WINDOW *msg_win = newwin(height, width, start_y, start_x);
    box(msg_win, 0, 0);
    mvwaddwstr(msg_win, 2, (width - wcslen(message)) / 2, message);
    mvwaddwstr(msg_win, 4, (width - 8) / 2, L"[ Back ]");
    wrefresh(msg_win);
    
    int ch;
    while ((ch = getch()) != '\n');
    
    delwin(msg_win);
    refresh();
}

int login_page() {
    setlocale(LC_ALL, "");
    initscr();
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    bkgd(COLOR_PAIR(1));
    refresh();
    
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(1);

    char username[MAX_LEN] = {0};
    char password[MAX_LEN] = {0};
    int current_field = 0;
    int username_index = 0;
    int password_index = 0;
    int ch;
    int login_status = -2;
    int width = 40, height = 12;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;

    while (1) {
        erase();
        
        mvaddwstr(start_y + 1, start_x + (width - 10) / 2, L"Login Page");
        mvaddwstr(start_y + 3, start_x + 2, L"Username:");
        mvaddwstr(start_y + 5, start_x + 2, L"Password:");
        mvaddwstr(start_y + 7, start_x + (width - 10) / 2, L"[ Login ]");
        mvaddwstr(start_y + 9, start_x + (width - 8) / 2, L"[ Back ]");

        mvaddwstr(start_y + 3, start_x + 12, L"[");
        mvprintw(start_y + 3, start_x + 13, "%s", username);
        mvaddwstr(start_y + 3, start_x + 13 + username_index, L"]");

        mvaddwstr(start_y + 5, start_x + 12, L"[");
        for (int i = 0; i < password_index; i++)
            mvaddwstr(start_y + 5, start_x + 13 + i, L"*");
        mvaddwstr(start_y + 5, start_x + 13 + password_index, L"]");

        if (current_field == 2) {
            attron(A_REVERSE);
            mvaddwstr(start_y + 7, start_x + (width - 10) / 2, L"[ Login ]");
            attroff(A_REVERSE);
        }
        if (current_field == 3) {
            attron(A_REVERSE);
            mvaddwstr(start_y + 9, start_x + (width - 8) / 2, L"[ Back ]");
            attroff(A_REVERSE);
        }

        move(start_y + (current_field == 0 ? 3 : 5), start_x + 13 + (current_field == 0 ? username_index : password_index));
        refresh();
        ch = getch();
        switch (ch) 
        {
            case KEY_UP: 
                current_field = (current_field - 1 + 4) % 4;
                break;
            case KEY_DOWN:
                current_field = (current_field + 1) % 4;
                break;
            case '\n':
                if (current_field == 2) {
                    login_status = login(username, password);
                    if (login_status == -1)
                        show_message(L"Username not found!");
                    else if (login_status == 0)
                        show_message(L"Password is incorrect");
                    else if (login_status == 1)
                        show_message(L"Login successful!");
                } else if (current_field == 3) {
                    return -1;
                } else if (current_field == 0)
                    current_field = 1;
                break;
            default:
                if (current_field == 0 && username_index < MAX_LEN - 1) {
                    if (ch == KEY_BACKSPACE || ch == 127) {
                        if (username_index > 0) 
                            username[--username_index] = '\0';
                    } else {
                        username[username_index++] = ch;
                        username[username_index] = '\0';
                    }
                } else if (current_field == 1 && password_index < MAX_LEN - 1) {
                    if (ch == KEY_BACKSPACE || ch == 127) {
                        if (password_index > 0) 
                            password[--password_index] = '\0';
                    } else {
                        password[password_index++] = ch;
                        password[password_index] = '\0';
                    }
                }
                break;
        }
    }
    return 0;
}
