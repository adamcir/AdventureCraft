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
#include <SDL2/SDL_ttf.h>
#include "games.h"


int window_width;
int window_height;

typedef struct {
    SDL_Rect rect;
    int speed;
} Player;

int buttonid;


int main (){
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

    if (TTF_Init() < 0) {
        show_mini_window("Game", "SDL_ttf init error!");
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("fonts/ShareTech-Regular.ttf", 32);

    if (font == NULL) {
        show_mini_window("Game", "Font loading error!");
        return 1;
    }

    int doorBeginX;
    int doorBeginY;

    buttonid = show_mini_window_OC("Game", "Do you want FULLSCREEN mode?");
    if (BUTTON_OK == buttonid) {
        window_width = 1920;
        window_height = 1080;
        doorBeginX = 1820;
        doorBeginY = 980;
        window = SDL_CreateWindow(
            "AdventureCraft",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            window_width,
            window_height,
            SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP  // přidán flag pro fullscreen
        );
    } else {
        window_width = 800;
        window_height = 600;
        doorBeginX = 700;
        doorBeginY = 500;
        window = SDL_CreateWindow(
            "AdventureCraft",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            window_width,
            window_height,
            SDL_WINDOW_SHOWN
        );
        ;
    }


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

    SDL_Color textcolor = {0, 0,0 , 0};


    SDL_Surface* doorSurface = IMG_Load("textures/door.png");
    SDL_Surface* playerSurface = IMG_Load("textures/player.png");
    SDL_Surface* backroundSurface = IMG_Load("textures/backround.png");
    if (!playerSurface && !doorSurface) {
        show_mini_window("Game", "Images loading error!");
        return 1;
    }

    SDL_Texture* playerTexture = SDL_CreateTextureFromSurface(renderer, playerSurface);
    SDL_Texture* doorTexture = SDL_CreateTextureFromSurface(renderer, doorSurface);
    SDL_Texture* backroundTexture = SDL_CreateTextureFromSurface(renderer, backroundSurface);
    SDL_FreeSurface(playerSurface);
    SDL_FreeSurface(doorSurface);
    SDL_FreeSurface(backroundSurface);

    if (!playerTexture) {
        show_mini_window("Game", "Textures creation error!");
        return 1;
    }
    char buffer[100];
    SDL_Rect door = {doorBeginX, doorBeginY, 100, 100};
    SDL_Rect textRect = {0, 0, 90, 50};
    SDL_Rect backroundRect = {0, 0, window_width, window_height};

    Player player = {
        .rect = {
            .x = 0,
            .y = 0,
            .w = 61,
            .h = 100
        },
        .speed = 5
    };

    buttonid = 0;
    show_mini_window("Game", "Welcome to AdventureCraft!");
    show_mini_window("Game", "Press WASD to move");
    buttonid = show_mini_window_OC("Game", "Are you ready!");
    if (BUTTON_OK != buttonid) {
        show_mini_window("Game", "Bye!");
        return 0;
    }


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
        if (player.rect.x > window_width - player.rect.w) player.rect.x = window_width - player.rect.w;
        if (player.rect.y > window_height - player.rect.h) player.rect.y = window_height - player.rect.h;

        if (SDL_HasIntersection(&player.rect, &door)) {
            show_mini_window("YOU WIN!!!!", "You're so super!");
            running = SDL_FALSE;
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, backroundTexture, NULL, &backroundRect);

        sprintf(buffer, "pos: %d, %d", player.rect.x, player.rect.y);
        SDL_Surface* fontSurface = TTF_RenderText_Solid(font, buffer, textcolor);
        SDL_Texture* fontTexture = SDL_CreateTextureFromSurface(renderer, fontSurface);
        SDL_FreeSurface(fontSurface);

        SDL_RenderCopy(renderer, fontTexture, NULL, &textRect);

        SDL_RenderCopy(renderer, doorTexture, NULL, &door);
        SDL_RenderCopy(renderer, playerTexture, NULL, &player.rect);
        SDL_DestroyTexture(fontTexture);

        SDL_RenderPresent(renderer);

        SDL_Delay(16);

    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyTexture(doorTexture);
    SDL_DestroyTexture(backroundTexture);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
