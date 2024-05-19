#include "Swindow.h"

#include <stdio.h>

#include "SDL2/SDL.h"

void show(const char* title) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return;
    }

    SDL_Window* win = SDL_CreateWindow(title, 100, 100, 640, 480, SDL_WINDOW_SHOWN);
    if (win == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    int close = 0;

    while (!close)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
	        switch (event.type)
	        {
	        case SDL_QUIT:
                close = 1;
                break;
	        }

            SDL_SetRenderDrawColor(rend, 0, 0, 255, 255);
            SDL_RenderClear(rend);
            SDL_RenderPresent(rend);
        }
    }
}
