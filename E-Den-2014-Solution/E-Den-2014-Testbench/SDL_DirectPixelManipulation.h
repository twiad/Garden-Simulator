// SDL_DirectPixelManipulation.h

#ifndef SDL_DIRECTPIXELMANIPULATION_H
#define SDL_DIRECTPIXELMANIPULATION_H

#include "SDL.h"

Uint32 getpixel(SDL_Surface *surface, int x, int y);
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);

#endif