#include <iostream>
#include <stdio.h>
#include <fstream>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <random>

// 방향키
#define UP					72
#define DOWN				80
#define LEFT				75
#define RIGHT				77

// Player
#define START_POS_X			5
#define START_POS_Y			22
#define SIZE_X				8
#define SIZE_Y				11
#define MAX_HP				3

// Bullet
#define BULLET_START_X		START_POS_X + 18

// Huddle
#define HUDDLE_TYPE			3
#define HUDDLE_START_X		104
#define ROCK_START_Y		29
#define ROCK_SIZE_X			4
#define ROCK_SIZE_Y			4
#define TREE_START_Y		25
#define TREE_SIZE_X			5
#define TREE_SIZE_Y			8
#define BIRD_TYPE			2
#define BIRD_START_Y0		7
#define BIRD_START_Y1		12
#define BIRD_START_Y2		17
#define BIRD_SIZE_X			8
#define BIRD_SIZE_Y			4

// Monster
#define MONSTER_START_X		95
#define MONSTER_BOUND_UP	3
#define MONSTER_BOUND_DOWN	18
#define MONSTER_SIZE		12

// Item
#define ITEM_TYPE			1
#define ITEM_START_X		106
#define ITEM_BOUND_UP		8
#define ITEM_BOUND_DOWN		25
#define HEART_SIZE_X		7
#define HEART_SIZE_Y		5
#define STAR_SIZE			5		

// Game Setting
#define SPEED_UPDATE		35
#define ITEM_UPDATE			15
#define MONSTER_UPDATE		30
#define STAR_MAINTAIN		7
#define BULLET_SPEED		2
#define WAIT				300
#define GHOST_SCORE			10
#define JUMP_SIZE			8

using namespace std;
typedef unsigned int uint;

/*----------자료형 영역----------*/
enum {
	BLACK,
	DARK_BLUE,
	DARK_GREEN,
	DARK_SKYBLUE,
	DARK_RED,
	DARK_VOILET,
	DARK_YELLOW,
	GRAY,
	DARK_GRAY,
	BLUE,
	GREEN,
	SKYBLUE,
	RED,
	VIOLET,
	YELLOW,
	WHITE,
};

typedef struct _position {
	int x;
	int y;
}P;

class Basis {
	protected:
		P pos;
	public:
		int get_Xpos() { return pos.x; }
		int get_Ypos() { return pos.y; }
		void set_Xpos(int val) { pos.x = val; }
		void set_Ypos(int val) { pos.y = val; }
		void reset(int X, int Y) { pos.x = X; pos.y = Y; }
};

class Player : public Basis {
	private:
		uint score;
		uint hp;
	public:
		Player() : score(0), hp(MAX_HP) { Basis::reset(START_POS_X, START_POS_Y); }
		uint get_score() { return score; }
		uint get_hp() { return hp; }
		void set_score(uint val) { score = val; }
		void set_hp(uint val) { hp = val; }
		friend void Character(Player& myplay, bool status);
		void reset() {
			Basis::reset(START_POS_X, START_POS_Y);
			hp = MAX_HP;
		}
};

class Bullet : public Basis {
	public:
		Bullet() { Basis::reset(BULLET_START_X, 0); }
		void reset() { Basis::reset(BULLET_START_X, 0); }
};

class Huddle : public Basis {
	public:
		Huddle() { Basis::reset(HUDDLE_START_X, 0); }
		void reset() { Basis::reset(HUDDLE_START_X, 0); }
};

class Monster : public Basis {
	private:
		bool isDead;
	public:
		Monster() : isDead(FALSE) { Basis::reset(MONSTER_START_X, MONSTER_BOUND_UP); }
		bool get_dead() { return isDead; }
		void set_dead(bool val) { isDead = val; }
		void reset() {
			Basis::reset(MONSTER_START_X, MONSTER_BOUND_UP);
			isDead = FALSE;
		}
};

class Item : public Basis {
	public:
		Item() { Basis::reset(ITEM_START_X, ITEM_BOUND_UP); }
		void reset() { Basis::reset(ITEM_START_X, ITEM_BOUND_UP); }
};

/*---------전역 변수 초기화---------*/
int playerNum;
Player playerInfo[3];
Bullet bulletInfo;
Huddle huddleInfo[3];	// 0 : rock, 1 : tree, 2 : bird
Monster monsterInfo;
Item itemInfo[3];		// 0 :heart, 1 :star, 2 : blank / disappear

/*----------함수 영역 시작----------*/
void init_class()
{
	for (int i = 0; i < 3; i++) {
		playerInfo[i].reset();
		huddleInfo[i].reset();
		itemInfo[i].reset();
	}
	bulletInfo.reset();
	monsterInfo.reset();
}

void get_score_file()
{
	uint num[3] = { 0 };

	ifstream in("score.txt");
	if (!in.is_open()) return;

	in >> num[0] >> num[1] >> num[2];
	for (int i = 0; i < 3; i++)
		playerInfo[i].set_score(num[i]);
	in.close();

	return;
}

