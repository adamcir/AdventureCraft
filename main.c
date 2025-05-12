/*
 * Copyright (C) 2025 Adam Cír
 *
 * AdventureCraft
 *
 * This file is part of AdventureCraft.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * NOT ALLOWED:
 * - Use this code in closed-source software
 * - Remove or modify author information
 *
 * Author: Adam Cír
 * Email: adam.cir@ptw.cz
 */


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

typedef struct {
    SDL_Rect rect;
    int speed;
} Player;


void show_mini_window(const char* title, const char* text) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,  // typ zprávy (ERROR, WARNING, INFORMATION)
                           title,                 // titulek okna
                           text,                  // text zprávy
                           NULL);

}

int main() {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_bool running = SDL_TRUE;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        show_mini_window("Game", "Init error!");
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        show_mini_window("Game", "SDL_image init error!");
        return 1;
    }

    SDL_Rect door = {700, 500, 100, 100};

    window = SDL_CreateWindow(
        "Game",                  // název okna
        SDL_WINDOWPOS_CENTERED,          // pozice X
        SDL_WINDOWPOS_CENTERED,          // pozice Y
        WINDOW_WIDTH,                    // šířka
        WINDOW_HEIGHT,                   // výška
        SDL_WINDOW_SHOWN                 // flags
    );

    if (!window) {
        show_mini_window("Game", "Window error!");
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        show_mini_window("Game", "Renderer error!");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_Surface* doorSurface = IMG_Load("textures/door.png");
    SDL_Surface* playerSurface = IMG_Load("textures/player.png");

    if (!playerSurface && !doorSurface) {
        show_mini_window("Game", "Images loading error!");
        return 1;
    }

    SDL_Texture* playerTexture = SDL_CreateTextureFromSurface(renderer, playerSurface);
    SDL_Texture* doorTexture = SDL_CreateTextureFromSurface(renderer, doorSurface);
    SDL_FreeSurface(playerSurface);
    SDL_FreeSurface(doorSurface);

    if (!playerTexture) {
        show_mini_window("Game", "Textures creation error!");
        return 1;
    }


    Player player = {
        .rect = {
            .x = 0,
            .y = 0,
            .w = 61,
            .h = 100
        },
        .speed = 5
    };

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = SDL_FALSE;
            }
        }
        const Uint8* key_status = SDL_GetKeyboardState(NULL);
        if (key_status[SDL_SCANCODE_ESCAPE]) {
            running = SDL_FALSE;
        }
        if (key_status[SDL_SCANCODE_W]) {
            player.rect.y -= player.speed;
        }

        if (key_status[SDL_SCANCODE_S]) {
            player.rect.y += player.speed;
        }
        if (key_status[SDL_SCANCODE_A]) {
            player.rect.x -= player.speed;
        }
        if (key_status[SDL_SCANCODE_D]) {
            player.rect.x += player.speed;
        }

        if (player.rect.x < 0) player.rect.x = 0;
        if (player.rect.y < 0) player.rect.y = 0;
        if (player.rect.x > 800 - player.rect.w) player.rect.x = 800 - player.rect.w;
        if (player.rect.y > 600 - player.rect.h) player.rect.y = 600 - player.rect.h;

        if (SDL_HasIntersection(&player.rect, &door)) {
            show_mini_window("YOU WIN!!!!", "You're so super!");
            running = SDL_FALSE;
        }


        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, doorTexture, NULL, &door);
        SDL_RenderCopy(renderer, playerTexture, NULL, &player.rect);

        SDL_RenderPresent(renderer);

        SDL_Delay(16);

    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
