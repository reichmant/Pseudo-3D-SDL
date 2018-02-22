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
// Required SDL and standard IO libraries
#include "SDL2/SDL.h"
#include "stdio.h"
#include "stdlib.h"  // rand(), srand()
#include "time.h"    // time()
#include "pthread.h"


#define WORLD_WIDTH 5
#define WORLD_HEIGHT 5

int worldMap[WORLD_WIDTH][WORLD_HEIGHT]=
{
  {1,1,1,1,1},
  {1,0,0,1,1},
  {1,0,0,0,1},
  {1,1,0,0,1},
  {1,1,1,1,1}
};


// Function Declarations
void DrawPixel(int r, int g, int b, int x, int y);
int ValidateColorRange(int color);
void RenderBackground();
void CheckForInput();
void RenderSquare();

// Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Globals
SDL_Renderer *renderer;
SDL_Event event;
SDL_Window *window;

int main(void) {


    /* Setup stuff */
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
    RenderBackground();
    /* End setup */

    /* Thread fun */
    pthread_t input_thread; //this variable is our reference to the second thread
    
    if(pthread_create(&input_thread, NULL, CheckForInput, NULL)) { //create a second thread which executes CheckForInput()
    	fprintf(stderr, "Error creating thread\n");
    	return 1;
    }

    //*****************************************************************************************//
    // The major part of the rendering will be done here.
    //     Ideally, we'll have a self-contained set of functions for the ray casting algorithm
    //     The grid of pixels will be passed back here once the entire frame is rendered
    //     We'll fill the window with the rendered frame and update accordingly
    //     This will be a loop: render frame, pass to SDL and display it.
    //      (Then render the next frame, etc.)
    //*****************************************************************************************//

    /* Perform raycast */
    RayCast();

    if(pthread_join(input_thread, NULL)) {
    	fprintf(stderr, "Error joining thread\n");
    	return 2;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
    
}

void RayCast(){
	/*
	// Where we are
	double player_posX = 3;
	double player_posY = 3;
	// Where we're facing
	double directionX = -1;
	double directionY = 0;
	// camera plane position
	double planeX = 0;
	double planeY = 0.66;
	// time between frames
	double currentFrame = 0;
	double lastFrame = 0;
	*/

	double posX = 22, posY = 12;  //x and y start position
	double dirX = -1, dirY = 0; //initial direction vector
	double planeX = 0, planeY = 0.66; //the 2d raycaster version of camera plane

	double time = 0; //time of current frame
	double oldTime = 0; //time of previous frame

	double w;

	while(1){
		for(int x = 0; w < SCREEN_WIDTH; x++){
			//calculate ray position and direction
		    double cameraX = 2 * x / w - 1; //x-coordinate in camera space
		    double rayDirX = dirX + planeX * cameraX;
		    double rayDirY = dirY + planeY * cameraX;
		    //which box of the map we're in
            int mapX = (int) posX;
	        int mapY = (int) posY;

	        //length of ray from current position to next x or y-side
	        double sideDistX;
	        double sideDistY;

	        //length of ray from one x or y-side to next x or y-side
	        double deltaDistX = abs(1 / rayDirX);
	        double deltaDistY = abs(1 / rayDirY);
	        double perpWallDist;

	        //what direction to step in x or y-direction (either +1 or -1)
	        int stepX;
	        int stepY;

	        int hit = 0; //was there a wall hit?
	        int side; //was a NS or a EW wall hit?
	        //perform DDA
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
	            //Check if ray has hit a wall
	            if (worldMap[mapX][mapY] > 0) hit = 1;
			}
	        //Calculate distance projected on camera direction (Euclidean distance will give fisheye effect!)
	        if (side == 0) perpWallDist = (mapX - posX + (1 - stepX) / 2) / rayDirX;
	        else           perpWallDist = (mapY - posY + (1 - stepY) / 2) / rayDirY;

	        //Calculate height of line to draw on screen
	        double h;
	        int lineHeight = (int)(h / perpWallDist);

	        //calculate lowest and highest pixel to fill in current stripe
	        int drawStart = -lineHeight / 2 + h / 2;
	        if(drawStart < 0)drawStart = 0;
	        int drawEnd = lineHeight / 2 + h / 2;
	        if(drawEnd >= h)drawEnd = h - 1;

	        /*
	        //choose wall color
	        switch(worldMap[mapX][mapY])
	        	{
	                case 1:  color = RGB_Red;  break; //red
	                case 2:  color = RGB_Green;  break; //green
	                case 3:  color = RGB_Blue;   break; //blue
	                case 4:  color = RGB_White;  break; //white
	                default: color = RGB_Yellow; break; //yellow
	            }

	            

	        //give x and y sides different brightness
	        if (side == 1) {color = color / 2;}

	        //draw the pixels of the stripe as a vertical line
	        verLine(x, drawStart, drawEnd, color);
	        */

			for (int y = 0; y < SCREEN_HEIGHT; y++){
				DrawPixel(128, 128, 128, x, y);
			}
	    }
	    //timing for input and FPS counter
	    oldTime = time;
		time = 12;
	    double frameTime = (time - oldTime) / 1000.0; //frameTime is the time this frame has taken, in seconds
	    SDL_RenderPresent(renderer);

	    //speed modifiers
	    double moveSpeed = frameTime * 5.0; //the constant value is in squares/second
	    double rotSpeed = frameTime * 3.0; //the constant value is in radians/second
	    
	    /*

	    //move forward if no wall in front of you
	    if (keyDown(SDLK_UP)){
	    	if(worldMap[(int) (posX + dirX * moveSpeed)][(int) (posY)] == 0) posX += dirX * moveSpeed;
	    	if(worldMap[(int) (posX)][(int)(posY + dirY * moveSpeed)] == 0) posY += dirY * moveSpeed;
	    }
		//move backwards if no wall behind you
		if (keyDown(SDLK_DOWN)){
	    	if(worldMap[(int)(posX - dirX * moveSpeed)][(int)(posY)] == 0) posX -= dirX * moveSpeed;
	    	if(worldMap[(int)(posX)][(int)(posY - dirY * moveSpeed)] == 0) posY -= dirY * moveSpeed;
	    }	 
	    //rotate to the right
	    if (keyDown(SDLK_RIGHT)){
	        //both camera direction and camera plane must be rotated
	        double oldDirX = dirX;
	        dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
	        dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
	        double oldPlaneX = planeX;
	        planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
	    	planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
	    }
	    //rotate to the left
	    if (keyDown(SDLK_LEFT)){
	        //both camera direction and camera plane must be rotated
	        double oldDirX = dirX;
	        dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
	        dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
	        double oldPlaneX = planeX;
	        planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
			planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
	    }
	    */
	}
}

