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

#define MAX_BLOCKS 171

int mouseX, mouseY, mouseXI, mouseYI;
int blocks_count = 0;
int window_width, window_height;
int buttonid;

typedef struct {
    SDL_Rect rect;
    int speed;
    int lives;
} Player;

typedef struct {
    SDL_Rect rect;
    int active;
} Block;

typedef struct {
    SDL_Rect rect;
    int active;
} Block_cursor;

int count_placed_blocks(Block* blocks, int blocks_count) {
    int count = 0;
    for (int i = 0; i < blocks_count; i++) {
        if (blocks[i].active) {
            count++;
        }
    }
    return count;
}

int main (){
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_bool running = SDL_TRUE;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        show_mini_window("Fatal", "Failed to initialize a graphics. Game crashed with code 1");
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        show_mini_window("Fatal", "Failed to initialize a SDL2_IMG lib. Game crashed with code 1");
        return 1;
    }

    if (TTF_Init() < 0) {
        show_mini_window("Fatal", "Failed to initialize a SDL2_TTF lib. Game crashed with code 1");
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("fonts/font.ttf", 32);
    if (font == NULL) {
        show_mini_window("Fatal", "Failed to open a font. Game crashed with code 1");
        return 1;
    }

    Block blocks[MAX_BLOCKS];
    for (int i = 0; i < MAX_BLOCKS; i++) {
        blocks[i].active = 0;
    }

    window_width = 1900;
    window_height = 900;
    window = SDL_CreateWindow("AdventureCraft", SDL_WINDOWPOS_CENTERED, 0, window_width, window_height, SDL_WINDOW_SHOWN);

    if (!window) {
        show_mini_window("Fatal", "Failed to create a window. Game crashed with code 1");
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_DestroyWindow(window);
        show_mini_window("Fatal", "Failed to create a render. Game crashed with code 1");
        SDL_Quit();
        return 1;
    }

    SDL_Color textcolor = {0, 0, 0, 0};

    SDL_Surface* playerSurface = IMG_Load("textures/player.png");
    SDL_Surface* blockSurface = IMG_Load("textures/block.png");
    
    if (!playerSurface || !blockSurface) {
        SDL_DestroyWindow(window);
        show_mini_window("Fatal", "Failed to load images. Game crashed with code 1");
        return 1;
    }

    SDL_Texture* playerTexture = SDL_CreateTextureFromSurface(renderer, playerSurface);
    SDL_Texture* blockTexture = SDL_CreateTextureFromSurface(renderer, blockSurface);
    
    SDL_FreeSurface(playerSurface);
    SDL_FreeSurface(blockSurface);

    if (!playerTexture || !blockTexture) {
        SDL_DestroyWindow(window);
        show_mini_window("Fatal", "Failed to create textures. Game crashed with code 1");
        return 1;
    }
    char buffer1[128];
    char buffer2[128];

    SDL_Rect infoTextRect = {0, 0, 700, 50};
    SDL_Rect livesTextRect = {0, 50, 250, 50};

    Player player = {
        .rect = {window_width/2 - 30, window_height/2 - 50, 61, 100},
        .speed = 5,
        .lives = 3
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

        if (player.lives <= 0) {
            show_mini_window("Info", "You died.");
            running = SDL_FALSE;
        }

        if (key_status[SDL_SCANCODE_ESCAPE]) {
            running = SDL_FALSE;
        }
        int old_x = player.rect.x;
        int old_y = player.rect.y;

        int new_x = player.rect.x;
        if (key_status[SDL_SCANCODE_A] && player.rect.x > 0) {
            new_x -= player.speed;
        }
        if (key_status[SDL_SCANCODE_D] && player.rect.x < window_width - player.rect.w) {
            new_x += player.speed;
        }

        player.rect.x = new_x;
        int collision = 0;
        for (int i = 0; i < blocks_count; i++) {
            if (blocks[i].active && SDL_HasIntersection(&player.rect, &blocks[i].rect)) {
                collision = 1;
                break;
            }
        }
        if (collision) player.rect.x = old_x;

        int new_y = player.rect.y;
        if (key_status[SDL_SCANCODE_W] && player.rect.y > 0) {
            new_y -= player.speed;
        }
        if (key_status[SDL_SCANCODE_S] && player.rect.y < window_height - player.rect.h) {
            new_y += player.speed;
        }

        int backup_x = player.rect.x;
        player.rect.y = new_y;
        collision = 0;
        for (int i = 0; i < blocks_count; i++) {
            if (blocks[i].active && SDL_HasIntersection(&player.rect, &blocks[i].rect)) {
                collision = 1;
                break;
            }
        }
        if (collision) player.rect.y = old_y;
        player.rect.x = backup_x;

        SDL_SetRenderDrawColor(renderer, 155, 155, 155, 0);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_GetMouseState(&mouseXI, &mouseYI);
        mouseX = (mouseXI / 100) * 100;
        mouseY = (mouseYI / 100) * 100;

        Block_cursor  blockCursor = {
            .rect = {mouseX, mouseY, 100, 100},
            .active = 1
        };

        if (blockCursor.rect.x >= player.rect.x + 300 || blockCursor.rect.y >= player.rect.y + 300 || blockCursor.rect.x <= player.rect.x - 300 || blockCursor.rect.y <= player.rect.y - 300) {
            blockCursor.active = 0;
        }

        if (blockCursor.active) {
            SDL_RenderDrawRect(renderer, &blockCursor.rect);
        }

        if (buttons & SDL_BUTTON_RMASK) {
            int collisionBlock = 0;
            int inside =
                blockCursor.rect.x >= 0 &&
                blockCursor.rect.y >= 0 &&
                blockCursor.rect.x + blockCursor.rect.w <= window_width &&
                blockCursor.rect.y + blockCursor.rect.h <= window_height;

            for (int i = 0; i < blocks_count; i++) {
                if (blocks[i].active && SDL_HasIntersection(&blockCursor.rect, &blocks[i].rect)) {
                    collisionBlock = 1;
                    break;
                }
            }
            if (inside && blockCursor.active && !SDL_HasIntersection(&blockCursor.rect, &player.rect) && !collisionBlock && blocks_count < MAX_BLOCKS) {
                blocks[blocks_count].rect = blockCursor.rect;
                blocks[blocks_count].active = 1;
                blocks_count++;
            }

        }

        if (buttons & SDL_BUTTON_LMASK){
            for (int i = 0; i < blocks_count; i++) {
                if (blocks[i].active && blockCursor.active &&
                    mouseX >= blocks[i].rect.x && mouseX < blocks[i].rect.x + blocks[i].rect.w &&
                    mouseY >= blocks[i].rect.y && mouseY < blocks[i].rect.y + blocks[i].rect.h) {
                    blocks[i] = blocks[blocks_count - 1];
                    blocks_count--;
                    break;
                }
            }
        }

        for (int i = 0; i < blocks_count; i++) {
            if (blocks[i].active) {
                SDL_RenderCopy(renderer, blockTexture, NULL, &blocks[i].rect);
            }
        }

        SDL_RenderCopy(renderer, playerTexture, NULL, &player.rect);

        sprintf(buffer1, "AdventureCraft BLOCK_TEST3");

        SDL_Surface* fontSurface1 = TTF_RenderText_Solid(font, buffer1, textcolor);
        SDL_Texture* fontTexture1 = SDL_CreateTextureFromSurface(renderer, fontSurface1);

        sprintf(buffer2, "Lives: %d", player.lives);

        SDL_Surface* fontSurface2 = TTF_RenderText_Solid(font, buffer2, textcolor);
        SDL_Texture* fontTexture2 = SDL_CreateTextureFromSurface(renderer, fontSurface2);

        SDL_RenderCopy(renderer, fontTexture1, NULL, &infoTextRect);
        SDL_RenderCopy(renderer, fontTexture2, NULL, &livesTextRect);

        SDL_FreeSurface(fontSurface1);
        SDL_FreeSurface(fontSurface2);

        SDL_DestroyTexture(fontTexture1);
        SDL_DestroyTexture(fontTexture2);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyTexture(blockTexture);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return 0;
}