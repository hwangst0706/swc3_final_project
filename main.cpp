#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <random>

// 방향키
#define UP 72
#define DOWN 80
#define LEFT 75
#define RIGHT 77

#define START_POS_X 4
#define START_POS_Y 22
#define MAX_HP 3

#define HUDDLE_START 90

using namespace std;
typedef unsigned int uint;

/*----------자료형 영역----------*/
enum {
    BLACK, DARK_BLUE, DARK_GREEN, DARK_SKYBLUE, DARK_RED, DARK_VOILET, DAKR_YELLOW,
    GRAY, DARK_GRAY, BLUE, GREEN, SKYBLUE, RED, VIOLET, YELLOW, WHITE,
};

typedef struct _position {
    int x;
    int y;
}P;

class Huddle {
private:
    uint type; //rock, tree, bird, cloud
    P pos;
public:
    Huddle() { pos.x = HUDDLE_START; pos.y = 0; type = 0; }
    Huddle(uint t) : type(t) { pos.x = HUDDLE_START; pos.y = 0; }
    uint get_type() { return type; }
    int get_Xpos() { return pos.x; }
    int get_Ypos() { return pos.y; }
    void set_Xpos(int val) { pos.x = val; }
    void set_Ypos(int val) { pos.y = val; }
    void set_type(uint t) { type = t; }
};

class Player {
private:
    uint score;
    uint hp;
    P pos;
public:
    Player() : score(0), hp(3) { pos.x = START_POS_X; pos.y = START_POS_Y; }
    uint get_score() { return score; }
    uint get_hp() { return hp; }
    int get_Xpos() { return pos.x; }
    int get_Ypos() { return pos.y; }
    void set_score(uint s) { score = s; }
    void set_hp(uint val) { hp = val; }
    void set_Xpos(int val) { pos.x = val; }
    void set_Ypos(int val) { pos.y = val; }
    friend void Character();
    /*void reset(){
        score = 0;
        hp = MAX_HP;
        pos.x = START_POS_X;
        pos.y = START_POS_Y;
    }*/
};

class bullet {
private:
    P pos;
};

class map {
private:
    uint time;
};

Player playerInfo[3];
int playerNum;
Huddle huddleInfo[3];

/*----------함수 영역 시작----------*/
void init_players()
{
    for (int i = 0; i < 3; i++)
    {
        playerInfo[i].set_hp(3);
        playerInfo[i].set_Xpos(START_POS_X);
        playerInfo[i].set_Ypos(START_POS_Y);
    }
}

void get_scores()
{
    uint num[3] = { 0 };
    ifstream in("score.txt");
    if (!in.is_open()) return;
    in >> num[0] >> num[1] >> num[2];
    for (int i = 0; i < 3; i++)
    {
        playerInfo[i].set_score(num[i]);
    }
    in.close();
    return;
}

void set_scores()
{
    ofstream out("score.txt");
    for (int i = 0; i < 3; i++)
    {
        out << playerInfo[i].get_score() << " ";
    }
    out.close();
    return;
}

