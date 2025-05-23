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
#include <math.h>
#include "games.h"

#define MAX_WOODS 100000


int mouseX, mouseY, mouseXI, mouseYI, placeX, placeY;
int woods_count = 0;
int lives = 3;

int window_width;
int window_height;

typedef struct {
    SDL_Rect rect;
    int speed;
} Player;

typedef struct {
    SDL_Rect rect;
    int active;
} Wood;

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

    TTF_Font* font = TTF_OpenFont("fonts/Roboto-VariableFont_wdth,wght.ttf", 32);

    if (font == NULL) {
        show_mini_window("Game", "Font loading error!");
        return 1;
    }

    Wood woods[MAX_WOODS];

    for (int i = 0; i < MAX_WOODS; i++) {
        woods[i].active = 0;
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
            SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP
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


    SDL_Surface* playerSurface = IMG_Load("textures/player.png");
    SDL_Surface* backroundSurface = IMG_Load("textures/backround.png");
    SDL_Surface* woodSurface = IMG_Load("textures/wood.png");
    if (!playerSurface && !backroundSurface && !woodSurface) {
        show_mini_window("Game", "Images loading error!");
        return 1;
    }

    SDL_Texture* playerTexture = SDL_CreateTextureFromSurface(renderer, playerSurface);
    SDL_Texture* backroundTexture = SDL_CreateTextureFromSurface(renderer, backroundSurface);
    SDL_Texture* woodTexture = SDL_CreateTextureFromSurface(renderer, woodSurface);
    SDL_FreeSurface(playerSurface);
    SDL_FreeSurface(backroundSurface);
    SDL_FreeSurface(woodSurface);

    if (!playerTexture && !backroundTexture && !woodTexture) {
        show_mini_window("Game", "Textures creation error!");
        return 1;
    }
    char buffer[100];
    SDL_Rect door = {doorBeginX, doorBeginY, 100, 100};
    SDL_Rect textRect = {0, 0, 300, 50};
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

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = SDL_FALSE;
            }
        }
        Uint32 buttons = SDL_GetMouseState(&mouseX, &mouseY);

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

        if (lives <= 0) {
            show_mini_window("Info", "You died!");
            running = SDL_FALSE;
        }

        if (SDL_HasIntersection(&player.rect, &door)) {
            show_mini_window("YOU WIN!!!!", "You're so super!");
            running = SDL_FALSE;
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, backroundTexture, NULL, &backroundRect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_GetMouseState(&mouseXI, &mouseYI);
        mouseX = round(mouseXI / 100.0) * 100;
        mouseY = round(mouseYI / 100.0) * 100;
        int detectWoodPosition(int x, int y, SDL_Rect* rect) {
            return x >= rect->x && x < rect->x + rect->w &&
                   y >= rect->y && y < rect->y + rect->h;
        }

        SDL_Rect blockCursor = {mouseX, mouseY, 100, 100};
        SDL_RenderDrawRect(renderer, &blockCursor);
        if (buttons & SDL_BUTTON_RMASK) {
            if (woods_count < MAX_WOODS) {
                woods[woods_count].rect.x = mouseX;
                woods[woods_count].rect.y = mouseY;
                woods[woods_count].rect.w = 100;
                woods[woods_count].rect.h = 100;
                woods[woods_count].active = 1;
                woods_count++;
            }
        }   else if (buttons & SDL_BUTTON_LMASK) {
                for (int i = 0; i < woods_count; i++) {
                    if (woods[i].active && detectWoodPosition(mouseX, mouseY, &woods[i].rect)) {
                        if (i != woods_count - 1) {
                            woods[i] = woods[woods_count - 1];
                        }
                        woods[woods_count - 1].active = 0;
                        woods_count--;
                        break;
                    }
                }
        }


        for (int i = 0; i < woods_count; i++) {
            if (woods[i].active) {
                SDL_RenderCopy(renderer, woodTexture, NULL, &woods[i].rect);
            }
        }


        sprintf(buffer, "AdventureCraft Alpha, lives %d", lives);

        SDL_Surface* fontSurface = TTF_RenderText_Solid(font, buffer, textcolor);
        SDL_Texture* fontTexture = SDL_CreateTextureFromSurface(renderer, fontSurface);
        SDL_FreeSurface(fontSurface);
        SDL_RenderCopy(renderer, fontTexture, NULL, &textRect);

        SDL_RenderCopy(renderer, playerTexture, NULL, &player.rect);
        SDL_DestroyTexture(fontTexture);

        SDL_RenderPresent(renderer);

        SDL_Delay(16);

    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyTexture(backroundTexture);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
