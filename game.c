#include <stdio.h>
#include <ncursesw/ncurses.h>
#include <time.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>
#include "game_win.c"

#define MAP_ROWS 38
#define MAP_COLUMNS 144
#define MAX_ROOMS 12
#define RANDOM(min, max) ((rand() % ((max) - (min) + 1)) + (min))

typedef enum
{
    regular,
    enchant,
    treasure,
    nightmare
} room_type;

typedef enum
{
    N,
    NE,
    E,
    SE,
    S,
    SW,
    W,
    NW
} direction;

typedef struct
{
    int x, y;
} Stares_plase;

typedef struct
{
    int x, y, rows, columns;
    int door[2];
    room_type type;
} Room;

typedef struct
{
    int x, y;
    int health,  gold;
    float hunger;
    int damage;
    int damage_duration;
    int speed;
    int speed_duration;
    int foods[4];
    int weapons[5];
    int talismans[3];
} Player;

wchar_t map[4][MAP_ROWS][MAP_COLUMNS];
int vision_map[4][MAP_ROWS][MAP_COLUMNS];
wchar_t player_map[4][MAP_ROWS][MAP_COLUMNS];
int rooms_number[4] = {0};
Room rooms[4][MAX_ROOMS];
Player player;
Stares_plase stares_plase[4];
int pick = 1;
void print_message(int message, int m2)
{
    move(1, 7);
    clrtoeol();
    mvprintw(1, 7, "                                            gold : %d        hunger : %d", message, m2);
    refresh();
}

void game_win()
{
    game_win_();
    char a = getch();
    return;
}
void talisman_choice(int n)
{
    if(n==0)
    {
        player.health +=20;
    }
    if(n == 2)
    {
        player.damage_duration += 10;
    }
    if(n == 1)
    {
        player.speed_duration += 20;
    }
}

void food_choice(int n)
{
    if(n==0)
    {
        player.hunger += 20;
    }
    if(n == 1)
    {
        player.health -= 5;
        player.hunger += 10;
    }
    if(n == 2)
    {
        player.hunger+=20;
        player.damage_duration += 10;
    }
    if(n == 3)
    {
        player.speed_duration += 20;
        player.hunger+=20;
    }
}

void talisman_window() {
    int height = 12, width = 40;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;

    WINDOW *potion = newwin(height, width, start_y, start_x);
    wclear(potion);
    box(potion, 0, 0);

    int len = strlen("** TALISMANS **");
    mvwprintw(potion, 1, (width - len) / 2, "** TALISMANS **");

    int salve = 0, madness = 0, boot = 0;
    salve = player.talismans[0];
    madness = player.talismans[1];
    boot = player.talismans[2];

    int salve_id = 0, madness_id = 0, boot_id = 0;
    int identifier = 1;
    if (salve != 0) {
        mvwprintw(potion, identifier + 1, 1, "%d. %d Healing Salve", identifier, salve);
        salve_id = identifier++;
    }
    if (madness != 0) {
        mvwprintw(potion, identifier + 1, 1, "%d. %d Mask of Madness", identifier, madness);
        madness_id = identifier++;
    }
    if (boot != 0) {
        mvwprintw(potion, identifier + 1, 1, "%d. %d Boot of Travel", identifier, boot);
        boot_id = identifier++;
    }
    if (salve == 0 && madness == 0 && boot == 0) {
        char text[] = "You don't have any talismans to use!";
        int x = (width - strlen(text)) / 2;
        mvwprintw(potion, 4, x, "%s", text);
    }

    wrefresh(potion);
    int choice = wgetch(potion) - '0';

    if (choice == salve_id) talisman_choice(0);
    else if (choice == madness_id) talisman_choice(2);
    else if (choice == boot_id) talisman_choice(1);
    wclear(potion);
    delwin(potion);
}



