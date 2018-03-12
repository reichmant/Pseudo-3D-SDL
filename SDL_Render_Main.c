/**************************************************************
* FILENAME:		SDL_Render_Main.c
*
* DESCRIPTION:	The actual project file.
*
* NOTES:		Render a basic SDL window. No point writing a description
*				for test file. Just various tests and per-pixel rendering.
*
* AUTHORS:		Thomas Reichman
*				C commenting conventions adapted from http://syque.com/cstyle/ch4.htm
**************************************************************/


#include "SDL2/SDL.h"	// Required SDL
#include "stdio.h"		// and standard IO libraries
#include "stdlib.h"		// rand(), srand()
#include "time.h"		// time()
#include "pthread.h"	// multithreading support
#include "const.h"		// personal set of constants


#define WORLD_WIDTH 5
#define WORLD_HEIGHT 9
double positionInfo[9] = {3.0, 6.0, -1.0, 0.0, 0.0, 0.66, 0.0, 0.0, FALSE};
int worldMap[WORLD_HEIGHT][WORLD_WIDTH]=
{
	{1,2,3,2,1},
	{1,0,0,0,1},
	{1,0,0,0,1},
	{1,0,0,0,1},	
	{1,0,0,0,1},
	{1,0,0,0,1},
	{1,0,0,0,1},
	{1,1,0,0,1},
	{1,1,1,1,1}
};


// Function Declarations
void DrawPixel(int r, int g, int b, int x, int y);
int ValidateColorRange(int color);
void RenderBackground();
void CheckForInput(double *positionInfo);
void* RayCast(double *positionInfo);
bool verLine(int x, int y1, int y2, int wall_color);

// Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Globals
SDL_Renderer *renderer;
SDL_Event event;
SDL_Window *window;

/* ---- main() ---------------------------------------
* Parameters: 	None
* Type: 		Public
* Return:		termination reason (success/failure)
* Description:
*	Encapsulates the entire program
*		1. Setup a basic SDL window and threads
*		2. Run two threads that interact with eachother.
*			One thread checks for input and returns which key(s) have been pressed.
*			The other updates the player's position/direction and renders a new frame.
*		3. When a "quit" condition is reached, the program terminates successfully.
* --------------------------------- end interruptHandler() ---- */
int main(){
	/* Setup stuff */
	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
	RenderBackground();

	/* Thread fun */
	pthread_t raycast_thread; //this variable is our reference to the second thread
	

	/* setup locals */
/*
	double player_posX = 3.0; // Where we are
	double player_posY = 6.0;

	double directionX = -1.0; // Where we're facing
	double directionY = 0.0;

	double planeX = 0.0;		// Camera plane position
	double planeY = 0.66;

	double currentFrame = 0;// Time between frames
	double lastFrame = 0;
*/
	//return 1;

	//double positionInfo[9] = {3.0, 6.0, -1.0, 0.0, 0.0, 0.66, 0.0, 0.0, FALSE};
/*
	printf("%f", positionInfo[1]);
	/* Create a second thread which executes RayCast(keylist) /
	if(pthread_create(&raycast_thread, NULL, RayCast, &positionInfo)) {
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}
	// Perform CheckForInput /
	CheckForInput(positionInfo);

	// Pass along the list of pressed keys /
	if(pthread_join(raycast_thread, positionInfo)) {
		fprintf(stderr, "Error joining thread\n");
		return 2;
	}
*/
	while (positionInfo[9] == FALSE){
		RayCast()
	}



	/* Cleanly exit */
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return EXIT_SUCCESS;
	
}

