#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "lib.h"


/* Snake game for linux terminal */

int main (int argc, char **argv) {

	/* Choose level */
	int level = -1;
	char* input_buffer = malloc(SCORE_BUFFER_SIZE * sizeof(*input_buffer));
	printf("Which level do you wanna play? Please, insert a number between 0 and 2\n\n");
	printf("0 : Vanilla snake\n");
	printf("1 : Faster snake\n");
	printf("2 : Walls in the map\n\n");
	printf("Level = ");
	fgets(input_buffer, SCORE_BUFFER_SIZE, stdin);
	while (sscanf(input_buffer, "%d", &level) != 1 || ( level != 0 && level != 1 && level != 2 ) ) {
		printf("Level = ");
		fgets(input_buffer, SCORE_BUFFER_SIZE, stdin);
	}
	free(input_buffer);

	/* Hide cursor */
	printf("\e[?25l");

	/* Disable echo */
	struct termios oldt, newt;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	
	/* Clear terminal and move cursor to the top left corner */
	printf("\x1b[%dJ", 2);
	printf("\x1b[%d;%df", 1, 1);
	
	/* Variables */
	int quit = 0;
	int gameover = 0;
	int win = 0;
	char board[NUM_ROWS * NUM_COLS];
	char score_buffer[SCORE_BUFFER_SIZE];
	char input_move[INPUT_BUFFER_SIZE];
	Snake snake;
	int num_apples = 40;
	Apple apples[num_apples];
	int len;
	

	/* Initialize the snake */
	snake.length = 1;
	snake.body[0].x = 5;
	snake.body[0].y = 5;
	snake.vx = 1;
	snake.vy = 0;

	/* Create apples */
	srand(time(NULL));
	for (int i = 0; i < num_apples; i++) {
		apples[i].x = 1 + rand() % (NUM_ROWS-2);
		apples[i].y = 1 + rand() % (NUM_COLS-2);
		apples[i].eaten = 0;
	}

	/* Create walls */
	Wall* walls = malloc(NUM_WALLS * sizeof(*walls));
	if (level == 2) {
		init_walls(walls, snake);
	}

	/* Check overlap between walls and apples */
	check_overlap_wall_apple(walls, apples, num_apples);

	/* Print commands */
	printf("\nCommands:\n\n");
	printf("\t-> w : up\n");
	printf("\t-> d : right\n");
	printf("\t-> s : down\n");
	printf("\t-> a : left\n");
	printf("\t-> q : quit\n\n");

	while (! quit && ! gameover && ! win) {

		/* Initialize the board */
		fill_board(board);

		/* Make the move */
		move_snake(&snake);

		/* Check if the snake ate an apple */
		check_apple_eaten(apples, num_apples, &snake);

		/* Check for gameover */
		check_gameover(&snake, &gameover);

		/* Check for gameover with walls */
		if (level == 2) { check_gameover_walls(&snake, walls, &gameover); }

		/* Check for win */
		if(snake.length == MAX_SNAKE_LEN-1) {win = 1;}

		/* Check that apples do not overlap with walls */
		if (level == 2) {
			check_overlap_wall_apple(walls, apples, num_apples);
		}

		/* Place the apples */
		place_apples(apples, num_apples, board);

		/* Place the snake */
		place_snake(snake, board);

		/* Place walls */
		if (level == 2) {
			place_walls(walls, board);
		}
		
		/* Print the board */
		print_board(board);

		/* Print the score */
		snprintf(score_buffer, SCORE_BUFFER_SIZE, "\nScore : %d", (snake.length - 1) * 100 );
		puts(score_buffer);

		/* Wait time */
		if (level != 1) {
			usleep(8 * 1000000 / 60);
		} else {
			usleep(6 * 1000000 / 60);
		}


		/* Read input */
		struct timeval tv;
		fd_set fds;
		tv.tv_sec = 0;
		tv.tv_usec = 0;

		FD_ZERO(&fds);
		FD_SET(STDIN_FILENO, &fds);
		select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);

		if(FD_ISSET(STDIN_FILENO, &fds)) {
			len = read(STDIN_FILENO, input_move, INPUT_BUFFER_SIZE);
			int input = input_move[len - 1];
			update_snake(&snake, input, &quit);
		}

		if (! gameover && ! quit && ! win) {
			/* Clear the score line, move cursor up of NUM_ROWS + 1 and cancel from there to the bottom */
			fflush(stdout);
			printf("\x1b[%dD", (int)strlen(score_buffer));
			printf("\x1b[%dA", NUM_ROWS + 2);
			printf("\x1b[%dJ", 0);
		}

	}

	free(walls);

	/* Show cursor */
	printf("\e[?25h");

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

	if (quit == 1) {
		printf("\n");
	}

	if (quit == 0) {
		printf("\nGame over!\n\nFinal score = %d\n\n", (snake.length-1) * 100);
	}

	if (win == 1) {
		printf("\nCongratulations! You won! You've reached the maximum snake size!\n\nFinal score = %d\n\n", (snake.length-1) * 100);
	}

	return 0;
}