void food_window() {
    int win_h = 10, win_w = 50;
    int start_y = (LINES - win_h) / 2;
    int start_x = (COLS - win_w) / 2;

    WINDOW *food = newwin(win_h, win_w, start_y, start_x);
    wclear(food);
    box(food, 0, 0);
    int adi_id=0, fased_id=0, ala_id=0, jadoo_id=0;
    int adi = player.foods[0],  fased = player.foods[1], ala = player.foods[2], jadoo = player.foods[3];
    wattron(food, COLOR_PAIR(1));
    int len = strlen("** FOOD **");
    mvwprintw(food, 1, (win_w - len) / 2, "** FOOD **");
    wattroff(food, COLOR_PAIR(1));


    int identifier = 1;
    if (adi != 0) {
        mvwprintw(food, identifier + 1, 1, "%d. %d regular food", identifier, adi);
        adi_id = identifier;
        identifier++;
    }
    if (fased != 0) {
        mvwprintw(food, identifier + 1, 1, "%d. %d spoiled food", identifier, fased);
        fased_id = identifier;
        identifier++;
    }
    if (ala != 0) {
        mvwprintw(food, identifier + 1, 1, "%d. %d premium food", identifier, ala);
        ala_id = identifier;
        identifier++;
    }
    if (jadoo != 0) {
        mvwprintw(food, identifier + 1, 1, "%d. %d magical food", identifier, jadoo);
        jadoo_id = identifier;
        identifier++;
    }


    if (ala == 0 && adi ==0 && jadoo ==0&&fased==0) {
        //print_message("You don't have any food to consume!");
    }

    wrefresh(food);

    int choice = getch();
    choice = choice - '0';
    if (choice == adi_id) food_choice(0);
    else if (choice == jadoo_id) food_choice(3);
    else if (choice == fased_id) food_choice(1);
    else if (choice == ala_id) food_choice(2);
}

void each_move(int new_x, int new_y)
{
    player.x = new_x;
    player.y = new_y;
    player.hunger -= 1;
    if (player.damage_duration > 0)
        player.damage_duration--;
    if (player.speed_duration > 0)
        player.speed_duration--;
}
void print_health(int floor_number)
{
    int health = player.health;

    if (health > 100) health = 100;
    int filled = (health * 20) / 100;
    move(LINES - 1, COLS - 20 - 10);
    addch('[');
    for (int i = 0; i < 20; i++) {
        if (i < filled) {
            attron(COLOR_PAIR(1));
            addch('=');
            attroff(COLOR_PAIR(1));
            refresh();
        } else {
            attron(COLOR_PAIR(1));
            addch(' ');
            attroff(COLOR_PAIR(1));
            refresh();
        }
    }
    addch(']');
    mvprintw(LINES - 1, COLS - 39, "Health: ");
    mvprintw(LINES - 1, COLS - 8, "%d %% ", (health * 100) / 100);
    refresh();
}

void colors()
{
    start_color();
    init_color(10, 700, 700, 700); // یه سفید ملایم‌تر
    init_pair(1, COLOR_BLACK, 10);
}

int find_my_room(int floor_number, int x, int y)
{
    for(int i = 0; i < rooms_number[floor_number]; i++)
    {
        for (int j = rooms[floor_number][i].y; j < rooms[floor_number][i].y + rooms[floor_number][i].rows ; j++)
        {
            for (int k = rooms[floor_number][i].x; k < rooms[floor_number][i].x + rooms[floor_number][i].columns ; k++)
            {
                if((x == k) && (y == j))
                {
                    return i;
                }
            }
        }
    }
    return -1;
}

void full_vision_room(int floor_number, Room room)
{
    for (int i = room.y; i < room.y + room.rows; i++)
    {
        for (int j = room.x; j < room.x + room.columns; j++)
        {
            vision_map[floor_number][i][j] = 1;
        }
    }
}

