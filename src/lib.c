#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include "lib.h"


void fill_board(char* board) {

    /* Fill the board */
     for (int i = 0; i < NUM_ROWS; i++) {
        for (int j = 0; j < NUM_COLS; j++) {

            board[i * NUM_COLS + j] = ' ';
            
            if (j == 0 || j == NUM_COLS - 1) {
                board[i * NUM_COLS + j] = '|';
            }
            if (i == 0 || i == NUM_ROWS - 1) {
                board[i * NUM_COLS + j] = '-';
            }
        }
        
    }
}



void check_gameover(Snake* snake, int* gameover) {

    for(int i = 1; i<snake->length; i++) {
        if(snake->body[0].x == snake->body[i].x && snake->body[0].y == snake->body[i].y) {
            *gameover = 1;
        }
    }
}



void check_gameover_walls (Snake* snake, Wall* walls, int* gameover) {
    for(int i = 0; i < NUM_WALLS; i++) {
        if(snake->body[0].x == walls[i].x && snake->body[0].y == walls[i].y) {
            *gameover = 1;
        }
    }
}



void check_apple_eaten(Apple* apples, int num_apples, Snake* snake){

    for (int i = 0; i<num_apples; i++) {
        if (apples[i].x == snake->body[0].x && apples[i].y == snake->body[0].y && apples[i].eaten == 0) {
            
            /* Apple has been eaten */
            apples[i].eaten = 1;
            printf("\a");
            
            /* Snake gets longer */
            snake->length += 1;
            for (int i = snake->length-1; i>0; i--) {
                snake->body[i] = snake->body[i-1];
            }
            snake->body[0].x += snake->vx;
            snake->body[0].y += snake->vy;
        
        }        
    }
}



void place_apples(Apple* apples, int num_apples, char* board){

    int eatable_apples = 0;
    for(int i = 0; i < num_apples; i++) {
        if (apples[i].eaten == 0) {
            board[apples[i].x * NUM_COLS + apples[i].y] = '+';
            ++eatable_apples;
        }
    }

    /* If I have less than 10 apples, I make more apples spawn */
    if (eatable_apples < 10) {
        for(int i = 0; i < num_apples; i++) {
            if (apples[i].eaten == 1) {
                apples[i].x = 1 + rand() % (NUM_ROWS-2);
		        apples[i].y = 1 + rand() % (NUM_COLS-2);
                apples[i].eaten = 0;
                board[apples[i].x * NUM_COLS + apples[i].y] = '+';
                break;
            }
        }
    }
}



void place_snake(Snake snake, char* board) {

    /* Print the snake using @ for the head and ~ for the body */
    for (int piece = snake.length - 1; piece >= 0; piece--) {
        if (piece == 0) {
            board[snake.body[piece].x * NUM_COLS + snake.body[piece].y] = '@';
        } else {
            board[snake.body[piece].x * NUM_COLS + snake.body[piece].y] = '~';
        }
    }

}



void place_walls(Wall* walls, char* board){

    for(int i = 0; i < NUM_WALLS; i++) {
        board[walls[i].x * NUM_COLS + walls[i].y] = '#';
    }

}



void print_board(char* board) {

    /* Print the board */
    for (int i = 0; i < NUM_ROWS; i++) {
        for (int j = 0; j < NUM_COLS; j++) {

            /* Switch for colors: snake = green; apple = red */
            switch (board[i * NUM_COLS + j]) {
            case '@':
                printf("\x1b[43m%c", ' ');
                printf("\x1b[%dm", 0);
                break;
            
            case '~' :
                printf("\x1b[42m%c", ' ');
                printf("\x1b[%dm", 0);
                break;

            case '+' :
                printf("\x1b[41m%c", ' ');
                printf("\x1b[%dm", 0);
                break;

            case '#' :
                printf("\x1b[46m%c", ' ');
                printf("\x1b[%dm", 0);
                break;

            default:
                putchar(board[i * NUM_COLS + j]);
            }
        }
        putchar('\n');
    }

}



void move_snake(Snake* snake) {

    for (int i = snake->length-1; i>0; i--) {
        snake->body[i] = snake->body[i-1];
    }
    snake->body[0].x += snake->vx;
    snake->body[0].y += snake->vy;

    /* Periodic boundary conditions*/
    snake->body[0].x = (snake->body[0].x + NUM_ROWS) % NUM_ROWS;
    snake->body[0].y = (snake->body[0].y + NUM_COLS) % NUM_COLS;

}



void update_snake(Snake* snake, int input, int* quit){ 

    if (input == 'q') {
        *quit = 1;
    }
    if (input == 'w' && snake->vx != 1) {
        snake->vx = -1; snake->vy =  0;
    }
    if (input == 'a' && snake->vy != 1) {
        snake->vx =  0; snake->vy = -1;
    }
    if (input == 's' && snake->vx != -1) {
        snake->vx = 1; snake->vy =  0;
    }
    if (input == 'd' && snake->vy != -1) {
        snake->vx =  0; snake->vy = 1;
    }

}



void init_walls(Wall* walls, Snake snake){
    for (int i = 0; i < NUM_WALLS; i++) {
        do {
            walls[i].x = rand()%(NUM_ROWS - 2) + 1;
            walls[i].y = rand()%(NUM_COLS - 2) + 1;
        } while( walls[i].x >= snake.body[0].x && walls[i].x <= snake.body[0].x + 5 );
    }
}



void check_overlap_wall_apple(Wall* walls, Apple* apples, int num_apples) {
    
    for(int i = 0; i < num_apples; i++) {
        
        int swap = 1;
        
        while (swap == 1) {
            swap = 0;

            for (int j = 0; j < NUM_WALLS; j++) {
                if (apples[i].eaten == 0 && apples[i].x == walls[j].x && apples[i].y == walls[j].y) {
                    apples[i].x = 1 + rand() % (NUM_ROWS-2);
		            apples[i].y = 1 + rand() % (NUM_COLS-2);
                    swap = 1;
                }
            }
        
        }
        
    }
}