int check_fail(uint player, uint huddle, uint status)
{
    if (status == 1) return 0; //스타별을 먹은 상태이면 무조건 성공

    if (huddle == 0) // 돌
    {
        int x = huddleInfo[huddle].get_Xpos(), y = 30;
        for (int i = playerInfo[player].get_Xpos(); i < playerInfo[player].get_Xpos() + 9; i++)
        {
            for (int j = playerInfo[player].get_Ypos(); j < playerInfo[player].get_Ypos() + 11; j++)
            {
                for (int nx = x; nx < x + 4; nx++)
                {
                    for (int ny = y; ny < y + 4; ny++)
                    {
                        if (i == nx && j == ny) return 1;
                    }
                }
            }
        } 
    }
    else if (huddle == 1) // 나무
    {
        int x = huddleInfo[huddle].get_Xpos(), y = 25;
        for (int i = playerInfo[player].get_Xpos(); i < playerInfo[player].get_Xpos() + 9; i++)
        {
            for (int j = playerInfo[player].get_Ypos(); j < playerInfo[player].get_Ypos() + 11; j++)
            {
                for (int nx = x; nx < x + 5; nx++)
                {
                    for (int ny = y; ny < y + 8; ny++)
                    {
                        if (i == nx && j == ny) return 1;
                    }
                }
            }
        }

    }
    else if (huddle == 2) // 새
    {
        int x = huddleInfo[huddle].get_Xpos(), y = 5;
        for (int i = playerInfo[player].get_Xpos(); i < playerInfo[player].get_Xpos() + 9; i++)
        {
            for (int j = playerInfo[player].get_Ypos(); j < playerInfo[player].get_Ypos() + 11; j++)
            {
                for (int nx = x; nx < x + 12; nx++)
                {
                    for (int ny = y; ny < y + 4; ny++)
                    {
                        if (i == nx && j == ny) return 1;
                    }
                }
            }
        }
    }
    return 0;
}

void init_huddle()
{
    for (int i = 0; i < 3; i++)
    {
        huddleInfo[i].set_type(i);
        huddleInfo[i].set_Xpos(HUDDLE_START);
    }
}

void cursor_pos(int x, int y)
{
    COORD p;
    p.X = x;
    p.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), p);
}

int input_key()
{
    if (_kbhit())
        return _getch();
    return 0;
}

int input_key_dir()
{
    char c;
    if (_kbhit())
    {
        c = _getch();
        if (c == -32)
            return _getch();
    }
    return 0;
}

int MainMenu()
{
    int x = 44, y = 10;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);

    system("cls");
    cursor_pos(x, y);
    printf("=================================");
    cursor_pos(x + 4, y + 3);
    printf("▶  G A M E S T A R T");
    cursor_pos(x + 8, y + 6);
    printf("S C O R E");
    cursor_pos(x + 8, y + 9);
    printf("E X I T");
    cursor_pos(x, y + 12);
    printf("=================================");
    printf("\n\n\n\n\n\n\n\n\n\n\n");

    int menu = 0;
    bool isSelect = FALSE;

    while (1) {
        int key = input_key_dir();
        if (key == DOWN) menu++;
        else if (key == UP) menu += 2;
        else if (key == RIGHT) isSelect = TRUE;
        else if (key == 0) continue;

        menu %= 3;

        if (menu == 0) {
            system("cls");
            cursor_pos(x, y);
            printf("=================================");
            cursor_pos(x + 4, y + 3);
            printf("▶  G A M E S T A R T");
            cursor_pos(x + 8, y + 6);
            printf("S C O R E");
            cursor_pos(x + 8, y + 9);
            printf("E X I T");
            cursor_pos(x, y + 12);
            printf("=================================");
        }
        else if (menu == 1) {
            system("cls");
            cursor_pos(x, y);
            printf("=================================");
            cursor_pos(x + 8, y + 3);
            printf("G A M E S T A R T");
            cursor_pos(x + 4, y + 6);
            printf("▶  S C O R E");
            cursor_pos(x + 8, y + 9);
            printf("E X I T");
            cursor_pos(x, y + 12);
            printf("=================================");
        }
        else if (menu == 2) {
            system("cls");
            cursor_pos(x, y);
            printf("=================================");
            cursor_pos(x + 8, y + 3);
            printf("G A M E S T A R T");
            cursor_pos(x + 8, y + 6);
            printf("S C O R E");
            cursor_pos(x + 4, y + 9);
            printf("▶  E X I T");
            cursor_pos(x, y + 12);
            printf("=================================");
        }
        printf("\n\n\n\n\n\n\n\n\n\n\n");

        if (isSelect) break;
    }

    return menu;
}

