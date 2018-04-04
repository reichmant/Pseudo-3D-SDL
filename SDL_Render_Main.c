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
*						a. A 2-Dimensional grid space is hardcoded in map file
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
#include "SDL_Helpers.h"

// Function Declarations
void CheckForInput();
void RayCast();

// Position Globals
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

int displayFPS = FPS_OFF; // don't show FPS unless specified via arguments

/* ---- main() ---------------------------------------
* Parameters: 	Various Arguments (see ReadMe.MD)
* Type: 		Public
* Return:		termination reason (success/failure)
* Description:
*	Encapsulates the entire program
*		0. Handle any commandline arguments if neccessary
*		1. Setup a basic SDL window
*		2. Alternatively run two functions:
*			a. One checks for input and updates information about the player
*				(position, direction etc.) based on which keys are presssed.
*			b. The other updates the player's position/direction and renders
*				a new frame.
*		3. When a "quit" condition is reached, the program terminates successfully.
*			All related libraries exit cleanly and window/process is destroyed.
* --------------------------------- end main() ---- */
int main(int argc, char *argv[]){
	/* Argument Variables */
	for (int i = 1; i < argc; i++){
		if (!strcmp(argv[i], "-showfps") && (i+1) < argc){
			if (!strcmp(argv[i+1], "1")){
				printf("%s\n", "Showing FPS in Window");
				displayFPS = FPS_WINDOW;
			}
			else if (!strcmp(argv[i+1], "2")){
				printf("%s\n", "Showing FPS in Terminal");
				displayFPS = FPS_TERMINAL;
			}
		}
		else if (!strcmp(argv[i], "-h") && (i+1) < argc){
			printf("%s%i\n", "Vertical resoluton: ",   atoi(argv[i+1]));
			SCREEN_HEIGHT =  atoi(argv[i+1]);
			if (SCREEN_HEIGHT = 0) SCREEN_WIDTH = 480;
		}
		else if (!strcmp(argv[i], "-w") && (i+1) < argc){
			if (!isdigit(argv[i+1][0]) || argv[i+1] == 0) SCREEN_WIDTH = 640;
			else SCREEN_WIDTH  =  atoi(argv[i+1]);
			printf("%s%i\n", "Horizontal resoluton: ", SCREEN_WIDTH);
		}


	}

	/* Setup libraries and create window */
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
	SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
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
	//RenderBackground();
	
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	/*
	for (int x = 0; x < SCREEN_WIDTH; x++){
		for (int y = 0; y < SCREEN_HEIGHT; y++){
			SDL_RenderDrawPoint(renderer, x, y);
		}
	}
	*/
	SDL_RenderClear(renderer);

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
	/*	
	unsigned long endTime;
	unsigned long currentTime;
	currentTime = (unsigned long)time(NULL);
	endTime = currentTime + 1;
	while (currentTime < endTime)
	*/

	/* timing for input and FPS counter */
	*lastFrame = *currentFrame;
	//sleep( 16.66666666 );
	*currentFrame = SDL_GetTicks();
	long double frameTime = (*currentFrame - *lastFrame) / 1000.0; //frameTime is the time this frame has taken, in seconds
	
	if (displayFPS == FPS_WINDOW){
		TTF_Font * font = TTF_OpenFont("opensans.ttf", 25);
		SDL_Color color = { 255, 255, 255 };

		char result[999];
		sprintf(result, "%lu", (long unsigned) round(1.0/frameTime));
		SDL_Surface * surface = TTF_RenderText_Solid(font, result, color);
		SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);

		int texW = 0;
		int texH = 0;
		SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
		SDL_Rect dstrect = { 0, 0, texW, texH };
		SDL_RenderCopy(renderer, texture, NULL, &dstrect);
		
		TTF_CloseFont(font);
		SDL_DestroyTexture(texture);
		SDL_FreeSurface(surface);
	}

	else if (displayFPS == FPS_TERMINAL){
		printf("%i\n", (int) round(1.0/frameTime));
	}

	SDL_RenderPresent(renderer);
	//RenderBackground();

	//speed modifiers
	*moveSpeed = frameTime * 10.0; //the constant value is in squares/second
	*rotSpeed = frameTime * 6.0; //the constant value is in radians/second
	
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
