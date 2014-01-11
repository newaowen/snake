#ifndef SDL_UTIL
#define SDL_UTIL

#include <SDL.h>   /* All SDL App's need this */
#include <SDL_image.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_ttf.h>

extern SDL_Surface* sdl_init(int width, int height);


extern TTF_Font* sdl_init_font(char* font_name);

extern void sdl_set_title(char* s);

extern SDL_Surface* sdl_draw_text(TTF_Font* font, char* s, int color);

extern void sdl_draw_line(SDL_Surface* surface, int x0, int y0, int x1, int y1, int color);

extern void sdl_draw_box(SDL_Surface* surface, int left, int top, int right, int bottom, int color);

typedef struct {
    Uint8 r, g, b, a;
} sdl_color;

#endif

