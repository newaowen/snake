#include "sdl_util.h"

SDL_Surface* sdl_init(int width, int height) {
	SDL_Init( SDL_INIT_EVERYTHING );
	SDL_Surface* surface = SDL_SetVideoMode(width, height, 32, SDL_SWSURFACE | SDL_DOUBLEBUF);

	return surface;
}

TTF_Font* sdl_init_font(char* font_name) {
	// Initialize SDL_ttf library
	if (TTF_Init() != 0) {
		printf("TTF_Init() Failed: %s", TTF_GetError());
		SDL_Quit();
		exit(1);
	}

	// Load a font
	TTF_Font *font;
	font = TTF_OpenFont(font_name, 24);   	
	if (font == NULL) {
		printf("TF_OpenFont() Failed: %s", TTF_GetError());
		TTF_Quit();
		SDL_Quit();
		exit(1);
	}

	return font;
}

void sdl_set_title(char* s) {
	SDL_WM_SetCaption(s, 0 );
}

SDL_Surface* sdl_draw_text(TTF_Font* font, char* s, int color) {
	SDL_Surface *text;
	SDL_Color text_color = {255, 255, 255};
	text = TTF_RenderText_Solid(font, s, text_color);
	if (text == NULL) {
		printf("TTF_RenderText_Solid() Failed: %s", TTF_GetError());
		TTF_Quit();
		SDL_Quit();
		exit(1);
	}
	return text;
}

sdl_color sdl_get_color(int color) {
	sdl_color c;
	c.r = (color & 0xff000000) >> 24;
    c.g = (color & 0x00ff0000) >> 16; 
    c.b = (color & 0x0000ff00) >> 8; 
    c.a = (color & 0x000000ff); 
	return c;
}

void sdl_draw_line(SDL_Surface* surface, int x0, int y0, int x1, int y1, int color) {
	sdl_color c = sdl_get_color(color);
    lineRGBA(surface, x0, y0, x1, y1, c.r, c.g, c.b, c.a);
}

void sdl_draw_box(SDL_Surface* surface, int left, int top, int right, int bottom, int color) {
	sdl_color c = sdl_get_color(color);
	boxRGBA(surface, left, top, right, bottom, c.r, c.g, c.b, c.a);
}