void RenderSquare(){
	int squareDim = 90;
	int squareR = 128;
	int squareG = 128;
	int squareB = 128;
	int x;
	int y;

	for (x = (SCREEN_WIDTH/2)-(squareDim/2); x < ((SCREEN_WIDTH/2)-(squareDim/2))+(squareDim); x++){
	        for (y = (SCREEN_HEIGHT/2)-(squareDim/2); y < ((SCREEN_HEIGHT/2)-(squareDim/2))+(squareDim); y++){
	            DrawPixel(ValidateColorRange(squareR), ValidateColorRange(squareG), ValidateColorRange(squareB), x, y);
	        }
	    }
	/* Update screen */
	SDL_RenderPresent(renderer);
}

/* Draw function abstraction */
void DrawPixel(int r, int g, int b, int x, int y){
	SDL_SetRenderDrawColor(renderer, r, g, b, 255); // draw in green
	SDL_RenderDrawPoint(renderer, x, y);
}

/* Assert acceptable color values */
int ValidateColorRange(int color){
    if (color > 256) color = 256;
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

/* Close button functionality */
void CheckForInput(){
    // If you make it here, we're just waiting for input on the X button
	int quit = 0;
	while( !quit ){
    	    /* Poll for events */
            while( SDL_PollEvent( &event ) ){    
           	    switch( event.type ){
               	    /* Keyboard event */
                   	case SDL_KEYUP:
                       	printf("Hey, you pressed up!\n");
                       	break;

	                /* SDL_QUIT event (window close) */
    	            case SDL_QUIT:
        	            quit = 1;
               	        break;

                   	default:
                       	break;
	            }
    	    }
        }
    // The actual closing
    //SDL_DestroyRenderer(renderer);
    //SDL_DestroyWindow(window);
    //SDL_Quit();
//    return EXIT_SUCCESS;
}