void set_score_file()
{
	ofstream out("score.txt");

	for (int i = 0; i < 3; i++)
		out << playerInfo[i].get_score() << " ";
	out.close();

	return;
}

template <typename T>
inline T randomNum(T min, T max)
{
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<T> dist(min, max);

	return dist(gen);
}

inline void cursor_pos(int x, int y)
{
	COORD p;
	p.X = x;
	p.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), p);
}

int input_key_dir()
{
	if (_kbhit())
	{
		char c = _getch();
		if (c == -32) return _getch();
	}
	return 0;
}

int MainMenu()
{
	int x = 44, y = 9;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);

	system("cls");
	cursor_pos(x, y);
	printf("=================================");
	cursor_pos(x + 4, y + 3);
	printf("▶  G A M E  S T A R T");
	cursor_pos(x + 8, y + 6);
	printf("H O W  T O  P L A Y");
	cursor_pos(x + 8, y + 9);
	printf("S C O R E");
	cursor_pos(x + 8, y + 12);
	printf("E X I T");
	cursor_pos(x, y + 15);
	printf("=================================");
	printf("\n\n\n\n\n\n\n\n\n");

	int menu = 0;
	bool isSelect = FALSE;

	while (1) {
		int key = input_key_dir();

		if (key == DOWN) menu++;
		else if (key == UP) menu += 3;
		else if (key == RIGHT) isSelect = TRUE;
		else if (key == 0) continue;

		menu %= 4;

		if (menu == 0) {
			system("cls");
			cursor_pos(x, y);
			printf("=================================");
			cursor_pos(x + 4, y + 3);
			printf("▶  G A M E  S T A R T");
			cursor_pos(x + 8, y + 6);
			printf("H O W  T O  P L A Y");
			cursor_pos(x + 8, y + 9);
			printf("S C O R E");
			cursor_pos(x + 8, y + 12);
			printf("E X I T");
			cursor_pos(x, y + 15);
			printf("=================================");
		}
		else if (menu == 1) {
			system("cls");
			cursor_pos(x, y);
			printf("=================================");
			cursor_pos(x + 8, y + 3);
			printf("G A M E  S T A R T");
			cursor_pos(x + 4, y + 6);
			printf("▶  H O W  T O  P L A Y");
			cursor_pos(x + 8, y + 9);
			printf("S C O R E");
			cursor_pos(x + 8, y + 12);
			printf("E X I T");
			cursor_pos(x, y + 15);
			printf("=================================");
		}
		else if (menu == 2) {
			system("cls");
			cursor_pos(x, y);
			printf("=================================");
			cursor_pos(x + 8, y + 3);
			printf("G A M E  S T A R T");
			cursor_pos(x + 8, y + 6);
			printf("H O W  T O  P L A Y");
			cursor_pos(x + 4, y + 9);
			printf("▶  S C O R E");
			cursor_pos(x + 8, y + 12);
			printf("E X I T");
			cursor_pos(x, y + 15);
			printf("=================================");
		}
		else if (menu == 3) {
			system("cls");
			cursor_pos(x, y);
			printf("=================================");
			cursor_pos(x + 8, y + 3);
			printf("G A M E  S T A R T");
			cursor_pos(x + 8, y + 6);
			printf("H O W  T O  P L A Y");
			cursor_pos(x + 8, y + 9);
			printf("S C O R E");
			cursor_pos(x + 4, y + 12);
			printf("▶  E X I T");
			cursor_pos(x, y + 15);
			printf("=================================");
		}
		printf("\n\n\n\n\n\n\n\n\n");

		if (isSelect) {
			if (menu == 0) {
				system("cls");
				cursor_pos(x, y);
				printf("=================================");
				cursor_pos(x + 4, y + 3);
				printf("▷  G A M E  S T A R T");
				cursor_pos(x + 8, y + 6);
				printf("H O W  T O  P L A Y");
				cursor_pos(x + 8, y + 9);
				printf("S C O R E");
				cursor_pos(x + 8, y + 12);
				printf("E X I T");
				cursor_pos(x, y + 15);
				printf("=================================");
			}
			else if (menu == 1) {
				system("cls");
				cursor_pos(x, y);
				printf("=================================");
				cursor_pos(x + 8, y + 3);
				printf("G A M E  S T A R T");
				cursor_pos(x + 4, y + 6);
				printf("▷  H O W  T O  P L A Y");
				cursor_pos(x + 8, y + 9);
				printf("S C O R E");
				cursor_pos(x + 8, y + 12);
				printf("E X I T");
				cursor_pos(x, y + 15);
				printf("=================================");
			}
			else if (menu == 2) {
				system("cls");
				cursor_pos(x, y);
				printf("=================================");
				cursor_pos(x + 8, y + 3);
				printf("G A M E  S T A R T");
				cursor_pos(x + 8, y + 6);
				printf("H O W  T O  P L A Y");
				cursor_pos(x + 4, y + 9);
				printf("▷  S C O R E");
				cursor_pos(x + 8, y + 12);
				printf("E X I T");
				cursor_pos(x, y + 15);
				printf("=================================");
			}
			else if (menu == 3) {
				system("cls");
				cursor_pos(x, y);
				printf("=================================");
				cursor_pos(x + 8, y + 3);
				printf("G A M E  S T A R T");
				cursor_pos(x + 8, y + 6);
				printf("H O W  T O  P L A Y");
				cursor_pos(x + 8, y + 9);
				printf("S C O R E");
				cursor_pos(x + 4, y + 12);
				printf("▷  E X I T");
				cursor_pos(x, y + 15);
				printf("=================================");
			}
			printf("\n\n\n\n\n\n\n\n\n");
			Sleep(WAIT);

			break;
		}
	}

	return menu;
}