void GameStart()
{
    int x = 46, y = 10;
    system("cls");
    cursor_pos(x, y);
    printf("=============================");
    cursor_pos(x + 4, y + 3);
    printf("▶  P L A Y E R  1");
    cursor_pos(x + 8, y + 6);
    printf("P L A Y E R  2");
    cursor_pos(x + 8, y + 9);
    printf("P L A Y E R  3");
    cursor_pos(x, y + 12);
    printf("=============================");
    printf("\n\n\n\n\n\n\n\n\n\n\n");

    bool isSelect = FALSE;

    while (1) {
        int key = input_key_dir();
        if (key == DOWN) playerNum++;
        else if (key == UP) playerNum += 2;
        else if (key == RIGHT) isSelect = TRUE;
        else if (key == 0) continue;

        playerNum %= 3;

        if (playerNum == 0) {
            system("cls");
            cursor_pos(x, y);
            printf("=============================");
            cursor_pos(x + 4, y + 3);
            printf("▶  P L A Y E R  1");
            cursor_pos(x + 8, y + 6);
            printf("P L A Y E R  2");
            cursor_pos(x + 8, y + 9);
            printf("P L A Y E R  3");
            cursor_pos(x, y + 12);
            printf("=============================");
        }
        else if (playerNum == 1) {
            system("cls");
            cursor_pos(x, y);
            printf("=============================");
            cursor_pos(x + 8, y + 3);
            printf("P L A Y E R  1");
            cursor_pos(x + 4, y + 6);
            printf("▶  P L A Y E R  2");
            cursor_pos(x + 8, y + 9);
            printf("P L A Y E R  3");
            cursor_pos(x, y + 12);
            printf("=============================");
        }
        else if (playerNum == 2) {
            system("cls");
            cursor_pos(x, y);
            printf("=============================");
            cursor_pos(x + 8, y + 3);
            printf("P L A Y E R  1");
            cursor_pos(x + 8, y + 6);
            printf("P L A Y E R  2");
            cursor_pos(x + 4, y + 9);
            printf("▶  P L A Y E R  3");
            cursor_pos(x, y + 12);
            printf("=============================");
        }
        printf("\n\n\n\n\n\n\n\n\n\n\n");

        if (isSelect) break;
    }
}

void Count3sec()
{
    int x = 54, y = 11;
    system("cls");
    cursor_pos(x, y++);
    printf("■■■■■■");
    cursor_pos(x, y++);
    printf("          ■");
    cursor_pos(x, y++);
    printf("          ■");
    cursor_pos(x, y++);
    printf("          ■");
    cursor_pos(x, y++);
    printf("          ■");
    cursor_pos(x, y++);
    printf("■■■■■■");
    cursor_pos(x, y++);
    printf("          ■");
    cursor_pos(x, y++);
    printf("          ■");
    cursor_pos(x, y++);
    printf("          ■");
    cursor_pos(x, y++);
    printf("          ■");
    cursor_pos(x, y++);
    printf("■■■■■■");
    printf("\n\n\n\n\n\n\n\n\n\n\n");
    Sleep(1000);

    system("cls");
    y = 11;
    cursor_pos(x, y++);
    printf("■■■■■■");
    cursor_pos(x, y++);
    printf("          ■");
    cursor_pos(x, y++);
    printf("          ■");
    cursor_pos(x, y++);
    printf("          ■");
    cursor_pos(x, y++);
    printf("          ■");
    cursor_pos(x, y++);
    printf("■■■■■■");
    cursor_pos(x, y++);
    printf("■");
    cursor_pos(x, y++);
    printf("■");
    cursor_pos(x, y++);
    printf("■");
    cursor_pos(x, y++);
    printf("■");
    cursor_pos(x, y++);
    printf("■■■■■■");
    printf("\n\n\n\n\n\n\n\n\n\n\n");
    Sleep(1000);

    system("cls");
    x = 55, y = 11;
    cursor_pos(x, y++);
    printf("  ■■");
    cursor_pos(x, y++);
    printf("    ■");
    cursor_pos(x, y++);
    printf("    ■");
    cursor_pos(x, y++);
    printf("    ■");
    cursor_pos(x, y++);
    printf("    ■");
    cursor_pos(x, y++);
    printf("    ■");
    cursor_pos(x, y++);
    printf("    ■");
    cursor_pos(x, y++);
    printf("    ■");
    cursor_pos(x, y++);
    printf("    ■");
    cursor_pos(x, y++);
    printf("    ■");
    cursor_pos(x, y++);
    printf("■■■■■");
    printf("\n\n\n\n\n\n\n\n\n\n\n");
    Sleep(1000);
}

