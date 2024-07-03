#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define LEFT 0
#define RIGHT 1
#define WIDTH 10
#define HEIGHT 20
#define CELL_SIZE 30
#define BLOCK_COUNT 7
#define BLOCK_SIZE 4
#define FRAME 100000
#define FILLED '#'
#define EMPTY '.'



char board[HEIGHT][WIDTH];



char blocks[BLOCK_COUNT][BLOCK_SIZE][BLOCK_SIZE] = {
	{{'.', '.', '.', '.'},
		{'.', '.', '.', '.'},
		{0, 0, 0, 0},
		{'.', '.', '.', '.'},},

	{{'.', '.', '.', '.'},
		{'.', 1, 1, '.'},
		{'.', 1, 1, '.'},
		{'.', '.', '.', '.'}},

	{{'.', '.', '.', '.'},
		{'.', 2, '.', '.'},
		{2, 2, 2, '.'},
		{'.', '.', '.', '.'}},

	{{'.', '.', '.', '.'},
		{3, 3, '.', '.'},
		{'.', 3, 3, '.'},
		{'.', '.', '.', '.'}},

	{{'.', '.', '.', '.'},
		{'.', '.', 4, 4},
		{'.', 4, 4, '.'},
		{'.', '.', '.', '.'}},

	{{'.', 5, '.', '.'},
		{'.', 5, '.', '.'},
		{'.', 5, 5, '.'},
		{'.', '.', '.', '.'}},

	{{'.', '.', 6, '.'},
		{'.', '.', 6, '.'},
		{'.', 6, 6, '.'},
		{'.', '.', '.', '.'}}
};

char block[BLOCK_SIZE][BLOCK_SIZE];


SDL_Window * window;
SDL_Surface * surface;
SDL_PixelFormat *fmt;
Uint32 red, blue, green, magenta, cyan, yellow, white, black, gray;
Uint32 block_colors[BLOCK_COUNT];
Uint32 color;

void insert_new_block();

void init()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	for (int i = 0; i < HEIGHT; i++)
		for (int j = 0; j < WIDTH; j++)
			board[i][j] = '.';
	window = SDL_CreateWindow("hi", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, CELL_SIZE * WIDTH, CELL_SIZE * HEIGHT, 0);
	surface = SDL_GetWindowSurface(window);
	fmt = surface->format;
	black   = SDL_MapRGB(fmt, 0, 0, 0);
	gray    = SDL_MapRGB(fmt, 50, 50, 50);
	red     = SDL_MapRGB(fmt, 255, 0, 0);
	green   = SDL_MapRGB(fmt, 0, 255, 0);
	blue    = SDL_MapRGB(fmt, 0, 0, 255);
	yellow  = SDL_MapRGB(fmt, 255, 255, 0);
	magenta = SDL_MapRGB(fmt, 255, 0, 255);
	cyan    = SDL_MapRGB(fmt, 0, 255, 255);
	white   = SDL_MapRGB(fmt, 255, 255, 255);
	block_colors[0] = red;
	block_colors[1] = green;
	block_colors[2] = blue;
	block_colors[3] = yellow;
	block_colors[4] = magenta;
	block_colors[5] = cyan;
	block_colors[6] = white;
	insert_new_block();
}

void draw_board()
{
	SDL_FillRect(surface, NULL, gray);
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			SDL_Rect rect = {.x = j * CELL_SIZE + 1, .y = i * CELL_SIZE + 1, .w = CELL_SIZE - 2, .h = CELL_SIZE - 2};
			if (board[i][j] == '.')
			{
				SDL_FillRect(surface, &rect, black);
				continue;
			}
			SDL_FillRect(surface, &rect, block_colors[(int)board[i][j]]);
		}
	}
	SDL_UpdateWindowSurface(window);
}

bool block_available = false;
int block_pos_x = 2;
int block_pos_y = -1;


void fill_block(bool refill)
{
	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		for (int j = 0; j < BLOCK_SIZE; j++)
		{
			if (block[i][j] != '.' && i + block_pos_y >= 0)
			{
				board[i+block_pos_y][j+block_pos_x] = refill ? block[i][j] : '.';
			}
		}
	}

}

bool is_hitting()
{
	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		for (int j = 0; j < BLOCK_SIZE; j++)
		{
			if (block[i][j] != '.' && 
					i + block_pos_y >= 0 && 
					(i + block_pos_y >= HEIGHT || 
					 j + block_pos_x >= WIDTH ||  
					 j + block_pos_x < 0 || 
					 board[i+block_pos_y][j+block_pos_x] != '.'))
				return true;
		}
	}
	return false;
}

