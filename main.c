#include "perlin.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include <SDL/SDL.h>

static double __c(double t) {return 3 * t*t - 2 * t*t*t;}

static double __i(double t, double x, double y) {
	return (1 - t) * x + t * y;
}

Uint32 to_pixel(SDL_PixelFormat *fmt, double value) {
	double v = (value + (double) 1) / (double) 2;
	int a = 255;
	double r, g, b;
	if(v <= 0.5) { // water
		double p = v / 0.5;
		r = 0;
		g = 0;
		b = __i(p, 0.5, 1);
	}
	else if(v <= 0.53) { // beach
		double p = (v - 0.5) / 0.03;
		p = sqrt(p);
		r = __i(p, 0, 1);
		g = __i(p, 0, 1);
		b = __i(p, 1, 0.5);
	}
	else if(v <= 0.56) { // to-grass
		double p = (v - 0.53) / 0.03;
		p = __c(p);
		r = __i(p, 1, 0.5);
		g = 1;
		b = 0.5;
	}
	else if(v <= 0.7) { // grass
		r = 0.5;
		g = 1;
		b = 0.5;
	}
	else if(v <= 0.71) { // to-mountain
		double p = (v - 0.7) / 0.01;
		r = 0.5;
		g = __i(p, 1, 0.5);
		b = 0.5;
	}
	else if(v <= 0.80) { // mountain
		double p = (v - 0.71) / 0.09;
		r = __i(p, 0.5, 1);
		g = __i(p, 0.5, 1);
		b = __i(p, 0.5, 1);
	}
	else { // snow
		r = 1;
		g = 1;
		b = 1;
	}
	return SDL_MapRGBA(fmt,
		(int)(r * (double) 255),
		(int)(g * (double) 255),
		(int)(b * (double) 255),
		a
	);
}

int main(int ac, char **av) {
	////
	// Width, height
	////
	int width = 600;
	int height = 600;
	
	////
	// Random generator init
	////
	srand((unsigned) time(0));
	
	////
	// Alloc
	////
	vec3_field
		*vf0 = v3f_random_unit(3, 4, 2),
		*vf1 = v3f_random_unit(6, 5, 2),
		*vf2 = v3f_random_unit(8, 11, 2),
		*vf3 = v3f_random_unit(19, 21, 2);
	heightmap
		*hm0 = hm_perlin_noise(width, height, vf0, 0.5),
		*hm1 = hm_perlin_noise(width, height, vf1, 0.5),
		*hm2 = hm_perlin_noise(width, height, vf2, 0.5),
		*hm3 = hm_perlin_noise(width, height, vf3, 0.5);
	heightmap
		*hm = hm_new(width, height);
	
	////
	// Process
	////
	hm_add_scale(hm, hm0, 0.50);
	hm_add_scale(hm, hm1, 0.25);
	hm_add_scale(hm, hm2, 0.15);
	hm_add_scale(hm, hm3, 0.10);
	
	////
	// SDL TIME! FREE BEERS FOR EVERYONE AND THEIR DOG!
	// (though, you're the one paying, okay?)
	////
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		printf("Error: SDL can't init.");
		return -1;
	}
	
	SDL_Surface *surface;
	int running = 1;
	SDL_Event evt;
	
	surface = SDL_SetVideoMode(width, height, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	if(surface == NULL) {
		printf("Error: Can't set video mode.");
		return -1;
	}
	
	SDL_WM_SetCaption("Perlin noise test", NULL);
	
	while(running) {
		while(SDL_PollEvent(&evt)) {
			if(evt.type == SDL_QUIT) {
				running = 0;
			}
		}
		for(int y = 0; y < height; y++) for(int x = 0; x < width; x++) {
			Uint8 *pixel = (Uint8 *) surface->pixels + y * surface->pitch + x * 4;
			*(Uint32 *) pixel = to_pixel(surface->format, hm_get(hm, x, y));
		}
		SDL_Flip(surface);
	}
	
	SDL_Quit();
	
	////
	// Dealloc
	////
	v3f_destroy(vf0);
	v3f_destroy(vf1);
	v3f_destroy(vf2);
	v3f_destroy(vf3);
	hm_destroy(hm0);
	hm_destroy(hm1);
	hm_destroy(hm2);
	hm_destroy(hm3);
	hm_destroy(hm);
}
