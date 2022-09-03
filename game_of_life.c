#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define WIDTH 82
#define HEIGHT 27

#define WIDTHMATRIX 80
#define HEIGHTMATRIX 25

#define STARTINDEX 1
#define MAXMAP 5

#define INITSPEED 100000
#define MINSPEED 10000
#define MAXSPEED 1000000

void show_field(char pattern_matrix[HEIGHTMATRIX][WIDTHMATRIX]);

void fill_pattern_matrix(char pattern_matrix[HEIGHTMATRIX][WIDTHMATRIX]);

int neighbour_check(char pattern_matrix[HEIGHTMATRIX][WIDTHMATRIX], int y,
                    int x);

void change_matrix(char pattern_matrix[HEIGHTMATRIX][WIDTHMATRIX]);

int lives_count(char pattern_matrix[HEIGHTMATRIX][WIDTHMATRIX]);

int menu();

void read_file(FILE *ptr);

void patterns_list();

void printPosition(int posIntro);

void init_wins(WINDOW **wins, int n);

int game_speed(int c);

int main() {
  char pattern_matrix[HEIGHTMATRIX][WIDTHMATRIX];
  int k = 0, lives;
  FILE *f;
  fill_pattern_matrix(pattern_matrix);

  f = freopen("/dev/tty", "r", stdin);

  lives = lives_count(pattern_matrix);

  initscr();
  nodelay(stdscr, TRUE);
  noecho();

  int input_result = menu();
  if (input_result == 3) {
    fclose(f);
    return 1;
  }

  show_field(pattern_matrix);
  printw(" Ages: 0. Lives: %d\n", lives);

  refresh();
  usleep(2000000);
  clear();

  while (1) {
    k++;
    lives = lives_count(pattern_matrix);

    change_matrix(pattern_matrix);

    show_field(pattern_matrix);
    printw(" Ages: %d. Lives: %d\n", k, lives);

    refresh();
    usleep(game_speed(getch()));
    clear();
  }
  endwin();
  fclose(f);

  return 0;
}

int menu() {
  int res = 0;

  while (res != 1 && res != 3) {
    printw("1. Start\n");
    printw("2. Patterns list\n");
    printw("3. Exit\n");
    refresh();
    clear();

    int check = getchar();
    if (check == 49)
      res = 1;

    if (check == 50)
      patterns_list();

    if (check == 51)
      res = 3;
  }

  return res;
}

void read_file(FILE *ptr) {
  while (1) {
    int c = fgetc(ptr);
    if (c == EOF)
      break;
    if (c == '\n') {
      printw("\n");
    } else {
      printw("%c", c);
    }
  }
  printw("\n");
}

void patterns_list() {
  int enter = 0, direction, startIndex = STARTINDEX;

  while (1) {   // закончить цикл по нажатию пробела OR  enter
    if (enter == 27 || enter == 81 ||
        enter == 113) {   // выход из игры  esc OR  q  OR  Q
      break;
    }

    if (enter == 65 || enter == 97) {   //  A OR a   =>  -1
      direction = -1;
    }
    if (enter == 68 || enter == 100) {   //  Q OR q   =>  +1
      direction = 1;
    }

    if (enter != 10 && enter != 32) {
      startIndex = startIndex + direction;

      if (startIndex > MAXMAP) {   //  держим posIntro в границах
        startIndex = STARTINDEX;
      } else if (startIndex < STARTINDEX) {
        startIndex = MAXMAP;
      }
    }

    printPosition(startIndex);
    refresh();
    clear();

    enter = getchar();
  }

  endwin();
}

