/**************************************************************
* FILENAME:		SDL_Render_Main.c
*
* DESCRIPTION:	Primitive 3D rendering using a well-known raycasting
*				algorithm.
*
* NOTES:		Encapsulates the entire renderer.
*					1. Render a basic SDL window.
*						a. Resolution determined by -w ### -h ### arguments
*						b. Otherwise, a resolution of 640x480 is used
*					2. Set up a basic 'world' environment
*						a. A 2-Dimensional grid space is hardcoded
*						b. The player's position, direction, etc. are as well.
*				
*
* AUTHORS:		Thomas Reichman
*				C commenting conventions adapted from http://syque.com/cstyle/ch4.htm
**************************************************************/
#include "SDL2/SDL.h"	// Required SDL
#include "SDL2/SDL_ttf.h"
#include "stdio.h"		// and standard IO libraries
#include "stdlib.h"		// rand(), srand()
#include "time.h"		// time()
#include "pthread.h"	// multithreading support
#include "const.h"		// personal set of constants
#include "map1.h"

// Function Declarations
void CheckForInput();
void RayCast();


// Screen dimension constants
const int SCREEN_WIDTH = 640; //640
const int SCREEN_HEIGHT = 480; //480

// Globals
SDL_Renderer *renderer;
SDL_Event event;
SDL_Window *window;

double positionInfo[11]	= {3.0, 6.0, -1.0, 0.0, 0.0, 0.66, 0.0, 0.0, FALSE, 0.0, 0.0};
double *playerPosX 		= &positionInfo[0];
double *playerPosY 		= &positionInfo[1];
double *playerDirX 		= &positionInfo[2];
double *playerDirY 		= &positionInfo[3];
double *planeX 			= &positionInfo[4];
double *planeY 			= &positionInfo[5];
double *currentFrame 	= &positionInfo[6];
double *lastFrame 		= &positionInfo[7];
double *readyToQuit 	= &positionInfo[8];
double *moveSpeed 		= &positionInfo[9];
double *rotSpeed 		= &positionInfo[10];

bool displayFPS = FALSE;

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
int main(int argc, char *argv[]){
	/* Argument Variables */
	for (int i = 1; i < argc; i++){
		if (!strcmp(argv[i], "-showfps") && (i+1) < argc && !strcmp(argv[i+1], "1")){
			printf("%s\n", "YOUYOU");
			displayFPS = TRUE;
		}
	}

	/* Setup stuff */
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
	TTF_Font * font = TTF_OpenFont("opensans.ttf", 25);
	SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
	SDL_Color color = { 255, 255, 255 };
	SDL_Surface * surface = TTF_RenderText_Solid(font, "Welcome to Gigi Labs", color);
	printf("%s\n", "MEME");
	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);


	RenderBackground();

	/* Thread fun */
	//pthread_t raycast_thread; //this variable is our reference to the second thread
	
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
		}
