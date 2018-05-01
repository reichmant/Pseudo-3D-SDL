// Screen dimension defualts
int SCREEN_WIDTH = 640; //640
int SCREEN_HEIGHT = 480; //480
int FPS_LIMIT = 999;


// Globals
SDL_Renderer *renderer;
SDL_Event event;
SDL_Window *window;

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

/* Assert acceptable color values */
int ValidateColorRange(int color){
	if (color > 255) color = 255;
	else if (color < 0) color = 0;
	return color;
}

/* Draw function abstraction */
void DrawPixel(int r, int g, int b, int x, int y){
	SDL_SetRenderDrawColor(renderer, r, g, b, 255); // draw in told color
	SDL_RenderDrawPoint(renderer, x, y);
}

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

	SDL_RenderDrawLine(renderer, x, y1, x, y2);

/*
	for(int y = y1; y <= y2; y++)
	{
	 SDL_RenderDrawPoint(renderer, x, y);
	}
	return 1;
*/
}