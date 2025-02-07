#include <ncurses.h>
#include <string.h>
#include <stdio.h>
#include <regex.h>
#include <stdlib.h>
#include <time.h>
#define MAX_LEN 100
#define PASSWORD_LENGTH 12

// تابعی برای تولید رمز عبور تصادفی
void generate_random_password(char* password, int length) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{}|;:,.<>?/"; 
    int charset_size = sizeof(charset) - 1; // اندازه‌ی رشته charset

    // اطمینان از تولید اعداد تصادفی متفاوت
    srand(time(NULL));

    // تولید رمز عبور
    for (int i = 0; i < length; i++) {
        int random_index = rand() % charset_size; // انتخاب یک ایندکس تصادفی از charset
        password[i] = charset[random_index]; // اضافه کردن کاراکتر به رمز عبور
    }
    password[length] = '\0'; // پایان دادن به رشته
}

int is_valid_email(const char* email) 
{
    regex_t regex;
    const char *pattern = "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$";
    int result = !regcomp(&regex, pattern, REG_EXTENDED) && !regexec(&regex, email, 0, NULL, 0);
    regfree(&regex);
    return result;
}

int is_this_user_exist(const char* username) 
{
    FILE *file = fopen("users.txt", "r");
    if (file == NULL) 
        return 0;
    char line[MAX_LEN + 2];
    char stored_username[MAX_LEN + 2];

    while (fgets(line, sizeof(line), file) != NULL) 
    {    
        if (sscanf(line, "Username: %s,", stored_username) == 1) 
        {
            stored_username[strlen(stored_username)-1] = '\0';
            if (strcmp(username, stored_username) == 0) 
            {
                fclose(file);
                return 1; 
            }
        }
    }

    fclose(file);
    return 0; 
}

int is_password_strong(const char* password)
{
    int number = 0;
    int upper_case = 0;
    int lower_case = 0;
    int long_enogh = 0;
    if(strlen(password) > 6)
        long_enogh = 1;
    for(int i=0; i<strlen(password); i++)
    {
        if('A' <= password[i] && password[i] <= 'Z')
            upper_case = 1;
        if('a' <= password[i] && password[i] <= 'z')
            lower_case = 1;
        if('0' <= password[i] && password[i] <= '9')
            number = 1;
    }
    if((number + upper_case + lower_case + long_enogh) == 4)
        return 1;
    else
        return 0;
}

void save_user(const char* username, const char* password, const char* email) 
{
    FILE *file = fopen("users.txt", "a");
    if (file != NULL) {
        fprintf(file, "Username: %s, Password: %s, email: %s, High_score: %d.\n", username, password, email, 0);
        fclose(file);
    }
}