*/



	while (*readyToQuit != TRUE){
		CheckForInput();
		RayCast();
		//sleep( 16.66666666 );
	}

	/* Cleanly exit */
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_CloseFont(font);
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);
	TTF_Quit();
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
void RayCast(){
	/* Every frame starts with a background */
	/* It will be overwritten with actual data */
	RenderBackground();

	/* render one vertical line at a time */
	for(int x = 0; x < SCREEN_WIDTH; x++){
		// calculate ray position and direction
		double cameraX = 2 * x / ((double)(SCREEN_WIDTH) - 1); // x-coordinate in camera space
		double rayDirX = *playerDirX + *planeX * cameraX;
		double rayDirY = *playerDirY + *planeY * cameraX;
		// which box of the map we're in
			// we only need double precision for rendering calculations.
			// it's safe to round off here to figure out which box's boundaries we're within
		int mapX = (int) *playerPosX;
		int mapY = (int) *playerPosY;
		// length of ray from current position to next x or y-side
		double sideDistX;
		double sideDistY;
		 // length of ray from one side to the next
		double deltaDistX = fabs(1 / rayDirX);
		double deltaDistY = fabs(1 / rayDirY);
		double perpWallDist;
		// what direction to step in x or y-direction (either +1 or -1)
		int stepX;
		int stepY;
		int hit = 0;	// was there a wall hit?
		int side; 		// /was a NS or a EW wall hit?	
		//calculate step and initial sideDist
		if (rayDirX < 0){
			stepX = -1;
			sideDistX = (*playerPosX - mapX) * deltaDistX;
		}
		else{
			stepX = 1;
			sideDistX = (mapX + 1.0 - *playerPosX) * deltaDistX;
		}
		if (rayDirY < 0){
			stepY = -1;
			sideDistY = (*playerPosY - mapY) * deltaDistY;
		}
		else{
			stepY = 1;
			sideDistY = (mapY + 1.0 - *playerPosY) * deltaDistY;
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
			perpWallDist = (mapX - *playerPosX + (1 - stepX) / 2) / rayDirX;
		}
		else{
			perpWallDist = (mapY - *playerPosY + (1 - stepY) / 2) / rayDirY;
		}
		//Calculate height of line to draw on screen
		int lineHeight = (int)(SCREEN_HEIGHT / perpWallDist);
		//calculate lowest and highest pixel to fill in current stripe
		int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
		if(drawStart < 0)drawStart = 0;
		int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;
		if(drawEnd >= SCREEN_HEIGHT)drawEnd = SCREEN_HEIGHT - 1;
		//choose wall color
		double wall_color;
		switch(worldMap[mapX][mapY]){
			case 1:	wall_color = 1;		break; //red
			case 2:	wall_color = 2;		break; //green
			case 3:	wall_color = 3; 	break; //blue
			case 4:	wall_color = 4;		break; //white
			default: wall_color = 5;	break; //yellow
		}
		//give x and y sides different brightness
		if (side == 1) {wall_color = wall_color + 0.5;}
		//draw the pixels of the stripe as a vertical line
		verLine(x, drawStart, drawEnd, wall_color);
	}	
	/* timing for input and FPS counter */
	*lastFrame = *currentFrame;
	*currentFrame = SDL_GetTicks();
	double frameTime = (*currentFrame - *lastFrame) / 1000.0; //frameTime is the time this frame has taken, in seconds
	
	if (displayFPS == TRUE){
		TTF_Font * font = TTF_OpenFont("opensans.ttf", 25);
		SDL_Color color = { 255, 255, 255 };
		SDL_Surface * surface = TTF_RenderText_Solid(font, "Welcome to Gigi Labs", color);
		printf("%s\n", "MEME");
		SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		printf("%lf\n", 1.0 / frameTime);
	}
		
	SDL_RenderPresent(renderer);
	//RenderBackground();

	//speed modifiers
	*moveSpeed = frameTime * 5.0; //the constant value is in squares/second
	*rotSpeed = frameTime * 3.0; //the constant value is in radians/second
}