void generate_treaser(int floor_number)
{
    for (int i = 0; i < rooms_number[floor_number]; i++)
    {
        if (rooms[floor_number][i].type == treasure)
        {
            int x, y;
            do
            {
                x = RANDOM(rooms[floor_number][i].x + 1, rooms[floor_number][i].x + rooms[floor_number][i].columns - 2);
                y = RANDOM(rooms[floor_number][i].y + 1, rooms[floor_number][i].y + rooms[floor_number][i].rows - 2);
            } while (map[floor_number][y][x] != '.');
            map[floor_number][y][x] = 'T';
        }
    }
}
void generate_enemy(int floor_number)
{
    for (int i = 0; i < rooms_number[floor_number]; i++)
    {
        int area = (rooms[floor_number][i].columns - 2) * (rooms[floor_number][i].rows - 2);
        if (rooms[floor_number][i].type == regular)
        {
            for (int j = 0; j < RANDOM(1, 3); j++)
            {
                int x, y;
                do
                {
                    x = RANDOM(rooms[floor_number][i].x + 1, rooms[floor_number][i].x + rooms[floor_number][i].columns - 2);
                    y = RANDOM(rooms[floor_number][i].y + 1, rooms[floor_number][i].y + rooms[floor_number][i].rows - 2);
                } while (map[floor_number][y][x] != '.');
                int chance = RANDOM(0, 99);
                if (chance < 30)
                    map[floor_number][y][x] = L'௹';
                else if (chance < 50)
                    map[floor_number][y][x] = L'ʨ';
                else if (chance < 70)
                    map[floor_number][y][x] = L'⋔';
                else if (chance < 85)
                    map[floor_number][y][x] = L'₰';
                else if (chance < 100)
                    map[floor_number][y][x] = L'Ǔ';
            }
        }

        if (rooms[floor_number][i].type == treasure)
        {
            for (int j = 0; j < RANDOM(4, 6); j++)
            {
                int x, y;
                do
                {
                    x = RANDOM(rooms[floor_number][i].x + 1, rooms[floor_number][i].x + rooms[floor_number][i].columns - 2);
                    y = RANDOM(rooms[floor_number][i].y + 1, rooms[floor_number][i].y + rooms[floor_number][i].rows - 2);
                } while (map[floor_number][y][x] != '.');
                int chance = RANDOM(0, 99);
                if (chance < 30)
                    map[floor_number][y][x] = L'௹';
                else if (chance < 50)
                    map[floor_number][y][x] = L'ʨ';
                else if (chance < 70)
                    map[floor_number][y][x] = L'⋔';
                else if (chance < 85)
                    map[floor_number][y][x] = L'₰';
                else if (chance < 100)
                    map[floor_number][y][x] = L'Ǔ';
            }
        }
    }
}

void generate_weapon(int floor_number)
{
    for (int i = 0; i < rooms_number[floor_number]; i++)
    {
        int area = (rooms[floor_number][i].columns - 2) * (rooms[floor_number][i].rows - 2);
        for (int j = 0; j < ((area + 25) / 50); j++)
        {
            int x, y;
            do
            {
                x = RANDOM(rooms[floor_number][i].x + 1, rooms[floor_number][i].x + rooms[floor_number][i].columns - 2);
                y = RANDOM(rooms[floor_number][i].y + 1, rooms[floor_number][i].y + rooms[floor_number][i].rows - 2);
            } while (map[floor_number][y][x] != '.');
            int chance = RANDOM(1, 4);
            if (chance == 1)
            {
                map[floor_number][y][x] = L'◭';
            }
            else if (chance == 2)
            {
                map[floor_number][y][x] = L'⇜';
            }
            else if (chance == 3)
            {
                map[floor_number][y][x] = L'↢';
            }
            else
            {
                map[floor_number][y][x] = L'◄';
            }
        }
    }
}

void generate_talisman(int floor_number)
{
    for (int i = 0; i < rooms_number[floor_number]; i++)
    {
        int area = (rooms[floor_number][i].columns - 2) * (rooms[floor_number][i].rows - 2);
        if (rooms[floor_number][i].type == regular || rooms[floor_number][i].type == treasure)
        {
            for (int j = 0; j < ((area + 25) / 50); j++)
            {
                int x, y;
                do
                {
                    x = RANDOM(rooms[floor_number][i].x + 1, rooms[floor_number][i].x + rooms[floor_number][i].columns - 2);
                    y = RANDOM(rooms[floor_number][i].y + 1, rooms[floor_number][i].y + rooms[floor_number][i].rows - 2);
                } while (map[floor_number][y][x] != '.');
                int chance = RANDOM(1, 3);
                if (chance == 1)
                    map[floor_number][y][x] = L'⊗';
                if (chance == 2)
                    map[floor_number][y][x] = L'⊚';
                if (chance == 3)
                    map[floor_number][y][x] = L'⊕';
            }
        }

        if (rooms[floor_number][i].type == enchant)
        {
            for (int j = 0; j < (((area + 25) / 50) * 2); j++)
            {
                int x, y;
                do
                {
                    x = RANDOM(rooms[floor_number][i].x + 1, rooms[floor_number][i].x + rooms[floor_number][i].columns - 2);
                    y = RANDOM(rooms[floor_number][i].y + 1, rooms[floor_number][i].y + rooms[floor_number][i].rows - 2);
                } while (map[floor_number][y][x] != '.');
                int chance = RANDOM(1, 3);
                if (chance == 1)
                    map[floor_number][y][x] = L'⊗';
                if (chance == 2)
                    map[floor_number][y][x] = L'⊚';
                if (chance == 3)
                    map[floor_number][y][x] = L'⊕';
            }
        }
    }
}
void generate_food(int floor_number)
{
    for (int i = 0; i < rooms_number[floor_number]; i++)
    {
        int area = (rooms[floor_number][i].columns - 2) * (rooms[floor_number][i].rows - 2);
        for (int j = 0; j < ((area + 25) / 25); j++)
        {
            int x, y;
            do
            {
                x = RANDOM(rooms[floor_number][i].x + 1, rooms[floor_number][i].x + rooms[floor_number][i].columns - 2);
                y = RANDOM(rooms[floor_number][i].y + 1, rooms[floor_number][i].y + rooms[floor_number][i].rows - 2);
            } while (map[floor_number][y][x] != '.');
            int chance = RANDOM(0, 99);
            if (chance < 65)
            {
                map[floor_number][y][x] = '*';
            }
            else if (chance < 80)
            {
                map[floor_number][y][x] = 'x';
            }
            else if (chance < 90)
            {
                map[floor_number][y][x] = L'φ';
            }
            else
            {
                map[floor_number][y][x] = L'∞';
            }
        }
    }
}

