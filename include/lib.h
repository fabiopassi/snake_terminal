#ifndef __LIB_H__
#define __LIB_H__


/* Constants */
#define NUM_ROWS 20
#define NUM_COLS 20
#define MAX_SNAKE_LEN 350
#define SCORE_BUFFER_SIZE 128
#define INPUT_BUFFER_SIZE 4096
#define NUM_WALLS 10


/* Typedef */

typedef struct {
    int x,y;
} BodyPart;

typedef struct {
    int length;
    int vx,vy;
    BodyPart body[MAX_SNAKE_LEN];
} Snake;

typedef struct {
    int x,y;
    int eaten;
} Apple;

typedef struct {
    int x,y;
} Wall;


/* Headers of functions */

void fill_board(char* board);

void check_apple_eaten(Apple* apples, int num_apples, Snake* snake);

void check_gameover(Snake* snake, int* gameover);

void check_gameover_walls(Snake* snake, Wall* walls, int* gameover);

void place_apples(Apple* apples, int num_apples, char* board);

void place_snake(Snake snake, char* board);

void place_walls(Wall* walls, char* board);

void print_board(char* board);

void move_snake(Snake* snake);

void update_snake(Snake* snake, int input, int* quit);

void init_walls(Wall* walls, Snake snake);

void check_overlap_wall_apple(Wall* walls, Apple* apples, int num_apples);

#endif