void qCount3sec()
{
    int x = 55, y = 11;

    system("cls");
    cursor_pos(x, y++);
    printf("@@@@@@@@@@");
    cursor_pos(x, y++);
    printf("         @");
    cursor_pos(x, y++);
    printf("         @");
    cursor_pos(x, y++);
    printf("         @");
    cursor_pos(x, y++);
    printf("         @");
    cursor_pos(x, y++);
    printf("@@@@@@@@@@");
    cursor_pos(x, y++);
    printf("         @");
    cursor_pos(x, y++);
    printf("         @");
    cursor_pos(x, y++);
    printf("         @");
    cursor_pos(x, y++);
    printf("         @");
    cursor_pos(x, y++);
    printf("@@@@@@@@@@");
    Sleep(1000);

    system("cls");
    y = 11;
    cursor_pos(x, y++);
    printf("@@@@@@@@@@");
    cursor_pos(x, y++);
    printf("         @");
    cursor_pos(x, y++);
    printf("         @");
    cursor_pos(x, y++);
    printf("         @");
    cursor_pos(x, y++);
    printf("         @");
    cursor_pos(x, y++);
    printf("@@@@@@@@@@");
    cursor_pos(x, y++);
    printf("@");
    cursor_pos(x, y++);
    printf("@");
    cursor_pos(x, y++);
    printf("@");
    cursor_pos(x, y++);
    printf("@");
    cursor_pos(x, y++);
    printf("@@@@@@@@@@");
    Sleep(1000);

    system("cls");
    x = 55, y = 11;
    cursor_pos(x, y++);
    printf(" @@@@");
    cursor_pos(x, y++);
    printf("    @");
    cursor_pos(x, y++);
    printf("    @");
    cursor_pos(x, y++);
    printf("    @");
    cursor_pos(x, y++);
    printf("    @");
    cursor_pos(x, y++);
    printf("    @");
    cursor_pos(x, y++);
    printf("    @");
    cursor_pos(x, y++);
    printf("    @");
    cursor_pos(x, y++);
    printf("    @");
    cursor_pos(x, y++);
    printf("    @");
    cursor_pos(x, y++);
    printf("@@@@@@@@@");
    Sleep(1000);
}

void Score()
{
    string name[] = { "Player1", "Player2", "Player3" };
    int ranking[] = { 0, 1, 2 };
    int x = 45, y = 10;

    //sort(); ranking을 sort

    system("cls");
    cursor_pos(x, y);
    printf("=============================");
    cursor_pos(x + 5, y + 3);
    cout << "1st    " << name[ranking[0]] << "   " << playerInfo[ranking[0]].get_score();
    cursor_pos(x + 5, y + 6);
    cout << "2nd    " << name[ranking[1]] << "   " << playerInfo[ranking[1]].get_score();
    cursor_pos(x + 5, y + 9);
    cout << "3rd    " << name[ranking[2]] << "   " << playerInfo[ranking[2]].get_score();
    cursor_pos(x, y + 12);
    printf("=============================");
    printf("\n\n\n\n\n\n\n\n\n\n\n");

    while (1) {
        int key = input_key_dir();
        if (key == 0) continue;
        else break;
    }
}