void write_block(int current_block)
{
	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		for (int j = 0; j < BLOCK_SIZE; j++)
		{
			block[i][j] = blocks[current_block][i][j];
		}
	}
}

void check_lines()
{
	int di = 0;
	for (int i = HEIGHT-1; i >= 0; i--)
	{
		bool all_filled = true;
		for (int j = 0; j < WIDTH; j++)
		{
			all_filled = all_filled && board[i][j] != '.';
		}
		if (all_filled)
		{
			di++;
			continue;
		}
		for (int j = 0; j < WIDTH; j++)
		{
			board[i+di][j] = board[i][j];
		}
	}
	for (int i = di-1; i >= 0; i--)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			board[i][j] = '.';
		}
	}
}

int min(int a, int b) { if (a < b) return a; return b; }

bool touching_sky()
{
	for (int i = 0; i < min(-block_pos_y, BLOCK_SIZE); i++)
	{
		for (int j = 0; j < BLOCK_SIZE; j++)
		{
			if (block[i][j] != '.')
				return true;
		}
	}
	return false;
}

bool still_hitting()
{
	fill_block(false);
	block_pos_y++;
	bool result = is_hitting();
	block_pos_y--;
	fill_block(true);
	return result;
}

void insert_new_block()
{
	block_available = true;
	int current_block = rand() % BLOCK_COUNT;
	color = block_colors[current_block];
	block_pos_x = 2;
	block_pos_y = -2;
	write_block(current_block);
}

bool update_block()
{
	if (!block_available && still_hitting())
	{
		check_lines();
		insert_new_block();
		return true;
	}
	fill_block(false);
	block_pos_y++;
	if (is_hitting())
	{
		block_pos_y--;
		if (touching_sky())
		{
			return false;
		}
		fill_block(true);
		block_available = false;
		return true;
	}
	fill_block(true);
	return true;
}


void try_move(int dx)
{
	fill_block(false);
	block_pos_x += dx;
	if (is_hitting())
	{

		block_pos_x -= dx;
	}
	fill_block(true);
	draw_board();
}

char tmp[BLOCK_SIZE][BLOCK_SIZE];
void copy_back_tmp()
{
	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		for (int j = 0; j < BLOCK_SIZE; j++)
		{
			block[i][j] = tmp[i][j];
		}
	}
}


void rotate_left()
{
	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		for (int j = 0; j < BLOCK_SIZE; j++)
		{
			tmp[i][j] = block[BLOCK_SIZE-j-1][i];
		}
	}
	copy_back_tmp();
}

void rotate_right()
{
	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		for (int j = 0; j < BLOCK_SIZE; j++)
		{
			tmp[i][j] = block[j][BLOCK_SIZE-i-1];
		}
	}
	copy_back_tmp();
	
}

bool try_rotate(int direction)
{
	if (direction == LEFT)
	{
		rotate_left();
	}
	else
	{
		rotate_right();
	}
	bool valid = !is_hitting();
	if (!valid && direction == LEFT)
	{
		rotate_right();
	}
	else if (!valid)
	{
		rotate_left();
	}
	return valid;
}

void rotate(int direction)
{
	fill_block(false);
	int dx[] = {0, -1, +1};
	for (int i = 0; i < sizeof(dx) / sizeof(dx[0]); i++)
	{
		block_pos_x += dx[i];
		if(try_rotate(direction))
		{
			break;
		}
		block_pos_x -= dx[i];
	}
	fill_block(true);
}

void fast_lower()
{
	fill_block(false);
	while (!is_hitting())
	{
		block_pos_y++;
	}
	block_pos_y--;
	fill_block(true);
}
void update_key(int keycode)
{
	switch(keycode)
	{
		case SDLK_LEFT:
			try_move(-1);
			break;
		case SDLK_RIGHT:
			try_move(1);
			break;
		case SDLK_SPACE:
			fast_lower();
			break;
		case SDLK_z:
			rotate(RIGHT);
			break;
		case SDLK_x:
			rotate(LEFT);
			break;
	}
}

int main(int argc, char* argv[])
{
	srand(time(NULL));
	init();


	struct timeval prev, current;
	gettimeofday(&prev, NULL);

	draw_board();
	for (;;)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch( event.type )
			{
				case SDL_KEYDOWN:
					update_key(event.key.keysym.sym);
					break;
				case SDL_QUIT:
					SDL_Quit();
					return 0;
				default:
					break;
			}
		}
		gettimeofday(&current, NULL);
		if ( (current.tv_sec - prev.tv_sec) * 1000000 + (current.tv_usec - prev.tv_usec) <= FRAME)
			continue;
		prev = current;
		if(!update_block())
		{
			SDL_Quit();
			return 0;
		}
		draw_board();
	}
}