//
bool verLine(int x, int y1, int y2, double wall_color){
	//swap y1 and y2
	if(y2 < y1){
		y1 += y2;
		y2 = y1 - y2;
		y1 -= y2;
	} 
	if(y2 < 0 || y1 >= SCREEN_HEIGHT	|| x < 0 || x >= SCREEN_WIDTH) return 0; //no single point of the line is on screen
	if(y1 < 0) y1 = 0; //clip
	if(y2 >= SCREEN_WIDTH) y2 = SCREEN_HEIGHT - 1; //clip

	//printf("%lf\n", wall_color);

	if (wall_color == 1.0){
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // draw in red color	
	}
	else if (wall_color == 1.5){
		SDL_SetRenderDrawColor(renderer, 130, 0, 0, 255); // draw in reddish color	
	}
	else if (wall_color == 2.0){
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // draw in green color	
	}
	else if (wall_color == 2.5){
		SDL_SetRenderDrawColor(renderer, 0, 130, 0, 255); // draw in greenish color	
	}
	else if (wall_color == 3.0){
		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // draw in blue color	
	}
	else if (wall_color == 3.5){
		SDL_SetRenderDrawColor(renderer, 0, 0, 130, 255); // draw in blueish color	
	}
	else if (wall_color == 5.0){
		SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // draw in yellow color	
	}
	else{
		SDL_SetRenderDrawColor(renderer, 255, 130, 0, 255); // draw in yellow color	
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

void CheckForInput(){
	//printf("%s", "CheckForInput");
	//printf("%f\n", positionInfo[1]);
	//return;
	/* Poll for events */
		
	//*readyToQuit = *readyToQuit + 1;
	//printf("%lf\n", positionInfo[8]);
	//printf("%lf\n", readyToQuit);
	
	while( SDL_PollEvent( &event ) ){
		// SDL_QUIT event (window close) /
		if( event.type == SDL_QUIT ){
			*readyToQuit = TRUE;
		}
		else{
			//double moveSpeed = 0.1;
			//double rotSpeed = 0.1;
			Uint8 *keystates = SDL_GetKeyboardState(NULL);
		 	// Keyboard event /
			if( keystates[ SDL_SCANCODE_W ] ){
			 	//printf("Hey, you pressed W!\n");

			 	/* Determine coordinates of where we'd like to end up */
			 	double newXPos = *playerPosX + (*playerDirX * *moveSpeed);
			 	double newYPos = *playerPosY + (*playerDirY * *moveSpeed);

			 	/* End up there if we can */
				if(worldMap[(int)(newXPos)][(int)(*playerPosY)] == EMPTYSPACE){
					*playerPosX = newXPos;
				}
      			if(worldMap[(int)(*playerPosX)][(int)(newYPos)] == EMPTYSPACE){
      				*playerPosY = newYPos;
      			}
			}
			if( keystates[ SDL_SCANCODE_S ] ){
				//printf("Hey, you pressed S!\n");
				double newXPos = *playerPosX - (*playerDirX * *moveSpeed);
				double newYPos = *playerPosY - (*playerDirY * *moveSpeed);

			 	/* End up there if we can */
				if(worldMap[(int)(newXPos)][(int)(*playerPosY)] == EMPTYSPACE){
					*playerPosX = newXPos;
				}
      			if(worldMap[(int)(*playerPosX)][(int)(newYPos)] == EMPTYSPACE){
      				*playerPosY = newYPos;
      			}
			}
			if( keystates[ 	SDL_SCANCODE_LEFT ]){
			 	double oldDirX = *playerDirX;
			 	*playerDirX = *playerDirX * cos(*rotSpeed) - *playerDirY * sin(*rotSpeed);
			 	*playerDirY = oldDirX * sin(*rotSpeed) + *playerDirY * cos(*rotSpeed);
			 	double oldPlaneX = *planeX;
			 	*planeX = *planeX * cos(*rotSpeed) - *planeY * sin(*rotSpeed);
			 	*planeY = oldPlaneX * sin(*rotSpeed) + *planeY * cos(*rotSpeed);
			}
			if( keystates[ 	SDL_SCANCODE_RIGHT ] ){
			 	double oldDirX = *playerDirX;
			 	*playerDirX = *playerDirX * cos(-*rotSpeed) - *playerDirY * sin(-*rotSpeed);
			 	*playerDirY = oldDirX * sin(-*rotSpeed) + *playerDirY * cos(-*rotSpeed);
			 	double oldPlaneX = *planeX;
			 	*planeX = *planeX * cos(-*rotSpeed) - *planeY * sin(-*rotSpeed);
			 	*planeY = oldPlaneX * sin(-*rotSpeed) + *planeY * cos(-*rotSpeed);
			}
			if( keystates[ 	SDL_SCANCODE_D ] ){
			 	//printf("Hey, you pressed D!\n");

			}
			if( keystates[ SDL_SCANCODE_A ] ){
				//printf("Hey, you pressed A!\n");

			}
		}
	}
	
}