int SelectPlayer()
{
	int x = 44, y = 11;

	system("cls");
	cursor_pos(x, y);
	printf("=================================");
	cursor_pos(x + 4, y + 3);
	printf("▶  P L A Y E R  1");
	cursor_pos(x + 8, y + 6);
	printf("P L A Y E R  2");
	cursor_pos(x + 8, y + 9);
	printf("P L A Y E R  3");
	cursor_pos(x, y + 12);
	printf("=================================");
	printf("\n\n\n\n\n\n\n\n\n\n");

	int select = 0;

	while (1) {
		int key = input_key_dir();

		if (key == DOWN) playerNum++;
		else if (key == UP) playerNum += 2;
		else if (key == RIGHT) select = 1;
		else if (key == LEFT) select = 2;
		else if (key == 0) continue;

		playerNum %= 3;

		if (playerNum == 0) {
			system("cls");
			cursor_pos(x, y);
			printf("=================================");
			cursor_pos(x + 4, y + 3);
			printf("▶  P L A Y E R  1");
			cursor_pos(x + 8, y + 6);
			printf("P L A Y E R  2");
			cursor_pos(x + 8, y + 9);
			printf("P L A Y E R  3");
			cursor_pos(x, y + 12);
			printf("=================================");
		}
		else if (playerNum == 1) {
			system("cls");
			cursor_pos(x, y);
			printf("=================================");
			cursor_pos(x + 8, y + 3);
			printf("P L A Y E R  1");
			cursor_pos(x + 4, y + 6);
			printf("▶  P L A Y E R  2");
			cursor_pos(x + 8, y + 9);
			printf("P L A Y E R  3");
			cursor_pos(x, y + 12);
			printf("=================================");
		}
		else if (playerNum == 2) {
			system("cls");
			cursor_pos(x, y);
			printf("=================================");
			cursor_pos(x + 8, y + 3);
			printf("P L A Y E R  1");
			cursor_pos(x + 8, y + 6);
			printf("P L A Y E R  2");
			cursor_pos(x + 4, y + 9);
			printf("▶  P L A Y E R  3");
			cursor_pos(x, y + 12);
			printf("=================================");
		}
		printf("\n\n\n\n\n\n\n\n\n\n");

		if (select == 1) {
			if (playerNum == 0) {
				system("cls");
				cursor_pos(x, y);
				printf("=================================");
				cursor_pos(x + 4, y + 3);
				printf("▷  P L A Y E R  1");
				cursor_pos(x + 8, y + 6);
				printf("P L A Y E R  2");
				cursor_pos(x + 8, y + 9);
				printf("P L A Y E R  3");
				cursor_pos(x, y + 12);
				printf("=================================");
			}
			else if (playerNum == 1) {
				system("cls");
				cursor_pos(x, y);
				printf("=================================");
				cursor_pos(x + 8, y + 3);
				printf("P L A Y E R  1");
				cursor_pos(x + 4, y + 6);
				printf("▷  P L A Y E R  2");
				cursor_pos(x + 8, y + 9);
				printf("P L A Y E R  3");
				cursor_pos(x, y + 12);
				printf("=================================");
			}
			else if (playerNum == 2) {
				system("cls");
				cursor_pos(x, y);
				printf("=================================");
				cursor_pos(x + 8, y + 3);
				printf("P L A Y E R  1");
				cursor_pos(x + 8, y + 6);
				printf("P L A Y E R  2");
				cursor_pos(x + 4, y + 9);
				printf("▷  P L A Y E R  3");
				cursor_pos(x, y + 12);
				printf("=================================");
			}
			printf("\n\n\n\n\n\n\n\n\n\n");
			Sleep(WAIT);
			break;
		}
		else if (select == 2) break;
	}

	return select;
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
	printf("\n\n\n\n\n\n\n\n\n\n\n\n");
	Sleep(1000);

	y = 11;
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
	printf("■");
	cursor_pos(x, y++);
	printf("■");
	cursor_pos(x, y++);
	printf("■");
	cursor_pos(x, y++);
	printf("■");
	cursor_pos(x, y++);
	printf("■■■■■■");
	printf("\n\n\n\n\n\n\n\n\n\n\n\n");
	Sleep(1000);

	x = 55, y = 11;
	system("cls");
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
	printf("\n\n\n\n\n\n\n\n\n\n\n\n");
	Sleep(1000);
}