void Character(Player myplay)
{
    int x = myplay.get_Xpos(), y = myplay.get_Ypos();
    //system("cls");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_GRAY);
    cursor_pos(x + 6, y++);
    printf("■■■");
    cursor_pos(x + 4, y++);
    printf("■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_BLUE);
    printf("■■■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_GRAY);
    printf("■");
    cursor_pos(x + 2, y++);
    printf("■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_BLUE);
    printf("■■■■■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_GRAY);
    printf("■");
    cursor_pos(x + 2, y++);
    printf("■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_BLUE);
    printf("■■■■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_GRAY);
    printf("■■");
    cursor_pos(x, y++);
    printf("■■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_BLUE);
    printf("■■■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_GRAY);
    printf("■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_SKYBLUE);
    printf("■■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_GRAY);
    printf("■");
    cursor_pos(x, y++);
    printf("■■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_BLUE);
    printf("■■■■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_GRAY);
    printf("■■");
    cursor_pos(x, y++);
    printf("■■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_BLUE);
    printf("■■■■■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_GRAY);
    printf("■");
    cursor_pos(x, y++);
    printf("■■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_BLUE);
    printf("■■■■■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_GRAY);
    printf("■");
    cursor_pos(x + 2, y++);
    printf("■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_BLUE);
    printf("■■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_GRAY);
    printf("■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_BLUE);
    printf("■■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_GRAY);
    printf("■");
    cursor_pos(x + 2, y++);
    printf("■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_BLUE);
    printf("■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_GRAY);
    printf("■  ■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_BLUE);
    printf("■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_GRAY);
    printf("■");
    cursor_pos(x + 3, y++);
    printf("■■    ■■");
    //system("pause");
}

void Rock(int posX)
{
    int x = posX, y = 30;
    //system("cls");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_GRAY);
    cursor_pos(x + 2, y++);
    printf("■■");
    cursor_pos(x, y++);
    printf("■■■■");
    cursor_pos(x, y++);
    printf("■■■■");
    cursor_pos(x, y++);
    printf("■■■■");
    //system("pause");
}

void Tree(int posX)
{
    int x = posX, y = 25;
    //system("cls");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_GREEN);
    cursor_pos(x + 3, y++);
    printf("■■");
    cursor_pos(x + 2, y++);
    printf("■■■");
    cursor_pos(x + 1, y++);
    printf("■■■■");
    cursor_pos(x, y++);
    printf("■■■■■");
    cursor_pos(x, y++);
    printf("■■■■■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DAKR_YELLOW);
    cursor_pos(x + 2, y++);
    printf("■■");
    cursor_pos(x + 2, y++);
    printf("■■");
    cursor_pos(x + 2, y++);
    printf("■■");
    //system("pause");
}

void Bird(int posX)
{
    int x = posX, y = 5;
    //system("cls");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), GRAY);
    cursor_pos(x + 2, y++);
    printf("■        ■");
    cursor_pos(x, y++);
    printf("■  ■    ■  ■");
    cursor_pos(x + 6, y++);
    printf("■■");
    cursor_pos(x + 6, y++);
    printf("■■");
    //system("pause");
}

void GameOver()
{
    int x = 37, y = 10;
    system("cls");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);

    //GAME
    cursor_pos(x, y++);
    printf("■■■■      ■■      ■      ■    ■■■■");
    cursor_pos(x, y++);
    printf("■          ■    ■    ■■  ■■    ■");
    cursor_pos(x, y++);
    printf("■  ■■    ■■■■    ■  ■  ■    ■■■■");
    cursor_pos(x, y++);
    printf("■    ■    ■    ■    ■      ■    ■");
    cursor_pos(x, y++);
    printf("■■■■    ■    ■    ■      ■    ■■■■");

    //OVER
    y += 2;
    cursor_pos(x, y++);
    printf("■■■■    ■      ■    ■■■■    ■■■");
    cursor_pos(x, y++);
    printf("■    ■     ■    ■     ■          ■    ■");
    cursor_pos(x, y++);
    printf("■    ■      ■  ■      ■■■■    ■■■");
    cursor_pos(x, y++);
    printf("■    ■       ■■       ■          ■   ■");
    cursor_pos(x, y++);
    printf("■■■■        ■        ■■■■    ■    ■");
    printf("\n\n\n\n\n\n\n\n\n\n\n");
    system("pause");
}

void ShowScore(uint score)
{
    cursor_pos(0, 0);
    printf("SCORE = %u", score);
}

int Playing()
{
    time_t start = time(NULL), finish = time(NULL);
    time(&start);
    int flowTime;
    char jump_type = 0; //no jump state
    int jump_size = 8, jump_cnt = 0;
    bool down = false;

    // 장애물 종류 선정을 위한 난수 생성
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(0, 2);
    
    //장애물 관련 변수
    int huddle_speed = 2, huddle_type = 0;

    //스타별 관련 변수
    int isStar = 0;
    
    //system("cls");
    while (1) {
        system("cls");
        int key = input_key_dir();

        //30초마다 장애물 속도 증가
        time_t cur = time(NULL);
        time(&cur);
        if (((int)difftime(cur, start)) > (huddle_speed-1) * 30) huddle_speed++;

        if (key == UP) {
            jump_cnt = 0;

            if (jump_type == 0)
                jump_type = 1;

            else if (jump_type == 1) {
                jump_type = 2;
                down = false;
            }
        }

        if (jump_type) {
            jump_cnt++;

            if (down)
                playerInfo[playerNum].set_Ypos(playerInfo[playerNum].get_Ypos() + 1);
            else
                playerInfo[playerNum].set_Ypos(playerInfo[playerNum].get_Ypos() - 1);

            if (jump_cnt == jump_size) {
                if (!down) {
                    Character(playerInfo[playerNum]);
                    down = true;
                    jump_cnt = 0;
                }
            }

            if (playerInfo[playerNum].get_Ypos() == START_POS_Y) {
                jump_type = 0;
                down = false;
            }
        }

        //장애물 위치 선정
        uint cur_x = huddleInfo[huddle_type].get_Xpos();
        huddleInfo[huddle_type].set_Xpos(cur_x - huddle_speed);

        //장애물 타입 교체
        if (huddleInfo[huddle_type].get_Xpos() < 0)
        {
            huddle_type = dis(gen);
            huddleInfo[huddle_type].set_Xpos(HUDDLE_START);
        }

        //현재 점수 출력
        ShowScore((int)difftime(cur, start));
        //캐릭터 그리기
        Character(playerInfo[playerNum]);

        //게임오버 판정
        if (check_fail(playerNum, huddle_type, isStar)) break;

        //장애물 그리기
        switch (huddle_type) {
        case 0:
            Rock(huddleInfo[huddle_type].get_Xpos()); break;
        case 1:
            Tree(huddleInfo[huddle_type].get_Xpos()); break;
        case 2:
            Bird(huddleInfo[huddle_type].get_Xpos()); break;
        }
    }
    //점수 설정
    time(&finish);
    flowTime = (int)difftime(finish, start);
    playerInfo[playerNum].set_score(flowTime);
    return playerInfo[playerNum].get_score();
}

int main()
{
    system("mode con:cols=120 lines=35"); //가로, 세로
    //SetConsoleTitle("Team10  Google Dinosaurs Game Plus Version");
    get_scores();
    
    while (1)
    {
        int gameScore = 0;
        init_huddle();
        init_players();
        playerNum = 0;

        switch (MainMenu()) {
        case 0: GameStart(); Count3sec(); gameScore = Playing(); GameOver(); break;
        case 1: Score(); break;
        case 2: system("cls"); set_scores(); return 0;
        }
    }
    return 0;
}