void generate_gold(int floor_number)
{
    for (int i = 0; i < rooms_number[floor_number]; i++)
    {
        int area = (rooms[floor_number][i].columns - 2) * (rooms[floor_number][i].rows - 2);
        for (int j = 0; j < ((area + 25) / 50); j++)
        {
            int x, y;
            do
            {
                x = RANDOM(rooms[floor_number][i].x + 1, rooms[floor_number][i].x + rooms[floor_number][i].columns - 2);
                y = RANDOM(rooms[floor_number][i].y + 1, rooms[floor_number][i].y + rooms[floor_number][i].rows - 2);
            } while (map[floor_number][y][x] != '.');
            int chance = RANDOM(1, 10);
            if (chance < 6)
            {
                map[floor_number][y][x] = L'$';
            }
            else if (chance < 9)
            {
                map[floor_number][y][x] = L'€';
            }
            else
            {
                map[floor_number][y][x] = L'£';
            }
        }
    }
}

void generate_trap(int floor_number)
{
    for (int i = 0; i < rooms_number[floor_number]; i++)
    {
        int area = (rooms[floor_number][i].columns - 2) * (rooms[floor_number][i].rows - 2);
        if (rooms[floor_number][i].type == regular)
        {
            for (int j = 0; j < ((area + 15) / 50); j++)
            {
                int x, y;
                do
                {
                    x = RANDOM(rooms[floor_number][i].x + 1, rooms[floor_number][i].x + rooms[floor_number][i].columns - 2);
                    y = RANDOM(rooms[floor_number][i].y + 1, rooms[floor_number][i].y + rooms[floor_number][i].rows - 2);
                } while (map[floor_number][y][x] != '.');
                map[floor_number][y][x] = '^';
            }
        }
        if (rooms[floor_number][i].type == enchant)
        {
            for (int j = 0; j < (((area + 15) / 50) * 2); j++)
            {
                int x, y;
                do
                {
                    x = RANDOM(rooms[floor_number][i].x + 1, rooms[floor_number][i].x + rooms[floor_number][i].columns - 2);
                    y = RANDOM(rooms[floor_number][i].y + 1, rooms[floor_number][i].y + rooms[floor_number][i].rows - 2);
                } while (map[floor_number][y][x] != '.');
                map[floor_number][y][x] = '^';
            }
        }
        if (rooms[floor_number][i].type == treasure)
        {
            for (int j = 0; j < (((area + 15) / 50) * 3.5); j++)
            {
                int x, y;
                do
                {
                    x = RANDOM(rooms[floor_number][i].x + 1, rooms[floor_number][i].x + rooms[floor_number][i].columns - 2);
                    y = RANDOM(rooms[floor_number][i].y + 1, rooms[floor_number][i].y + rooms[floor_number][i].rows - 2);
                } while (map[floor_number][y][x] != '.');
                map[floor_number][y][x] = '^';
            }
        }
    }
}

void spawn_player(int floor_number, int x, int y)
{
    if (x == -1 && y == -1)
    {
        do
        {
            x = RANDOM(rooms[floor_number][0].x + 2, rooms[floor_number][0].x + rooms[floor_number][0].columns - 3);
            y = RANDOM(rooms[floor_number][0].y + 2, rooms[floor_number][0].y + rooms[floor_number][0].rows - 3);
        } while (map[floor_number][y][x] != '.');
        player.x = x;
        player.y = y;
        player.health = 100;
        player.hunger = 100;
        player.gold = 0;
        player.damage_duration = 0;
        player.speed_duration = 0;
        for (int i = 0; i < 3; i++)
            player.talismans[i] = 0;
        for (int i = 0; i < 4; i++)
            player.foods[i] = 0;
        for (int i = 0; i < 5; i++)
            player.weapons[i] = 0;
    }
    else
    {
        player.x = x;
        player.y = y;
    }
}