void sign_up_page() 
{
    clear();
    cbreak();                 // Disable line buffering
    noecho();                 // Disable echoing of characters
    keypad(stdscr, TRUE);     // Enable special keys like arrow keys
    curs_set(1);              // Show the cursor

    // تغییر رنگ پس‌زمینه و رنگ نوشته
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);  // متن مشکی و پس‌زمینه سفید
    bkgd(COLOR_PAIR(1));  // تغییر پس‌زمینه به سفید
    refresh();

    char username[MAX_LEN + 2] = {0};
    char password[MAX_LEN + 2] = {0};
    char email[MAX_LEN + 2] = {0};
    char confirm_password[MAX_LEN + 2] = {0};
    int index = 0;
    int ch;

    int width = 40, height = 14;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;

    // رسم کادر مشکی دور فرم
    for (int i = start_y; i < start_y + height; i++) {
        mvhline(i, start_x, ACS_HLINE, width);  // خطوط افقی داخل فرم
    }
    for (int i = start_x; i < start_x + width; i++) {
        mvvline(start_y, i, ACS_VLINE, height);  // خطوط عمودی داخل فرم
    }
    mvaddch(start_y, start_x, ACS_ULCORNER);  // گوشه بالا چپ
    mvaddch(start_y, start_x + width - 1, ACS_URCORNER);  // گوشه بالا راست
    mvaddch(start_y + height - 1, start_x, ACS_LLCORNER);  // گوشه پایین چپ
    mvaddch(start_y + height - 1, start_x + width - 1, ACS_LRCORNER);  // گوشه پایین راست

    while(1) 
    {
        erase();
        
        // نمایش عنوان صفحه و کادر دور فرم
        mvprintw(start_y + 1, start_x + (width - 10) / 2, "Sign Up");

        // وارد کردن یوزرنیم با حاشیه [ ]
        mvprintw(start_y + 3, start_x + 2, "Enter username: ");
        mvprintw(start_y + 4, start_x + 2, "[%s]", username);
        move(start_y + 4, start_x + 3 + strlen(username));
        refresh();

        index = 0;
        while ((ch = getch()) != '\n' && index < MAX_LEN - 1) 
        {
            if (ch == KEY_BACKSPACE || ch == 127) 
            {
                if (index > 0) {
                    index--;
                    username[index] = '\0'; // آخرین کاراکتر را حذف کن
                    move(start_y + 4, start_x + 2); // مکان‌نمای خط یوزرنیم
                    clrtoeol(); // پاک کردن خط فعلی
                    mvprintw(start_y + 4, start_x + 2, "[%s]", username); // نمایش مجدد یوزرنیم
                    refresh();
                }
            } 
            else if (ch >= 32 && ch <= 126) 
            {
                username[index++] = ch;
                username[index] = '\0'; 
                mvprintw(start_y + 4, start_x + 2, "[%s]", username);
                refresh();
            }
        }
        username[index] = '\0';

        if (is_this_user_exist(username) == 1) 
        {
            mvprintw(start_y + 6, start_x + 2, "This username already exists! Try again.");
            refresh();
        } 
        else 
        {
            break; 
        }
    }

    clear();
    index = 0;
    while(1) 
    {
        mvprintw(start_y + 2, start_x + 1, "Enter email: ");
        mvprintw(start_y + 4, start_x + 2, "[%s]", email);
        move(start_y + 4, start_x + 3 + strlen(email));
        refresh();
        while ((ch = getch()) != '\n' && index < MAX_LEN - 1) 
        {
            if (ch == KEY_BACKSPACE || ch == 127) 
            {
                if (index > 0) {
                    index--;
                    email[index] = '\0';
                    move(start_y + 4, start_x + 2); 
                    clrtoeol(); 
                    mvprintw(start_y + 4, start_x + 2, "[%s]", email);  
                    refresh();
                }
            } 
            else if (ch >= 32 && ch <= 126) 
            {
                email[index++] = ch;
                email[index] = '\0'; 
                mvprintw(start_y + 4, start_x + 2, "[%s]", email);
                refresh();
            }
        }
        email[index] = '\0';

        if (is_valid_email(email) == 0) 
        {
            mvprintw(start_y + 5, start_x + 2, "This email is not valid! Try again.");
            refresh();
        } 
        else 
        {
            break;
        }
    }

    clear();
    index = 0;
    while(1) 
    {
        mvprintw(start_y + 2, start_x, "Enter password: ");
        mvprintw(start_y + 4, start_x + 2, "[%s]", password);
        move(start_y + 4, start_x + 3 + strlen(password));
        refresh();

        // نمایش دکمه "Generate Password"
        int generate_password_button_y = start_y + 7;
        int generate_password_button_x = start_x + (width - 16) / 2;


        ch = getch();
        int as = 0;


        while ((ch != '\n') || as == 1) {
            if(ch == '\n')
            {
                char random_password[PASSWORD_LENGTH + 1] = {0};
                do{
                generate_random_password(random_password, PASSWORD_LENGTH);
                strcpy(password, random_password);
                }while(!is_password_strong(password));
                index = strlen(password);
                move(start_y + 4, start_x + 2);
                    clrtoeol();
                    mvprintw(start_y + 4, start_x + 2, "[%s]", password);
                    refresh();
            }
            if(ch == KEY_UP || ch == KEY_DOWN)
            {
                as+=1;
                as%=2;
            }
            if(as == 1)
            {
                attron(A_REVERSE);
                mvprintw(generate_password_button_y, generate_password_button_x, "Generate Password");
                attroff(A_REVERSE);
            }
            else
            {
                mvprintw(generate_password_button_y, generate_password_button_x, "Generate Password");
            }
            if (ch == KEY_BACKSPACE || ch == 127) {
                if (index > 0) {
                    index--;
                    password[index] = '\0';
                    move(start_y + 4, start_x + 2);
                    clrtoeol();
                    mvprintw(start_y + 4, start_x + 2, "[%s]", password);
                    refresh();
                }
            } else if (ch >= 32 && ch <= 126) {
                password[index++] = ch;
                password[index] = '\0';
                mvprintw(start_y + 4, start_x + 2, "[%s]", password);
                refresh();
            }
            ch = getch();
        }
        password[index] = '\0';

        if (is_password_strong(password) == 0) {
            mvprintw(start_y + 6, start_x + 2, "This password is not strong! Try again.");
            refresh();
        } else {
            break;
        }
    }

    clear();
    save_user(username, password, email);
    mvprintw(start_y + 3, start_x + (width - 20) / 2, "User registered successfully!");
    refresh();
    getch();
    clear();
}