/* ---- RayCast() ---------------------------------------
* Parameters: 	PositionInfo
* Type: 		Public
* Return:		None
* Description:
*	Renders an entire frame using a traditional raycasting algorithm
*	We'll fill the window with the rendered frame and update accordingly
*		Pixels are placed individually in vertical rows
* --------------------------------- end RayCast() ---- */
void* RayCast(double *positionInfo){
	printf("%s\n", "hi");
	printf("%f", positionInfo[1]);

	double player_posX = positionInfo[0]; // Where we are
	double player_posY = positionInfo[1];

	double directionX = positionInfo[2]; // Where we're facing
	double directionY = positionInfo[3];

	double planeX = positionInfo[4];		// Camera plane position
	double planeY = positionInfo[5];

	double currentFrame = positionInfo[6];// Time between frames
	double lastFrame = positionInfo[7];




	/* render one vertical line at a time */
	for(int x = 0; x < SCREEN_WIDTH; x++){
		// calculate ray position and direction
		double cameraX = 2 * x / (double)(SCREEN_WIDTH) - 1; // x-coordinate in camera space
		double rayDirX = directionX + planeX * cameraX;
		double rayDirY = directionY + planeY * cameraX;
		// which box of the map we're in
			// we only need double precision for rendering calculations.
			// it's safe to round off here to figure out which box's boundaries we're within
		int mapX = (int) player_posX;
		int mapY = (int) player_posY;
		// length of ray from current position to next x or y-side
		double sideDistX;
		double sideDistY;
		 // length of ray from one side to the next
		double deltaDistX = abs(1 / rayDirX);
		double deltaDistY = abs(1 / rayDirY);
		double perpWallDist;
		// what direction to step in x or y-direction (either +1 or -1)
		int stepX;
		int stepY;
		int hit = 0;	// was there a wall hit?
		int side; 		// /was a NS or a EW wall hit?	
		//calculate step and initial sideDist
		if (rayDirX < 0){
			stepX = -1;
			sideDistX = (player_posX - mapX) * deltaDistX;
		}
		else{
			stepX = 1;
			sideDistX = (mapX + 1.0 - player_posX) * deltaDistX;
		}
		if (rayDirY < 0){
			stepY = -1;
			sideDistY = (player_posY - mapY) * deltaDistY;
		}
		else{
			stepY = 1;
			sideDistY = (mapY + 1.0 - player_posY) * deltaDistY;
		}
		/* Simple Digital Differential Analysis Algorithm */
		// determines distance to next
		while (hit == 0){
			//jump to next map square, OR in x-direction, OR in y-direction
			if (sideDistX < sideDistY){
			sideDistX += deltaDistX;
			mapX += stepX;
			side = 0;
		}
			else{
			sideDistY += deltaDistY;
			mapY += stepY;
			side = 1;
			}
			//Check if ray has hit a wall (or out of bounds)
			if (worldMap[mapX][mapY] > 0) hit = 1;
		}
		//Calculate distance projected on camera direction
		if (side == 0){
			perpWallDist = (mapX - player_posX + (1 - stepX) / 2) / rayDirX;
		}
		else{
			perpWallDist = (mapY - player_posY + (1 - stepY) / 2) / rayDirY;
		}
		//Calculate height of line to draw on screen
		int lineHeight = (int)(SCREEN_HEIGHT / perpWallDist);
		//calculate lowest and highest pixel to fill in current stripe
		int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
		if(drawStart < 0)drawStart = 0;
		int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;
		if(drawEnd >= SCREEN_HEIGHT)drawEnd = SCREEN_HEIGHT - 1;
		//choose wall color
		int wall_color;
		switch(worldMap[mapX][mapY]){
		case 1:	wall_color = 1;		break; //red
		case 2:	wall_color = 2;		break; //green
		case 3:	wall_color = 3; 	break; //blue
		case 4:	wall_color = 4;		break; //white
		default: wall_color = 5;	break; //yellow
		}
		//give x and y sides different brightness
		//if (side == 1) {color = color / 2;}
		//draw the pixels of the stripe as a vertical line
		verLine(x, drawStart, drawEnd, wall_color);
	}	
	/* timing for input and FPS counter */
	lastFrame = currentFrame;
	currentFrame = SDL_GetTicks();
	double frameTime = (currentFrame - lastFrame) / 1000.0; //frameTime is the time this frame has taken, in seconds
	//printf("%s\n", 1.0 / frameTime);
		// CAUSES OCCASIONAL SEGFAULT
		
	SDL_RenderPresent(renderer);
	//RenderBackground();

	//speed modifiers
	double moveSpeed = frameTime * 5.0; //the constant value is in squares/second
	double rotSpeed = frameTime * 3.0; //the constant value is in radians/second
}

//
bool verLine(int x, int y1, int y2, int wall_color){
	if(y2 < y1) {y1 += y2; y2 = y1 - y2; y1 -= y2;} //swap y1 and y2
	if(y2 < 0 || y1 >= SCREEN_HEIGHT	|| x < 0 || x >= SCREEN_WIDTH) return 0; //no single point of the line is on screen
	if(y1 < 0) y1 = 0; //clip
	if(y2 >= SCREEN_WIDTH) y2 = SCREEN_HEIGHT - 1; //clip

	if (wall_color == 1){
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // draw in red color	
	}
	if (wall_color == 2){
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // draw in green color	
	}
	if (wall_color == 3){
		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // draw in blue color	
	}


	int bufp;
	//bufp = (int)scr->pixels + y1 * scr->pitch / 4 + x;
	for(int y = y1; y <= y2; y++)
	{
	 SDL_RenderDrawPoint(renderer, x, y);
	}
	return 1;
}

/* Draw function abstraction */
void DrawPixel(int r, int g, int b, int x, int y){
	SDL_SetRenderDrawColor(renderer, r, g, b, 255); // draw in told color
	SDL_RenderDrawPoint(renderer, x, y);
}

/* Assert acceptable color values */
int ValidateColorRange(int color){
	if (color > 255) color = 255;
	else if (color < 0) color = 0;
	return color;
}

void RenderBackground(){
	int currentPartition;
	int MAXpartition = 9; // must be odd for proper symmetry
	int R = 60;
	int G = 40;
	int B = 0;
	int x;
	int y;
	
	for (currentPartition = 0; currentPartition < MAXpartition+1; currentPartition++){
		for (x = 0; x < SCREEN_WIDTH; x++){
			for (y = (SCREEN_HEIGHT/MAXpartition)*(currentPartition); y < (SCREEN_HEIGHT/MAXpartition)*(currentPartition+1); y++){
				DrawPixel(ValidateColorRange(R), ValidateColorRange(G), ValidateColorRange(B), x, y);
			}
		}
		if (currentPartition > (MAXpartition/2)-1){
			R = R + 5;
			G = G + 5;
			B = B + 5;
		}
		else{
			R = R - 5;
			G = G - 5;
			B = B - 5;
		}

	}
}

void CheckForInput(double *positionInfo){
	printf("%s", "CheckForInput");
	printf("%f\n", positionInfo[1]);
	//return;
	/* Poll for events */
	
	while( SDL_PollEvent( &event ) ){	
 		switch( event.type ){
	 		// Keyboard event /
		 	case SDL_KEYUP:
			 	printf("Hey, you pressed up!\n");
			 	break;
			// SDL_QUIT event (window close) /
			case SDL_QUIT:

	 			break;
		 	default:
			 	break;
		}
	}
	
}