void change_floor(int *floor_number, int d)
{
    *floor_number += d;
    spawn_player(*floor_number, stares_plase[*floor_number].x, stares_plase[*floor_number].y);
}

void generate_stare(int floor_number)
{
    int x, y;
    do
    {
        x = RANDOM(rooms[floor_number][3].x + 2, rooms[floor_number][3].x + rooms[floor_number][3].columns - 3);
        y = RANDOM(rooms[floor_number][3].y + 2, rooms[floor_number][3].y + rooms[floor_number][3].rows - 3);
    } while (map[floor_number][y][x] != '.');
    map[floor_number][y][x] = L'⇕';
    stares_plase[floor_number].x = x;
    stares_plase[floor_number].y = y;
}

void generate_pillars(int floor_number)
{
    for (int i = 0; i < rooms_number[floor_number]; i++)
    {
        int area = (rooms[floor_number][i].columns - 2) * (rooms[floor_number][i].rows - 2);
        for (int j = 0; j < (area / 16); j++)
        {
            int x, y;
            do
            {
                x = RANDOM(rooms[floor_number][i].x + 2, rooms[floor_number][i].x + rooms[floor_number][i].columns - 3);
                y = RANDOM(rooms[floor_number][i].y + 2, rooms[floor_number][i].y + rooms[floor_number][i].rows - 3);
            } while (map[floor_number][y][x] != '.');
            map[floor_number][y][x] = L'▧';
        }
    }
}