void printPosition(int posIntro) {
  FILE *ptr;
  switch (posIntro) {
  case 1:
    ptr = fopen("blinkership.txt", "r");
    printw("1. Blinker ship\n\n");
    read_file(ptr);
    break;
  case 2:
    ptr = fopen("centinalreflector.txt", "r");
    printw("2. Centinal reflector\n\n");
    read_file(ptr);
    break;
  case 3:
    ptr = fopen("backrake.txt", "r");
    printw("3. Back rake\n\n");
    read_file(ptr);
    break;
  case 4:
    ptr = fopen("barge.txt", "r");
    printw("4. Barge\n\n");
    read_file(ptr);
    break;
  case 5:
    ptr = fopen("average.txt", "r");
    printw("5. Average\n\n");
    read_file(ptr);

    break;
  default:
    break;
  }
}

void show_field(char pattern_matrix[HEIGHTMATRIX][WIDTHMATRIX]) {
  for (int y = -1; y < HEIGHT - 1; y++) {
    for (int x = -1; x < WIDTH - 1; x++) {
      if (y == -1 || y == HEIGHT - 2) {
        printw("-");
      } else if (x == -1 || x == WIDTH - 2) {
        printw("|");
      } else if (y < HEIGHTMATRIX && x < WIDTHMATRIX) {
        printw("%c", pattern_matrix[y][x]);
      }
    }
    printw("\n");
  }
}

void change_matrix(char pattern_matrix[HEIGHTMATRIX][WIDTHMATRIX]) {
  char copy[HEIGHTMATRIX][WIDTHMATRIX];
  for (int i = 0; i < HEIGHTMATRIX; i++)
    for (int j = 0; j < WIDTHMATRIX; j++) {
      int neighbours = neighbour_check(pattern_matrix, i, j);
      if (pattern_matrix[i][j] == ' ' && neighbours == 3) {
        copy[i][j] = '@';
      } else {
        if ((neighbours == 2 || neighbours == 3) &&
            pattern_matrix[i][j] == '@') {
          copy[i][j] = '@';
        } else {
          copy[i][j] = ' ';
        }
      }
    }
  for (int i = 0; i < HEIGHTMATRIX; i++)
    for (int j = 0; j < WIDTHMATRIX; j++) {
      pattern_matrix[i][j] = copy[i][j];
    }
}

void fill_pattern_matrix(char pattern_matrix[HEIGHTMATRIX][WIDTHMATRIX]) {
  int i = 0, j = 0;
  char c = 'c';
  while (c != EOF) {
    c = getchar();
    if (c == '\n') {
      i++;
      j = 0;
    } else {
      pattern_matrix[i][j] = c;
      j++;
    }
  }
}

int neighbour_check(char pattern_matrix[HEIGHTMATRIX][WIDTHMATRIX], int y,
                    int x) {
  int neighbours = 0;
  for (int i = y - 1; i <= y + 1; i++)
    for (int j = x - 1; j <= x + 1; j++) {
      if (i != y || j != x) {
        int x_temp = j;
        int y_temp = i;
        if (x_temp < 0)
          x_temp += 80;
        if (x_temp > 79)
          x_temp -= 80;
        if (y_temp < 0)
          y_temp += 25;
        if (y_temp > 24)
          y_temp -= 25;
        if (pattern_matrix[y_temp][x_temp] == '@')
          neighbours += 1;
      }
    }
  return neighbours;
}

int lives_count(char pattern_matrix[HEIGHTMATRIX][WIDTHMATRIX]) {
  int lives = 0;

  for (int y = 0; y < HEIGHTMATRIX; y++) {
    for (int x = 0; x < WIDTHMATRIX; x++) {
      if (pattern_matrix[y][x] == '@')
        lives++;
    }
  }

  return lives;
}

int game_speed(int c) {
  static int speed = INITSPEED;

  if (c == 'w') {   //  W OR w   =>  -1
    speed -= 30000;
  }
  if (c == 's') {   //  S OR s   =>  +1
    speed += 30000;
  }

  if (speed > MAXSPEED) {   //  держим posIntro в границах
    speed = MAXSPEED;
  } else if (speed < MINSPEED) {
    speed = MINSPEED;
  }

  return speed;
}
