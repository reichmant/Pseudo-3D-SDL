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

// Function Declarations
void DrawPixel(int r, int g, int b, int x, int y);
int ValidateColorRange(int color);
void RenderBackground();
void WaitForClose();

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

    //*****************************************************************************************//
    // The major part of the rendering will be done here.
    //     Ideally, we'll have a self-contained set of functions for the ray casting algorithm
    //     The grid of pixels will be passed back here once the entire frame is rendered
    //     We'll fill the window with the rendered frame and update accordingly
    //     This will be a loop: render frame, pass to SDL and display it.
    //      (Then render the next frame, etc.)
    //*****************************************************************************************//

    /* Perform raycast */
    //getRaycastData();








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

    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_RIGHT]) {
        printf("<RETURN> is pressed.\n");
    }





    /* Update screen */
    SDL_RenderPresent(renderer);

    WaitForClose();
    
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
void WaitForClose(){
    // If you make it here, we're just waiting for input on the X button
int quit = 0;
while( !quit ){
            /* Poll for events */
            while( SDL_PollEvent( &event ) ){    
                switch( event.type ){
                    /* Keyboard event */
                    /* Pass the event data onto PrintKeyInfo() */
                    case SDL_KEYDOWN:
                    case SDL_KEYUP:
                        quit = 1;
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
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
//??? getRaycastData()