void Manual()
{
	int x = 36, y = 7;

	system("cls");
	cursor_pos(x - 1, y++);
	printf("=============== H O W  T O  P L A Y ===============");
	y++;
	cursor_pos(x + 12, y++);
	printf("↑ : JUMP    → : ATTACK");
	y++; y++;
	cursor_pos(x, y++);
	printf("Avoid various obstacles that are approaching!");
	cursor_pos(x, y++);
	printf("When you bump into obstacles, HP is reduced.");
	cursor_pos(x, y++);
	printf("If HP reaches 0, the game is over.");
	y++;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), VIOLET);
	cursor_pos(x, y++);
	printf("Get rid of the ghost using the attack!");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
	cursor_pos(x, y++);
	printf("If you bump into a ghost, the game ends right away.");
	cursor_pos(x, y++);
	printf("But if you catch a ghost, you'll get a bonus point.");
	y++;
	cursor_pos(x, y++);
	printf("There are items that help you.");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), RED);
	cursor_pos(x, y++);
	printf("Heart♥ increases your HP. (Maximum HP is 3)");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), YELLOW);
	cursor_pos(x, y++);
	printf("Star★ make you invincible.");
	y++;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
	cursor_pos(x, y++);
	printf("Survive as long as possible.");
	cursor_pos(x, y++);
	printf("GOOD LUCK!");
	cursor_pos(x, y + 2);
	printf("===================================================");
	printf("\n\n\n\n\n");

	while (1) {
		if (input_key_dir() == LEFT)
			break;
	}
}

void Score()
{
	string name[] = { "Player1", "Player2", "Player3" };
	int ranking[3] = { 0, 1, 2 };
	int x = 44, y = 10;

	int score0 = playerInfo[0].get_score();
	int score1 = playerInfo[1].get_score();
	int score2 = playerInfo[2].get_score();

	if (score0 >= score1) {
		if (score1 >= score2) {
			ranking[0] = 0; ranking[1] = 1; ranking[2] = 2;
		}
		else if (score0 >= score2) {
			ranking[0] = 0; ranking[1] = 2; ranking[2] = 1;
		}
		else {
			ranking[0] = 2; ranking[1] = 0; ranking[2] = 1;
		}
	}
	else {
		if (score0 >= score2) {
			ranking[0] = 1; ranking[1] = 0; ranking[2] = 2;
		}
		else if (score1 >= score2) {
			ranking[0] = 1; ranking[1] = 2; ranking[2] = 0;
		}
		else {
			ranking[0] = 2; ranking[1] = 1; ranking[2] = 0;
		}
	}

	system("cls");
	cursor_pos(x, y);
	printf("=================================");
	cursor_pos(x + 5, y + 3);
	cout << "1st    " << name[ranking[0]] << "   " << playerInfo[ranking[0]].get_score();
	cursor_pos(x + 5, y + 6);
	cout << "2nd    " << name[ranking[1]] << "   " << playerInfo[ranking[1]].get_score();
	cursor_pos(x + 5, y + 9);
	cout << "3rd    " << name[ranking[2]] << "   " << playerInfo[ranking[2]].get_score();
	cursor_pos(x, y + 12);
	printf("=================================");
	printf("\n\n\n\n\n\n\n\n\n\n\n");

	while (1) {
		if (input_key_dir() == LEFT)
			break;
	}
}

void Character(Player& myplay, bool status)
{
	int x = myplay.pos.x, y = myplay.pos.y;

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_GRAY);
	cursor_pos(x + 6, y++);
	printf("■■■");
	cursor_pos(x + 4, y++);
	printf("■");
	if (status)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), YELLOW);
	else
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_BLUE);
	printf("■■■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_GRAY);
	printf("■");
	cursor_pos(x + 2, y++);
	printf("■");
	if (status)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), YELLOW);
	else
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_BLUE);
	printf("■■■■■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_GRAY);
	printf("■");
	cursor_pos(x + 2, y++);
	printf("■");
	if (status)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), YELLOW);
	else
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_BLUE);
	printf("■■■■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_GRAY);
	printf("■■");
	cursor_pos(x, y++);
	printf("■■");
	if (status)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), YELLOW);
	else
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_BLUE);
	printf("■■■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_GRAY);
	printf("■");
	if (status)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), SKYBLUE);
	else
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_SKYBLUE);
	printf("■■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_GRAY);
	printf("■");
	cursor_pos(x, y++);
	printf("■■");
	if (status)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), YELLOW);
	else
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_BLUE);
	printf("■■■■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_GRAY);
	printf("■■");
	cursor_pos(x, y++);
	printf("■■");
	if (status)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), YELLOW);
	else
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_BLUE);
	printf("■■■■■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_GRAY);
	printf("■");
	cursor_pos(x, y++);
	printf("■■");
	if (status)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), YELLOW);
	else
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_BLUE);
	printf("■■■■■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_GRAY);
	printf("■");
	cursor_pos(x + 2, y++);
	printf("■");
	if (status)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), YELLOW);
	else
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_BLUE);
	printf("■■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_GRAY);
	printf("■");
	if (status)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), YELLOW);
	else
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_BLUE);
	printf("■■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_GRAY);
	printf("■");
	cursor_pos(x + 2, y++);
	printf("■");
	if (status)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), YELLOW);
	else
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_BLUE);
	printf("■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_GRAY);
	printf("■  ■");
	if (status)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), YELLOW);
	else
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_BLUE);
	printf("■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_GRAY);
	printf("■");
	cursor_pos(x + 3, y++);
	printf("■■    ■■");
}