int over_door(int floor_number)
{
    int door_count = 0;
    for (int i = 0; i < MAP_COLUMNS; i++)
    {
        for (int j = 0; j < MAP_ROWS; j++)
        {
            if (map[floor_number][j][i] == '+')
            {
                door_count++;
            }
        }
    }
    if (door_count > rooms_number[floor_number] * 3.4)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
void connect_doors(int x1, int y1, int x2, int y2, int floor_number)
{

    if (rand() % 2)
    {
        while (x1 != x2)
        {
            if ((map[floor_number][y1][x1] == L'║' || map[floor_number][y1][x1] == L'═'))
            {
                map[floor_number][y1][x1] = '+';
            }
            else if (map[floor_number][y1][x1] == ' ')
            {
                map[floor_number][y1][x1] = L'▒';
            }
            x1 += (x2 > x1) ? 1 : -1;
        }

        while (y1 != y2)
        {
            if ((map[floor_number][y1][x1] == L'║' || map[floor_number][y1][x1] == L'═'))
            {
                map[floor_number][y1][x1] = '+';
            }
            else if (map[floor_number][y1][x1] == ' ')
            {
                map[floor_number][y1][x1] = L'▒';
            }
            y1 += (y2 > y1) ? 1 : -1;
        }
    }
    else
    {
        while (y1 != y2)
        {
            if ((map[floor_number][y1][x1] == L'║' || map[floor_number][y1][x1] == L'═'))
            {
                map[floor_number][y1][x1] = '+';
            }
            else if (map[floor_number][y1][x1] == ' ')
            {
                map[floor_number][y1][x1] = L'▒';
            }
            y1 += (y2 > y1) ? 1 : -1;
        }

        while (x1 != x2)
        {
            if ((map[floor_number][y1][x1] == L'║' || map[floor_number][y1][x1] == L'═'))
            {
                map[floor_number][y1][x1] = '+';
            }
            else if (map[floor_number][y1][x1] == ' ')
            {
                map[floor_number][y1][x1] = L'▒';
            }
            x1 += (x2 > x1) ? 1 : -1;
        }
    }
}

void generate_corridors(int floor_number)
{
    for (int i = 2; i < rooms_number[floor_number]; i++)
    {
        connect_doors(rooms[floor_number][i - 2].door[0], rooms[floor_number][i - 2].door[1], rooms[floor_number][i].door[0], rooms[floor_number][i].door[1], floor_number);
    }
    for (int i = 1; i < rooms_number[floor_number]; i++)
    {
        connect_doors(rooms[floor_number][i - 1].door[0], rooms[floor_number][i - 1].door[1], rooms[floor_number][i].door[0], rooms[floor_number][i].door[1], floor_number);
    }
    if (floor_number == 3)
    {
        for (int i = 2; i < rooms_number[floor_number] - 1; i++)
        {
            connect_doors(rooms[floor_number][i - 2].door[0], rooms[floor_number][i - 2].door[1], rooms[floor_number][i].door[0], rooms[floor_number][i].door[1], floor_number);
        }
        for (int i = 1; i < rooms_number[floor_number] - 1; i++)
        {
            connect_doors(rooms[floor_number][i - 1].door[0], rooms[floor_number][i - 1].door[1], rooms[floor_number][i].door[0], rooms[floor_number][i].door[1], floor_number);
        }
        connect_doors(rooms[floor_number][0].door[0], rooms[floor_number][0].door[1], rooms[floor_number][rooms_number[floor_number] - 1].door[0], rooms[floor_number][rooms_number[floor_number] - 1].door[1], floor_number);
    }
}

void generate_doors(int floor_number, int room_index)
{
    Room *r = &rooms[floor_number][room_index];

    int dx, dy, valid_position = 0;

    int side = RANDOM(0, 3);
    dx = dy = 0;

    if (side == 0 && r->y > 0)
    {
        dy = 1;
        dx = RANDOM(1, r->columns - 2);
    }
    else if (side == 1 && r->y + r->rows < MAP_ROWS - 1)
    {
        dy = r->rows - 2;
        dx = RANDOM(1, r->columns - 2);
    }
    else if (side == 2 && r->x > 0)
    {
        dy = RANDOM(1, r->rows - 2);
        dx = 1;
    }
    else if (side == 3 && r->x + r->columns < MAP_COLUMNS - 1)
    {
        dy = RANDOM(1, r->rows - 2);
        dx = r->columns - 2;
    }

    r->door[0] = r->x + dx;
    r->door[1] = r->y + dy;
    valid_position = 1;
}

void generate_rooms(int floor_number, int room_number)
{
    if (room_number == 0 || rooms_number[floor_number] >= MAX_ROOMS)
        return;

    int attempts = 100;
    while (attempts--)
    {
        int room_x = RANDOM(1, MAP_COLUMNS - 15);
        int room_y = RANDOM(1, MAP_ROWS - 15);
        int room_rows = RANDOM(8, 15);
        int room_columns = RANDOM(8, 15);
        int valid = 1;

        for (int i = room_x - 1; i < room_x + room_columns + 1; i++)
        {
            for (int j = room_y; j < room_y + room_rows + 1; j++)
            {
                if (map[floor_number][j][i] != ' ')
                    valid = 0;
            }
        }

        if (valid)
        {
            map[floor_number][room_y + room_rows - 1][room_x] = L'╚';
            for (int i = room_x + 1; i < room_x + room_columns - 1; i++)
            {
                map[floor_number][room_y + room_rows - 1][i] = L'═';
            }
            map[floor_number][room_y + room_rows - 1][room_x + room_columns - 1] = L'╝';

            for (int i = room_y + 1; i < room_y + room_rows - 1; i++)
            {
                map[floor_number][i][room_x] = L'║';
                map[floor_number][i][room_x + room_columns - 1] = L'║';
            }
            map[floor_number][room_y][room_x] = L'╔';
            for (int i = room_x + 1; i < room_x + room_columns - 1; i++)
            {
                map[floor_number][room_y][i] = L'═';
            }
            map[floor_number][room_y][room_x + room_columns - 1] = L'╗';
            for (int i = room_x + 1; i < room_x + room_columns - 1; i++)
            {
                for (int j = room_y + 1; j < room_y + room_rows - 1; j++)
                {
                    map[floor_number][j][i] = '.';
                }
            }

            int room_index = rooms_number[floor_number]++;
            room_type type;
            int chance = RANDOM(1, 6);
            if (chance == 1)
            {
                rooms[floor_number][room_index] = (Room){room_x, room_y, room_rows, room_columns, {}, enchant};
            }
            else
            {
                rooms[floor_number][room_index] = (Room){room_x, room_y, room_rows, room_columns, {}, regular};
            }
            if (floor_number == 3 && room_number == 1)
            {
                rooms[floor_number][room_index] = (Room){room_x, room_y, room_rows, room_columns, {}, treasure};
            }
            generate_doors(floor_number, room_index);
            generate_rooms(floor_number, room_number - 1);
            return;
        }
    }
}

void generate_map(int floor_number, int room_number)
{

    do
    {
        for (int j = 0; j < MAP_ROWS; j++)
        {
            for (int k = 0; k < MAP_COLUMNS; k++)
            {
                map[floor_number][j][k] = ' ';
                vision_map[floor_number][j][k] = 0;
            }
        }

        rooms_number[floor_number] = 0;
        generate_rooms(floor_number, room_number);
        generate_corridors(floor_number);
    } while (over_door(floor_number));
    generate_pillars(floor_number);
    generate_stare(floor_number);
    generate_trap(floor_number);
    generate_gold(floor_number);
    generate_food(floor_number);
    generate_talisman(floor_number);
    generate_weapon(floor_number);
    generate_enemy(floor_number);
    generate_treaser(floor_number);
    full_vision_room(0, rooms[0][0]);
    full_vision_room(1, rooms[1][3]);
    full_vision_room(2, rooms[2][3]);
    full_vision_room(3, rooms[3][3]);
}

int player_move(int floor_number, direction dr)
{
    int new_x = player.x, new_y = player.y;
    switch (dr)
    {
    case N:
        new_y--;
        break;
    case NE:
        new_y--;
        new_x++;
        break;
    case E:
        new_x++;
        break;
    case SE:
        new_y++;
        new_x++;
        break;
    case S:
        new_y++;
        break;
    case SW:
        new_y++;
        new_x--;
        break;
    case W:
        new_x--;
        break;
    case NW:
        new_x--;
        new_y--;
        break;

    default:
        break;
    }
    wchar_t next_step = map[floor_number][new_y][new_x];

    if (next_step == L'▧' || next_step == L'║' || next_step == ' ' || next_step == L'═')
        return;
    each_move(new_x, new_y);


    if(next_step == '+' || next_step == 'Z')
    {
        full_vision_room(floor_number, rooms[floor_number][find_my_room(floor_number, new_x, new_y)]);
    }
    if(next_step == L'▒')
    {
        vision_map[floor_number][new_y][new_x] = 1;
        vision_map[floor_number][new_y][new_x+1] = 1;
        vision_map[floor_number][new_y][new_x-1] = 1;
        vision_map[floor_number][new_y+1][new_x+1] = 1;
        vision_map[floor_number][new_y+1][new_x-1] = 1;
        vision_map[floor_number][new_y+1][new_x] = 1;
        vision_map[floor_number][new_y-1][new_x+1] = 1;
        vision_map[floor_number][new_y-1][new_x-1] = 1;
        vision_map[floor_number][new_y-1][new_x] = 1;
    }

    //trap
    if (next_step == '^')
    {
        player.health -= 5;
    }

    // golds

    if (next_step == '$' && pick == 1)
    {
        map[floor_number][new_y][new_x] = '.';
        player.gold += 50;
    }
    if (next_step == L'€' && pick == 1)
    {
        map[floor_number][new_y][new_x] = '.';
        player.gold += 100;
    }
    if (next_step == L'£' && pick == 1)
    {
        map[floor_number][new_y][new_x] = '.';
        player.gold += 200;
    }

    // foods

    if (next_step == '*' && pick == 1)
    {
        map[floor_number][new_y][new_x] = '.';
        player.foods[0]++;
        player.hunger += 20;
    }
    if (next_step == 'x' && pick == 1)
    {
        map[floor_number][new_y][new_x] = '.';
        player.foods[1]++;
        player.health -= 5;
    }
    if (next_step == L'φ' && pick == 1)
    {
        player.damage_duration += 10;
        map[floor_number][new_y][new_x] = '.';
        player.foods[2]++;
    }
    if (next_step == L'∞' && pick == 1)
    {
        player.speed_duration += 20;
        map[floor_number][new_y][new_x] = '.';
        player.foods[3]++;
    }

    // talismans

    if (next_step == L'⊗' && pick == 1)
    {
        map[floor_number][new_y][new_x] = '.';
        player.talismans[0]++;
    }
    if (next_step == L'⊕' && pick == 1)
    {
        map[floor_number][new_y][new_x] = '.';
        player.talismans[1]++;
    }
    if (next_step == L'⊚' && pick == 1)
    {
        map[floor_number][new_y][new_x] = '.';
        player.talismans[2]++;
    }

    // weapon

    if (next_step == L'◭' && pick == 1)
    {
        map[floor_number][new_y][new_x] = '.';
        player.weapons[1] = 1;
    }
    if (next_step == L'⇜' && pick == 1)
    {
        map[floor_number][new_y][new_x] = '.';
        player.weapons[2] += 8;
    }
    if (next_step == L'↢' && pick == 1)
    {
        map[floor_number][new_y][new_x] = '.';
        player.weapons[3] += 20;
    }
    if (next_step == L'◄' && pick == 1)
    {
        map[floor_number][new_y][new_x] = '.';
        player.weapons[4] += 10;
    }
    if(next_step == 'T')
    {
        game_win();
        return 1;
        exit(0);
    }

    if (player.speed_duration > 0 && player.speed_duration % 2 == 1)
    {
        player_move(floor_number, dr);
    }
    return 0;
    
}
void show_map(int floor_number)
{
    wchar_t temp_line[MAP_COLUMNS + 1];
    for (int i = 0; i < MAP_ROWS; i++)
    {
        wmemcpy(temp_line, map[floor_number][i], MAP_COLUMNS);
        temp_line[MAP_COLUMNS] = L'\0';

        attron(COLOR_PAIR(1));
        mvaddwstr(i + 4, 7, temp_line);
        attroff(COLOR_PAIR(1));
    }
    attron(COLOR_PAIR(1));
    mvaddwstr(player.y + 4, player.x + 7, L"₪");
    attroff(COLOR_PAIR(1));
    refresh();
}

void show_visable_map(int floor_number)
{
    wchar_t temp_line[MAP_COLUMNS + 1];
    for (int i = 0; i < MAP_ROWS; i++)
    {
        wmemcpy(temp_line, map[floor_number][i], MAP_COLUMNS);
        temp_line[MAP_COLUMNS] = L'\0';
        for (int j = 0; j < MAP_COLUMNS; j++)
        {
            if(!(vision_map[floor_number][i][j]))
                temp_line[j] = ' ';
        }
        attron(COLOR_PAIR(1));
        mvaddwstr(i + 4, 7, temp_line);
        attroff(COLOR_PAIR(1));
    }
    attron(COLOR_PAIR(1));
    mvaddwstr(player.y + 4, player.x + 7, L"₪");
    attroff(COLOR_PAIR(1));
    refresh();
}



int game()
{
    srand(time(NULL));

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    setlocale(LC_ALL, "");
    colors();

    int full_vision = 0;
    int floor_number = 0;
    int *floor_numberp = &floor_number;
    for (int i = 0; i < 4; i++)
    {
        generate_map(i, RANDOM(6, 8));
    }
    spawn_player(floor_number, -1, -1);

    show_visable_map(floor_number);

    while (1)
    {
        if(player.health<=0 || player.hunger<=0)
        {
            //game_lose();
        }
        if(player.health>100)
        {
            player.health = 100;
        }
        if(player.hunger >100)
        {
            player.hunger =100;
        }
        int input = getch();
        
        switch (input)
        {
        case 'm':
            full_vision = (full_vision == 1) ? 0 : 1;
            break;
        case 'z':
            main();
            break;
        case 'W':
        case 'w':
        case '8':
            player_move(floor_number, N);
            break;

        case '9':
            player_move(floor_number, NE);
            break;

        case 'd':
        case 'D':
        case '6':
            player_move(floor_number, E);
            break;

        case '3':
            player_move(floor_number, SE);
            break;

        case 's':
        case 'S':
        case '2':
            player_move(floor_number, S);
            break;

        case '1':
            player_move(floor_number, SW);
            break;

        case 'a':
        case 'A':
        case '4':
            player_move(floor_number, W);
            break;

        case '7':
            player_move(floor_number, NW);
            break;
        case '>':
            if (player.x == stares_plase[floor_number].x && player.y == stares_plase[floor_number].y && floor_number != 3)
                change_floor(floor_numberp, +1);
            break;
        case '<':
            if (player.x == stares_plase[floor_number].x && player.y == stares_plase[floor_number].y && floor_number != 0)
                change_floor(floor_numberp, -1);
            break;
        case 'g':
        case 'G':
            pick = (pick+1)%2;
        case 'e':
        case 'E':
            food_window();
            break;
        case 't':
        case 'T':
            talisman_window();
            break;
        default:
            break;
        }
        if (full_vision == 1)
        {
            show_map(floor_number);
        }
        else
        {
            show_visable_map(floor_number);
        }
        print_message(player.gold, player.hunger);
        print_health(floor_number);
    }
}
