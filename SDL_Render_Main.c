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

// Function Declarations
void DrawPixel(int r, int g, int b, int x, int y);
int ValidateColorRange(int color);


// Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Globals
SDL_Renderer *renderer;

int main(void) {
    SDL_Event event;
    SDL_Window *window;
    int x;
    int y;
    /* Setup stuff */
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);

    /* Render background */
    //SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); // draw in: black
    //SDL_RenderClear(renderer); // fill screen with black

    /*
    for (x = 0; x < SCREEN_WIDTH; x++){
        for (y = 0; y < SCREEN_HEIGHT/5; y++){
            DrawPixel(60, 40, 0, x, y);
        }
    }

    for (x = 0; x < SCREEN_WIDTH; x++){
        for (y = SCREEN_HEIGHT/5; y < (SCREEN_HEIGHT/5)*2; y++){
            DrawPixel(30, 20, 0, x, y);
        }
    }

    for (x = 0; x < SCREEN_WIDTH; x++){
        for (y = (SCREEN_HEIGHT/5)*3; y < (SCREEN_HEIGHT/5)*4; y++){
            DrawPixel(30, 20, 0, x, y);
        }
    }

    for (x = 0; x < SCREEN_WIDTH; x++){
        for (y = (SCREEN_HEIGHT/5)*4; y < SCREEN_HEIGHT; y++){
            DrawPixel(60, 40, 0, x, y);
        }
    }
    */
    //har str[12];
    //int poo = sprintf(str, "%d", 42);
    int poo = 32;
    //printf("%d\n", poo);

    int MAXpartition = 9; // must be odd for proper symmetry
    int R = 60;
    int G = 40;
    int B = 0;
    int partition;
    for (partition = 0; partition < MAXpartition+1; partition++){
        for (x = 0; x < SCREEN_WIDTH; x++){
            //sprintf( "\n", "%d", ((SCREEN_HEIGHT/MAXpartition)*partition));
            //sprintf( "\n", "%d", ((SCREEN_HEIGHT/MAXpartition)*partition+1));
            //printf("\n");
            for (y = (SCREEN_HEIGHT/MAXpartition)*(partition); y < (SCREEN_HEIGHT/MAXpartition)*(partition+1); y++){
                poo = ((SCREEN_HEIGHT/MAXpartition)*partition);
                poo = ((SCREEN_HEIGHT/MAXpartition)*(partition+1));
                //printf("%d\n", poo);
                DrawPixel(ValidateColorRange(R), ValidateColorRange(G), ValidateColorRange(B), x, y);
            }
        }
        if (partition > (MAXpartition/2)-1){
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


    /* Update screen */
    SDL_RenderPresent(renderer);


    /* Close button functionality */
    // If you make it here, we're just waiting for input on the X button
    while (1) {
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;
    }
    // The actual closing
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
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


//??? getRaycastData()