void Dot(int posX) {
	int x = posX, y = bulletInfo.get_Ypos();

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_RED);
	cursor_pos(x, y);
	printf("■▶");
}

void Rock(int posX)
{
	int x = posX, y = ROCK_START_Y;

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_GRAY);
	cursor_pos(x + 2, y++);
	printf("■■");
	cursor_pos(x, y++);
	printf("■■■■");
	cursor_pos(x, y++);
	printf("■■■■");
	cursor_pos(x, y++);
	printf("■■■■");
}

void Tree(int posX)
{
	int x = posX, y = TREE_START_Y;

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
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_YELLOW);
	cursor_pos(x + 3, y++);
	printf("■■");
	cursor_pos(x + 3, y++);
	printf("■■");
	cursor_pos(x + 3, y++);
	printf("■■");
}

void Bird(int posX, int posY)
{
	int x = posX, y = posY;

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), GRAY);
	cursor_pos(x + 2, y++);
	printf("■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BLACK);
	printf("■■■■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), GRAY);
	printf("■");
	cursor_pos(x, y++);
	printf("■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BLACK);
	printf("■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), GRAY);
	printf("■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BLACK);
	printf("■■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), GRAY);
	printf("■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BLACK);
	printf("■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), GRAY);
	printf("■");
	cursor_pos(x + 6, y++);
	printf("■■");
	cursor_pos(x + 6, y++);
	printf("■■");
}

void Ghost(int posX, int posY)
{
	int x = posX, y = posY;

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), VIOLET);
	cursor_pos(x + 8, y++);
	printf("■■■■");
	cursor_pos(x + 4, y++);
	printf("■■■■■■■■");
	cursor_pos(x + 2, y++);
	printf("■■■■■■■■■■");
	cursor_pos(x, y++);
	printf("■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
	printf("■■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), VIOLET);
	printf("■■■■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
	printf("■■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), VIOLET);
	printf("■■■");
	cursor_pos(x, y++);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
	printf("■■■■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), VIOLET);
	printf("■■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
	printf("■■■■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), VIOLET);
	printf("■■");
	cursor_pos(x, y++);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_BLUE);
	printf("■■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
	printf("■■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), VIOLET);
	printf("■■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_BLUE);
	printf("■■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
	printf("■■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), VIOLET);
	printf("■■");
	cursor_pos(x, y++);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_BLUE);
	printf("■■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
	printf("■■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), VIOLET);
	printf("■■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DARK_BLUE);
	printf("■■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
	printf("■■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), VIOLET);
	printf("■■");
	cursor_pos(x, y++);
	printf("■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
	printf("■■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), VIOLET);
	printf("■■■■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
	printf("■■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), VIOLET);
	printf("■■■");
	cursor_pos(x, y++);
	printf("■■■■■■■■■■■■");
	cursor_pos(x, y++);
	printf("■■■■■■■■■■■■");
	cursor_pos(x, y++);
	printf("■■■  ■■■■  ■■■");
	cursor_pos(x, y++);
	printf("■■      ■■      ■■");
}

void Heart(int posX, int posY)
{
	int x = posX, y = posY;

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), RED);
	cursor_pos(x + 2, y++);
	printf("■■  ■■");
	cursor_pos(x, y++);
	printf("■■■■■■■");
	cursor_pos(x + 2, y++);
	printf("■■■■■");
	cursor_pos(x + 4, y++);
	printf("■■■");
	cursor_pos(x + 6, y++);
	printf("■");
}

void Star(int posX, int posY)
{
	int x = posX, y = posY;

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), YELLOW);
	cursor_pos(x + 4, y++);
	printf("■");
	cursor_pos(x + 4, y++);
	printf("■");
	cursor_pos(x, y++);
	printf("■■■■■");
	cursor_pos(x + 3, y++);
	printf("■■");
	cursor_pos(x + 1, y++);
	printf("■    ■");
}

void GameOver(uint score)
{
	int x = 37, y = 10;

	system("cls");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);

	// GAME
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

	// OVER
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

	cursor_pos(x + 15, y + 3);
	printf("S C O R E  %u", score);

	cursor_pos(x + 5, y + 9);
	system("pause");
}

void timeHP(uint flowtime)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
	cursor_pos(2, 1);
	printf("TIME  %u min %u sec", flowtime / 60, flowtime % 60);
	// printf("TIME  %u sec", flowtime);

	cursor_pos(102, 1);
	printf("HEART  ");
	for (uint i = 0; i < playerInfo[playerNum].get_hp(); i++) printf("♥ ");
}

bool check_attack()
{
	int bx = bulletInfo.get_Xpos(), by = bulletInfo.get_Ypos();
	int mx = monsterInfo.get_Xpos(), my = monsterInfo.get_Ypos();

	for (int i = bx; i < bx + 2; i++) {
		for (int nx = mx; nx < mx + MONSTER_SIZE; nx++) {
			for (int ny = my; ny < my + MONSTER_SIZE; ny++) {
				if (i == nx && by == ny) return TRUE;
			}
		}
	}
	return FALSE;
}

bool check_hunt(bool status)
{
	if (status) return FALSE;

	int px = playerInfo[playerNum].get_Xpos(), py = playerInfo[playerNum].get_Ypos();
	int mx = monsterInfo.get_Xpos(), my = monsterInfo.get_Ypos();

	for (int i = px; i < px + SIZE_X; i++) {
		for (int j = py; j < py + SIZE_Y; j++) {
			for (int nx = mx; nx < mx + MONSTER_SIZE; nx++) {
				for (int ny = my; ny < my + MONSTER_SIZE; ny++) {
					if (i == nx && j == ny) return TRUE;
				}
			}
		}
	}
	return FALSE;
}

bool check_fail(uint huddle, bool status)
{
	if (status) return FALSE;

	int px = playerInfo[playerNum].get_Xpos(), py = playerInfo[playerNum].get_Ypos();
	int hx = huddleInfo[huddle].get_Xpos(), hy;

	if (huddle == 0) {
		// rock
		hy = ROCK_START_Y;
		for (int i = px; i < px + SIZE_X; i++) {
			for (int j = py; j < py + SIZE_Y; j++) {
				for (int nx = hx; nx < hx + ROCK_SIZE_X; nx++) {
					for (int ny = hy; ny < hy + ROCK_SIZE_Y; ny++) {
						if (i == nx && j == ny) return TRUE;
					}
				}
			}
		}
	}
	else if (huddle == 1) {
		// tree
		hy = TREE_START_Y;
		for (int i = px; i < px + SIZE_X; i++) {
			for (int j = py; j < py + SIZE_Y; j++) {
				for (int nx = hx; nx < hx + TREE_SIZE_X; nx++) {
					for (int ny = hy; ny < hy + TREE_SIZE_Y; ny++) {
						if (i == nx && j == ny) return TRUE;
					}
				}
			}
		}

	}
	else if (huddle == 2) {
		// bird
		hy = huddleInfo[huddle].get_Ypos();
		for (int i = px; i < px + SIZE_X; i++) {
			for (int j = py; j < py + SIZE_Y; j++) {
				for (int nx = hx; nx < hx + BIRD_SIZE_X; nx++) {
					for (int ny = hy; ny < hy + BIRD_SIZE_Y; ny++) {
						if (i == nx && j == ny) return TRUE;
					}
				}
			}
		}
	}
	else {
		// rock & bird
		hx = huddleInfo[0].get_Xpos();
		hy = ROCK_START_Y;
		int bx = huddleInfo[2].get_Xpos(), by = huddleInfo[2].get_Ypos();
		for (int i = px; i < px + SIZE_X; i++) {
			for (int j = py; j < py + SIZE_Y; j++) {
				// rock
				for (int nx = hx; nx < hx + ROCK_SIZE_X; nx++) {
					for (int ny = hy; ny < hy + ROCK_SIZE_Y; ny++) {
						if (i == nx && j == ny) return TRUE;
					}
				}
				// bird
				for (int nx = bx; nx < bx + BIRD_SIZE_X; nx++) {
					for (int ny = by; ny < by + BIRD_SIZE_Y; ny++) {
						if (i == nx && j == ny) return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

bool check_success(uint item)
{
	int px = playerInfo[playerNum].get_Xpos(), py = playerInfo[playerNum].get_Ypos();
	int ix = itemInfo[item].get_Xpos(), iy = itemInfo[item].get_Ypos();

	if (item == 0) {
		// heart
		for (int i = px; i < px + SIZE_X; i++) {
			for (int j = py; j < py + SIZE_Y; j++) {
				for (int nx = ix; nx < ix + HEART_SIZE_X; nx++) {
					for (int ny = iy; ny < iy + HEART_SIZE_Y; ny++) {
						if (i == nx && j == ny) return TRUE;
					}
				}
			}
		}
	}
	else if (item == 1) {
		// star
		for (int i = px; i < px + SIZE_X; i++) {
			for (int j = py; j < py + SIZE_Y; j++) {
				for (int nx = ix; nx < ix + STAR_SIZE; nx++) {
					for (int ny = iy; ny < iy + STAR_SIZE; ny++) {
						if (i == nx && j == ny) return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

uint Playing()
{
	// 시간 관련 변수
	time_t start = time(NULL);
	time_t middle = time(NULL);
	time_t finish = time(NULL);
	time_t start_star = time(NULL);
	time(&start);
	time(&middle);
	time(&finish);
	time(&start_star);

	// 점수 관련 변수
	int num_ghost = 0;

	// 점프 관련 변수
	bool isDown = FALSE;
	int jump_cnt = 0, jump_type = 0;

	// 총알 관련 변수
	bool bullet_check = FALSE;

	// 장애물 관련 변수
	bool huddle_check = TRUE;
	int huddle_speed = 2, huddle_type = 0;

	// 몬스터 관련 변수
	int monster_speed = 2, monster_once = 0, monster_direction = 1;

	// 아이템 관련 변수
	bool isStar = FALSE, item_check = TRUE;
	int item_speed = 2, item_type = 0, item_once = 0;

	// 쓰레기 input 제거
	while (input_key_dir());

	while (1) {
		int key_dir = input_key_dir();

		/*---------플레이어---------*/
		// 플레이어 2단 점프
		if (key_dir == UP) {
			if (jump_type == 0)
				jump_type = 1;
			else if (jump_type == 1) {
				jump_type = 2;
				jump_cnt = 0;
				isDown = FALSE;
			}
		}

		// 플레이어 위치 선정
		if (jump_type) {
			if (isDown) playerInfo[playerNum].set_Ypos(playerInfo[playerNum].get_Ypos() + 1);
			else		playerInfo[playerNum].set_Ypos(playerInfo[playerNum].get_Ypos() - 1);
			
			jump_cnt++;

			if (jump_cnt == JUMP_SIZE) {
				if (!isDown) {
					isDown = TRUE;
					jump_cnt = 0;
				}
			}

			if (playerInfo[playerNum].get_Ypos() == START_POS_Y) {
				isDown = FALSE;
				jump_type = 0;
				jump_cnt = 0;
			}
		}
		/*--------------------------*/

		/*-----------총알-----------*/
		// 총알 발사 시 초기 위치 선정
		if (key_dir == RIGHT && !bullet_check) {
			bullet_check = TRUE;

			bulletInfo.set_Xpos(BULLET_START_X);
			bulletInfo.set_Ypos(playerInfo[playerNum].get_Ypos() + 4);
		}

		// 총알 이동 중 위치 선정
		if (bullet_check) {
			uint bullet_curx = bulletInfo.get_Xpos();

			bulletInfo.set_Xpos(bullet_curx + BULLET_SPEED);

			// 총알 공격이 성공했는지 판정
			if (((int)difftime(middle, start) > MONSTER_UPDATE) && !monsterInfo.get_dead()) {
				if (check_attack()) {
					bullet_check = FALSE;
					monsterInfo.set_dead(TRUE);
					num_ghost++;
				}
			}
		}
		if (bulletInfo.get_Xpos() > 116) bullet_check = FALSE;
		/*--------------------------*/

		/*----------장애물----------*/
		// 장애물 속도 증가
		middle = time(NULL);
		time(&middle);
		if ((int)difftime(middle, start) > (huddle_speed * SPEED_UPDATE)) huddle_speed++;

		// 장애물 위치 선정
		if (huddle_type == 3) {
			uint huddle_curx = huddleInfo[0].get_Xpos();
			huddleInfo[0].set_Xpos(huddle_curx - huddle_speed);
			huddleInfo[2].set_Xpos(huddle_curx - huddle_speed);
		}
		else {
			uint huddle_curx = huddleInfo[huddle_type].get_Xpos();
			huddleInfo[huddle_type].set_Xpos(huddle_curx - huddle_speed);
		}

		// 장애물 타입 교체
		if ((huddle_type == 3 && huddleInfo[0].get_Xpos() < 0) || (huddleInfo[huddle_type].get_Xpos() < 0)) {
			huddle_type = randomNum(0, HUDDLE_TYPE);
			huddle_check = TRUE;

			if (huddle_type == 3) {
				huddleInfo[0].set_Xpos(HUDDLE_START_X);
				huddleInfo[2].set_Xpos(HUDDLE_START_X);
				huddleInfo[2].set_Ypos(BIRD_START_Y0);
			}

			else {
				huddleInfo[huddle_type].set_Xpos(HUDDLE_START_X);
				if (huddle_type == 2) {
					switch (randomNum(0, BIRD_TYPE)) {
					case 0:
						huddleInfo[huddle_type].set_Ypos(BIRD_START_Y0);
						break;
					case 1:
						huddleInfo[huddle_type].set_Ypos(BIRD_START_Y1);
						break;
					case 2:
						huddleInfo[huddle_type].set_Ypos(BIRD_START_Y2);
						break;
					}
				}
			}
		}

		// 게임 오버 판정 (장애물)
		if (huddle_check && check_fail(huddle_type, isStar)) {
			huddle_check = FALSE;

			playerInfo[playerNum].set_hp(playerInfo[playerNum].get_hp() - 1);

			if (playerInfo[playerNum].get_hp() == 0) break;
		}
		/*--------------------------*/

		/*----------몬스터----------*/
		// 몬스터 위치 선정
		if ((int)difftime(middle, start) > MONSTER_UPDATE) {
			int monster_curx = monsterInfo.get_Xpos();
			int monster_cury = monsterInfo.get_Ypos();

			monsterInfo.set_Xpos(monster_curx - monster_speed);
			if (monster_cury <= MONSTER_BOUND_UP) monster_direction = 1;
			else if (monster_cury >= MONSTER_BOUND_DOWN) monster_direction = -1;
			monsterInfo.set_Ypos(monster_cury + monster_direction);

			if ((int)difftime(middle, start) % MONSTER_UPDATE == 0) monster_once++;
			if (monsterInfo.get_Xpos() < 0 && monster_once == 1) {
				monsterInfo.set_Xpos(MONSTER_START_X);
				monsterInfo.set_Ypos(randomNum(MONSTER_BOUND_UP, MONSTER_BOUND_DOWN));
				monsterInfo.set_dead(FALSE);
			}
			if ((int)difftime(middle, start) % (MONSTER_UPDATE + 1) == 0) monster_once = 0;
		}

		// 게임 오버 판정 (몬스터)
		if (((int)difftime(middle, start) > MONSTER_UPDATE) && (!monsterInfo.get_dead())) {
			if (check_hunt(isStar)) break;
		}
		/*--------------------------*/

		/*----------아이템----------*/
		// 아이템 위치 선정
		if ((int)difftime(middle, start) > ITEM_UPDATE) {
			int item_curx = itemInfo[item_type].get_Xpos();
			itemInfo[item_type].set_Xpos(item_curx - item_speed);

			// 아이템 타입 교체
			if ((int)difftime(middle, start) % ITEM_UPDATE == 0) item_once++;
			if ((itemInfo[item_type].get_Xpos() < 0) && (item_once == 1)) {
				item_check = TRUE;
				item_type = randomNum(0, ITEM_TYPE);

				itemInfo[item_type].set_Xpos(ITEM_START_X);
				itemInfo[item_type].set_Ypos(randomNum(ITEM_BOUND_UP, ITEM_BOUND_DOWN));

				if (item_type == 1) {
					start_star = time(NULL);
					time(&start_star);
				}
			}
			if ((int)difftime(middle, start) % (ITEM_UPDATE + 1) == 0) item_once = 0;
		}

		// 아이템 얻었는지 판정
		if (check_success(item_type) && item_check) {
			item_check = FALSE;

			if ((item_type == 0) && (playerInfo[playerNum].get_hp() < MAX_HP))
				playerInfo[playerNum].set_hp(playerInfo[playerNum].get_hp() + 1);
			else if (item_type == 1) isStar = TRUE;
			item_type = 2;
		}

		// 무적 상태 10초 유지
		if ((int)difftime(middle, start_star) > ((item_speed - 1) * STAR_MAINTAIN))
			isStar = FALSE;
		/*--------------------------*/

		/*---------화면 출력--------*/
		// 현재 플레이 시간, HP 출력
		timeHP((int)difftime(middle, start));

		// 플레이어 그리기
		Character(playerInfo[playerNum], isStar);

		// 아이템 그리기
		if ((int)difftime(middle, start) > ITEM_UPDATE) {
			if (itemInfo[item_type].get_Xpos() > 0) {
				switch (item_type) {
				case 0:
					Heart(itemInfo[item_type].get_Xpos(), itemInfo[item_type].get_Ypos()); break;
				case 1:
					Star(itemInfo[item_type].get_Xpos(), itemInfo[item_type].get_Ypos()); break;
				}
			}
		}

		// 총알 그리기
		if (bullet_check) Dot(bulletInfo.get_Xpos());

		// 몬스터 그리기
		if ((int)difftime(middle, start) > MONSTER_UPDATE) {
			if (!monsterInfo.get_dead() && monsterInfo.get_Xpos() > 0)
				Ghost(monsterInfo.get_Xpos(), monsterInfo.get_Ypos());
		}

		// 장애물 그리기
		switch (huddle_type) {
		case 0:
			Rock(huddleInfo[huddle_type].get_Xpos()); break;
		case 1:
			Tree(huddleInfo[huddle_type].get_Xpos()); break;
		case 2:
			Bird(huddleInfo[huddle_type].get_Xpos(), huddleInfo[huddle_type].get_Ypos()); break;
		case 3:
			Rock(huddleInfo[0].get_Xpos()); Bird(huddleInfo[2].get_Xpos(), huddleInfo[2].get_Ypos()); break;
		}

		// 화면 clear
		system("cls");
		/*--------------------------*/
	}

	// 점수 설정
	finish = time(NULL);
	time(&finish);
	uint total_score = (int)difftime(finish, start) + num_ghost * GHOST_SCORE;
	uint prev = playerInfo[playerNum].get_score();

	playerInfo[playerNum].set_score(total_score > prev ? total_score : prev);

	return total_score;
}

int main()
{
	system("mode con:cols=120 lines=35");	// 콘솔 창 가로, 세로

	get_score_file();

	while (1)
	{
		init_class();

		switch (MainMenu()) {
		case 0: {
			int select = SelectPlayer();

			if (select == 1) {
				Count3sec();				// 게임 시작 전 3초 count
				uint score = Playing();
				GameOver(score);
				set_score_file();			// output 파일 업데이트
				break;						// 첫 화면으로 이동
			}
			else if (select == 2) break;	// 첫 화면으로 이동
		}
		case 1: {
			Manual();
			break;							// 첫 화면으로 이동
		}
		case 2: {
			Score();
			break;							// 첫 화면으로 이동
		}
		case 3: {
			system("cls");					// 콘솔 창 clear
			set_score_file();				// output 파일 업데이트
			return 0;						// 종료
		}
		}
	}

	return 0;
}