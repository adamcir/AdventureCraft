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
 * GitHub: https://github.com/adamcir/AdventureCraft/
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "games.h"

#define MAX_BLOCKS 171
#define VERSION "BLOCK_TEST5.1"

int mouseX, mouseY, mouseXI, mouseYI;
int blocks_count = 0;
int window_width, window_height;
int buttonid;
int currentBlockType = 0;

typedef struct {
    SDL_Rect rect;
    int speed;
    int lives;
} Player;

typedef struct {
    SDL_Rect rect;
    int active;
    int type;
} Block;

typedef struct {
    SDL_Rect rect;
    int active;
} Block_cursor;

typedef struct {
    char version[32];
    int blocks_count;
    Player player_data;
    Block blocks_data[MAX_BLOCKS];
} WorldSave;

void showLoadingScreen(SDL_Renderer* renderer, TTF_Font* font, const char* message, int progress, FILE* logFile) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    int centerX = window_width / 2;
    int centerY = window_height / 2;

    int loadingWidth = 600;
    int loadingHeight = 200;
    SDL_Rect loadingBg = {
        centerX - loadingWidth/2,
        centerY - loadingHeight/2,
        loadingWidth,
        loadingHeight
    };
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderFillRect(renderer, &loadingBg);

    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderDrawRect(renderer, &loadingBg);

    int progressWidth = 500;
    int progressHeight = 30;
    SDL_Rect progressBg = {
        centerX - progressWidth/2,
        loadingBg.y + 100,
        progressWidth,
        progressHeight
    };
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderFillRect(renderer, &progressBg);

    SDL_Rect progressBar = {
        progressBg.x + 5,
        progressBg.y + 5,
        (int)((progressWidth - 10) * (progress / 100.0f)),
        progressHeight - 10
    };
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &progressBar);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &progressBg);


    if (font) {
        SDL_Color white = {255, 255, 255, 255};
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, message, white);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (textTexture) {
                int textWidth, textHeight;
                SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);
                SDL_Rect textRect = {950 - textWidth/2, 400, textWidth, textHeight};
                SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                SDL_DestroyTexture(textTexture);
            }
            SDL_FreeSurface(textSurface);
        }
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(300);
    printf("INFO: Loading screen state >> %d\n", progress);
    fprintf(logFile , "INFO: Loading screen state >> %d\n", progress);
    printf("INFO: Loading screen message >> %s\n", message);
    fprintf(logFile , "INFO: Loading screen message >> %s\n", message);
}

int count_placed_blocks(Block* blocks, int blocks_count) {
    int count = 0;
    for (int i = 0; i < blocks_count; i++) {
        if (blocks[i].active) {
            count++;
        }
    }
    return count;
}

int saveWorld(const char* filename, Block* blocks, int blocks_count, Player* player, FILE* logFile) {
    FILE* saveFile = fopen(filename, "wb");
    if (saveFile == NULL) {
        printf("ERR: Failed to create save file >> %s\n", filename);
        fprintf(logFile, "ERR: Failed to create save file >> %s\n", filename);
        return 0;
    }

    WorldSave worldSave;
    strcpy(worldSave.version, VERSION);
    worldSave.blocks_count = blocks_count;
    worldSave.player_data = *player;

    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (i < blocks_count) {
            worldSave.blocks_data[i] = blocks[i];
        } else {
            worldSave.blocks_data[i].active = 0;
        }
    }

    size_t written = fwrite(&worldSave, sizeof(WorldSave), 1, saveFile);
    fclose(saveFile);

    if (written == 1) {
        printf("INFO: World saved >> %s\n", filename);
        fprintf(logFile, "INFO: World saved >> %s\n", filename);
        show_mini_window("Info", "World saved.");
        return 1;
    } else {
        printf("ERR: Failed to write save file >> %s\n", filename);
        fprintf(logFile, "ERR: Failed to write save file >> %s\n", filename);
        show_mini_window("Error", "Failed to write a save file.");
        return 0;
    }
}

int loadWorld(const char* filename, Block* blocks, int* blocks_count, Player* player, FILE* logFile) {
    FILE* saveFile = fopen(filename, "rb");
    if (saveFile == NULL) {
        printf("ERR: Failed to open save file >> %s\n", filename);
        fprintf(logFile, "ERR: Failed to open save file >> %s\n", filename);
        show_mini_window("Error", "Failed to open a save file.");
        return 0;
    }

    WorldSave worldSave;
    size_t read = fread(&worldSave, sizeof(WorldSave), 1, saveFile);
    fclose(saveFile);

    if (read != 1) {
        printf("ERR: Failed to read save file >> %s\n", filename);
        fprintf(logFile, "ERR: Failed to read save file >> %s\n", filename);
        show_mini_window("Error", "Failed to read a save file.");
        return 0;
    }

    if (strcmp(worldSave.version, VERSION) != 0) {
        printf("WARN: Save file version mismatch >> Expected: %s, Got: %s\n", VERSION, worldSave.version);
        fprintf(logFile, "WARN: Save file version mismatch >> Expected: %s, Got: %s\n", VERSION, worldSave.version);
        show_mini_window("Error", "Save file version mismatch. Trying to open a save.");
    }

    *blocks_count = worldSave.blocks_count;
    *player = worldSave.player_data;

    for (int i = 0; i < MAX_BLOCKS; i++) {
        blocks[i] = worldSave.blocks_data[i];
    }

    printf("INFO: World loaded >> %s (blocks: %d)\n", filename, *blocks_count);
    fprintf(logFile, "INFO: World loaded >> %s (blocks: %d)\n", filename, *blocks_count);
    show_mini_window("Info", "World loaded.");
    return 1;
}

int main () {
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    printf("INFO: Opening log file >> log.txt\n");

    FILE* logFile = fopen("AdventureCraft-log.aclog", "w");
    if (logFile == NULL) {
        printf("ERR: Failed to open >> log.txt\n");
        show_mini_window("Fatal", "Failed to open a log file. Game crashed with code 1");
        return 1;
    }
    printf("INFO: Game started >> %s\n", asctime(timeinfo));
    fprintf(logFile, "INFO: Game started >> %s\n", asctime(timeinfo));

    printf("MAIN: Starting >> AdventureCraft\n");
    fprintf(logFile, "MAIN: Starting >> AdventureCraft\n");

    printf("===========================\n");
    printf("     ADAVA SOFTWARE by\n");
    printf("       (C) Adam Cír\n");
    printf("===========================\n");
    printf("Copyright (C) 2025 Adam Cír\n");
    printf("AdventureCraft %s\n", VERSION);
    printf("===========================\n");
    printf("  This software is under\n");
    printf("     license GPL v3.0\n");
    printf("===========================\n");
    printf("GitHub: https://github.com/adamcir/AdventureCraft\n");

    fprintf(logFile, "===========================\n");
    fprintf(logFile, "     ADAVA SOFTWARE by\n");
    fprintf(logFile, "       (C) Adam Cír\n");
    fprintf(logFile, "===========================\n");
    fprintf(logFile, "Copyright (C) 2025 Adam Cír\n");
    fprintf(logFile, "AdventureCraft%s\n", VERSION);
    fprintf(logFile, "===========================\n");
    fprintf(logFile, "  This software is under\n");
    fprintf(logFile, "     license GPL v3.0\n");
    fprintf(logFile, "===========================\n");
    fprintf(logFile, "GitHub: https://github.com/adamcir/AdventureCraft\n");

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_bool running = SDL_TRUE;

    printf("INFO: Initializing >> SDL2 - Video\n");
    fprintf(logFile, "INFO: Initializing >> SDL2 - Video\n");
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        show_mini_window("Fatal", "Failed to initialize a graphics. Game crashed with code 1");
        printf("FATAL: Failed to initialize >> SDL2 - Video");
        fprintf(logFile, "FATAL: Failed to initialize >> SDL2 - Video");
        fclose(logFile);
        return 1;
    }

    printf("INFO: Initializing >> SDL2 - Images\n");
    fprintf(logFile, "INFO: Initializing >> SDL2 - Images\n");
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        show_mini_window("Fatal", "Failed to initialize a SDL2_IMG lib. Game crashed with code 1");
        printf("FATAL: Failed to initialize >> SDL2_IMG");
        fprintf(logFile, "FATAL: Failed to initialize >> SDL2_IMG");
        fclose(logFile);
        SDL_Quit();
        return 1;
    }

    if (TTF_Init() < 0) {
        show_mini_window("Fatal", "Failed to initialize a SDL2_TTF lib. Game crashed with code 1");
        printf("FATAL: Failed to initialize >> SDL2_TTF");
        fprintf(logFile, "FATAL: Failed to initialize >> SDL2_TTF");
        fclose(logFile);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    window_width = 1900;
    window_height = 900;
    window = SDL_CreateWindow("AdventureCraft", SDL_WINDOWPOS_CENTERED, 0, window_width, window_height, SDL_WINDOW_SHOWN);

    if (!window) {
        show_mini_window("Fatal", "Failed to create a window. Game crashed with code 1");
        printf("FATAL: Failed to create >> window");
        fprintf(logFile, "FATAL: Failed to create >> window");
        fclose(logFile);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_DestroyWindow(window);
        show_mini_window("Fatal", "Failed to create a render. Game crashed with code 1");
        printf("FATAL: Failed to create >> render");
        fprintf(logFile, "FATAL: Failed to create >> render");
        fclose(logFile);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    SDL_Delay(500);

    showLoadingScreen(renderer, NULL, "PREPARING GAME...", 0, logFile);

    printf("INFO: Loading font >> font.ttf\n");
    fprintf(logFile, "INFO: Loading font >> font.ttf\n");
    showLoadingScreen(renderer, NULL, "LOADING FONT...", 20, logFile);
    TTF_Font* font = TTF_OpenFont("fonts/font.ttf", 20);
    if (font == NULL) {
        showLoadingScreen(renderer, NULL, "EXITING", 100, logFile);
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        printf("ERR: Missing >> font.ttf %s\n", TTF_GetError());
        fprintf(logFile, "ERR: Missing >> font.ttf %s\n", TTF_GetError());
        show_mini_window("Error", "Failed to open a font. Game crashed with code 1");
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    printf("INFO: Font loaded\n");
    fprintf(logFile, "INFO: Font loaded\n");

    Block blocks[MAX_BLOCKS];
    for (int i = 0; i < MAX_BLOCKS; i++) {
        blocks[i].active = 0;
        blocks[i].type = 0;
    }

    SDL_Color textcolor = {255, 255, 255, 255};

    printf("INFO: Loading texture >> player.png\n");
    fprintf(logFile, "INFO: Loading texture >> player.png\n");
    showLoadingScreen(renderer, font, "LOADING PLAYER TEXTURE...", 50, logFile);
    SDL_Surface* playerSurface = IMG_Load("textures/player.png");
    if (!playerSurface) {
        printf("ERR: Missing >> player.png: %s\n", IMG_GetError());
        fprintf(logFile, "ERR: Missing >> player.png: %s\n", IMG_GetError());

    } else {
        printf("INFO: Texture loaded >> %dx%d\n", playerSurface->w, playerSurface->h);
        fprintf(logFile, "INFO: Texture loaded >> %dx%d\n", playerSurface->w, playerSurface->h);
    }

    printf("INFO: Loading textures >> blocksTexture.png\n");
    fprintf(logFile, "INFO: Loading textures >> blocksTexture.png\n");
    showLoadingScreen(renderer, font, "LOADING BLOCKS TEXTURES...", 80, logFile);

    SDL_Surface* blocksSurface = IMG_Load("textures/blocksTexture.png");

    if (!blocksSurface) {
        printf("ERR: Missing >> blocksTexture.png: %s\n", IMG_GetError());
        fprintf(logFile, "ERR: Missing >> blocksTexture.png: %s\n", IMG_GetError());
    } else {
        printf("INFO: Textures loaded >> Blocks: %dx%d (px)\n", blocksSurface->w, blocksSurface->h);
        fprintf(logFile, "INFO: Textures loaded >> Blocks: %dx%d (px)\n", blocksSurface->w, blocksSurface->h);
    }

    if (!playerSurface || !blocksSurface) {
        showLoadingScreen(renderer, font, "EXITING", 100, logFile);
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        show_mini_window("Error", "Failed to load images. Game crashed with code 1");
        printf("ERR: Failed to load >> images");
        fprintf(logFile, "ERR: Failed to load >> images");
        fclose(logFile);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    SDL_Texture* playerTexture = SDL_CreateTextureFromSurface(renderer, playerSurface);
    SDL_Texture* blocksTexture = SDL_CreateTextureFromSurface(renderer, blocksSurface);

    SDL_FreeSurface(playerSurface);
    SDL_FreeSurface(blocksSurface);

    if (!playerTexture || !blocksTexture) {
        showLoadingScreen(renderer, font, "EXITING", 100, logFile);
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        show_mini_window("Error", "Failed to create textures. Game crashed with code 1");
        printf("ERR: Failed to create >> textures");
        fprintf(logFile, "ERR: Failed to create >> textures");
        fclose(logFile);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    showLoadingScreen(renderer, font, "LOADING COMPLETE!", 100, logFile);
    printf("INFO: All assets are loaded\n");
    fprintf(logFile, "INFO: All assets are loaded\n");
    SDL_Delay(500);

    char buffer1[128];
    char buffer2[128];
    char buffer3[128];

    SDL_Rect infoTextRect = {0, 0, 700, 50};
    SDL_Rect livesTextRect = {0, 25, 250, 50};
    SDL_Rect typeTextRect = {0, 50, 250, 50};
    SDL_Rect woodRect = {0, 0, 20, 20};
    SDL_Rect bricksRect = {20, 0, 20, 20};
    SDL_Rect stoneRect = {40, 0, 20, 20};
    SDL_Rect glassRect = {60, 0, 20, 20};
    SDL_Rect logRect = {80, 0, 20, 20};
    SDL_Rect grassRect = {100, 0, 20, 20};

    Player player = {
        .rect = {window_width/2 - 30, window_height/2 - 50, 45, 100},
        .speed = 5,
        .lives = 3
    };

    printf("INFO: Launched >> AdventureCraft, Starting >> Main game loop\n");
    fprintf(logFile, "INFO: Launched >> AdventureCraft, Starting >> Main game loop\n");

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0;
                    break;
            }
        }

        Uint32 buttons = SDL_GetMouseState(&mouseX, &mouseY);
        const Uint8* key_status = SDL_GetKeyboardState(NULL);

        if (player.lives <= 0) {
            show_mini_window("Info", "You died.");
            running = SDL_FALSE;
        }

        if (key_status[SDL_SCANCODE_ESCAPE]) {
            printf("INFO: Escaping >> AdventureCraft\n");
            fprintf(logFile, "INFO: Escaping >> AdventureCraft\n");
            running = SDL_FALSE;
        } else if (key_status[SDL_SCANCODE_1]) {
            if (currentBlockType != 0) {
                currentBlockType = 0;
                printf("INFO: Selected block type >> Wood (%d)\n", currentBlockType);
                fprintf(logFile, "INFO: Selected block type >> Wood (%d)\n", currentBlockType);
                SDL_Delay(100);
            }
        } else if (key_status[SDL_SCANCODE_2]) {
            if (currentBlockType != 1) {
                currentBlockType = 1;
                printf("INFO: Selected block type >> Bricks (%d)\n", currentBlockType);
                fprintf(logFile, "INFO: Selected block type >> Bricks (%d)\n", currentBlockType);
                SDL_Delay(100);
            }
        } else if (key_status[SDL_SCANCODE_3]) {
            if (currentBlockType != 2) {
                currentBlockType = 2;
                printf("INFO: Selected block type >> Stone (%d)\n", currentBlockType);
                fprintf(logFile, "INFO: Selected block type >> Stone (%d)\n", currentBlockType);
                SDL_Delay(100);
            }
        } else if (key_status[SDL_SCANCODE_4]) {
            if (currentBlockType != 3) {
                currentBlockType = 3;
                printf("INFO: Selected block type >> Glass (%d)\n", currentBlockType);
                fprintf(logFile, "INFO: Selected block type >> Glass (%d)\n", currentBlockType);
                SDL_Delay(100);
            }
        } else if (key_status[SDL_SCANCODE_5]) {
            if (currentBlockType != 4) {
                currentBlockType = 4;
                printf("INFO: Selected block type >> Log (%d)\n", currentBlockType);
                fprintf(logFile, "INFO: Selected block type >> Log (%d)\n", currentBlockType);
            }
        } else if (key_status[SDL_SCANCODE_6]) {
            if (currentBlockType != 5) {
                currentBlockType = 5;
                printf("INFO: Selected block type >> Grass (%d)\n", currentBlockType);
                fprintf(logFile, "INFO: Selected block type >> Grass (%d)\n", currentBlockType);
                SDL_Delay(100);
            }
        }

        int old_x = player.rect.x;
        int old_y = player.rect.y;

        int new_x = player.rect.x;
        if (key_status[SDL_SCANCODE_A] && player.rect.x > 0) {
            new_x -= player.speed;
            printf("INFO: Player moved >> |pos: x=%d, y=%d|\n", new_x, player.rect.y);
            fprintf(logFile, "INFO: Player moved >> |pos: x=%d, y=%d|\n", new_x, player.rect.y);
        }
        if (key_status[SDL_SCANCODE_D] && player.rect.x < window_width - player.rect.w) {
            new_x += player.speed;
            printf("INFO: Player moved >> |pos: x=%d, y=%d|\n", new_x, player.rect.y);
            fprintf(logFile, "INFO: Player moved >> |pos: x=%d, y=%d|\n", new_x, player.rect.y);
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
            printf("INFO: Player moved >> |pos: x=%d, y=%d|\n", player.rect.x, new_y);
            fprintf(logFile, "INFO: Player moved >> |pos: x=%d, y=%d|\n", player.rect.x, new_y);
        }
        if (key_status[SDL_SCANCODE_S] && player.rect.y < window_height - player.rect.h) {
            new_y += player.speed;
            printf("INFO: Player moved >> |pos: x=%d, y=%d|\n", player.rect.x, new_y);
            fprintf(logFile, "INFO: Player moved >> |pos: x=%d, y=%d|\n", player.rect.x, new_y);
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

        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 0);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_GetMouseState(&mouseXI, &mouseYI);
        mouseX = (mouseXI / 100) * 100;
        mouseY = (mouseYI / 100) * 100;

        Block_cursor  blockCursor = {
            .rect = {mouseX, mouseY, 100, 100},
            .active = 1
        };

        if (blockCursor.rect.x >= player.rect.x + 300 || blockCursor.rect.y >= player.rect.y + 400 || blockCursor.rect.x <= player.rect.x - 400 || blockCursor.rect.y <= player.rect.y - 400) {
            blockCursor.active = 0;
        }

        if (blockCursor.active) {
            SDL_RenderDrawRect(renderer, &blockCursor.rect);
        }

        if (key_status[SDL_SCANCODE_F5]) {
            if (saveWorld("world.acsave", blocks, blocks_count, &player, logFile)) {
                printf("INFO: Game saved successfully\n");
                fprintf(logFile, "INFO: Game saved successfully\n");
            }
            SDL_Delay(200);
        }

        if (key_status[SDL_SCANCODE_F9]) {
            if (loadWorld("world.acsave", blocks, &blocks_count, &player, logFile)) {
                printf("INFO: Game loaded successfully\n");
                fprintf(logFile, "INFO: Game loaded successfully\n");
            }
            SDL_Delay(200);
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
                blocks[blocks_count].type = currentBlockType;
                blocks_count++;
                printf("INFO: Block added >> |pos: x=%d, y=%d|type: %d|id: %d|\n", blockCursor.rect.x, blockCursor.rect.y, currentBlockType, blocks_count);
                fprintf(logFile, "INFO: Block added >> |pos: x=%d, y=%d|type: %d|id: %d|\n", blockCursor.rect.x, blockCursor.rect.y, currentBlockType, blocks_count);
            }
        }

        if (buttons & SDL_BUTTON_LMASK){
            for (int i = 0; i < blocks_count; i++) {
                if (blocks[i].active && blockCursor.active &&
                    mouseX >= blocks[i].rect.x && mouseX < blocks[i].rect.x + blocks[i].rect.w &&
                    mouseY >= blocks[i].rect.y && mouseY < blocks[i].rect.y + blocks[i].rect.h) {
                    printf("INFO: Block removed >> |pos: x=%d, y=%d|type: %d|id: %d|\n", blocks[i].rect.x, blocks[i].rect.y, blocks[i].type, i);
                    fprintf(logFile, "INFO: Block removed >> |pos: x=%d, y=%d|type: %d|id: %d|\n", blocks[i].rect.x, blocks[i].rect.y, blocks[i].type, i);
                    blocks[i] = blocks[blocks_count - 1];
                    blocks_count--;
                    break;
                }
            }
        }

        for (int i = 0; i < blocks_count; i++) {
            if (blocks[i].active) {
                if (blocks[i].type == 0) {
                    SDL_RenderCopy(renderer, blocksTexture, &woodRect, &blocks[i].rect);
                } else if (blocks[i].type == 1) {
                    SDL_RenderCopy(renderer, blocksTexture, &bricksRect, &blocks[i].rect);
                } else if (blocks[i].type == 2) {
                    SDL_RenderCopy(renderer, blocksTexture, &stoneRect, &blocks[i].rect);
                } else if (blocks[i].type == 3) {
                    SDL_RenderCopy(renderer, blocksTexture, &glassRect, &blocks[i].rect);
                } else if (blocks[i].type == 4) {
                    SDL_RenderCopy(renderer, blocksTexture, &logRect, &blocks[i].rect);
                } else if (blocks[i].type == 5) {
                    SDL_RenderCopy(renderer, blocksTexture, &grassRect, &blocks[i].rect);
                }
            }
        }

        SDL_RenderCopy(renderer, playerTexture, NULL, &player.rect);

        sprintf(buffer1, "AdventureCraft %s", VERSION);

        int textWidth1, textHeight1;
        TTF_SizeText(font, buffer1, &textWidth1, &textHeight1);

        SDL_Surface* fontSurface1 = TTF_RenderText_Solid(font, buffer1, textcolor);
        SDL_Texture* fontTexture1 = SDL_CreateTextureFromSurface(renderer, fontSurface1);

        sprintf(buffer2, "Lives: %d", player.lives);

        int textWidth2, textHeight2;
        TTF_SizeText(font, buffer2, &textWidth2, &textHeight2);

        SDL_Surface* fontSurface2 = TTF_RenderText_Solid(font, buffer2, textcolor);
        SDL_Texture* fontTexture2 = SDL_CreateTextureFromSurface(renderer, fontSurface2);

        char* currentBlockTypeString = "Wood";

        if (currentBlockType == 0) {
            currentBlockTypeString = "Wood";
        } else if (currentBlockType == 1) {
            currentBlockTypeString = "Bricks";
        } else if (currentBlockType == 2) {
            currentBlockTypeString = "Stone";
        } else if (currentBlockType == 3) {
            currentBlockTypeString = "Glass";
        } else if (currentBlockType == 4) {
            currentBlockTypeString = "Log";
        } else if (currentBlockType == 5) {
            currentBlockTypeString = "Grass";
        }

        sprintf(buffer3, "Type: %d (%s)", currentBlockType, currentBlockTypeString);

        int textWidth3, textHeight3;
        TTF_SizeText(font, buffer3, &textWidth3, &textHeight3);

        SDL_Surface* fontSurface3 = TTF_RenderText_Solid(font, buffer3, textcolor);
        SDL_Texture* fontTexture3 = SDL_CreateTextureFromSurface(renderer, fontSurface3);

        infoTextRect.w = textWidth1;
        infoTextRect.h = textHeight1;
        livesTextRect.w = textWidth2;
        livesTextRect.h = textHeight2;
        typeTextRect.w = textWidth3;
        typeTextRect.h = textHeight3;

        SDL_RenderCopy(renderer, fontTexture1, NULL, &infoTextRect);
        SDL_RenderCopy(renderer, fontTexture2, NULL, &livesTextRect);
        SDL_RenderCopy(renderer, fontTexture3, NULL, &typeTextRect);

        SDL_FreeSurface(fontSurface1);
        SDL_FreeSurface(fontSurface2);
        SDL_FreeSurface(fontSurface3);
        SDL_DestroyTexture(fontTexture1);
        SDL_DestroyTexture(fontTexture2);
        SDL_DestroyTexture(fontTexture3);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    printf("INFO: Destroying >> render\n");
    fprintf(logFile, "INFO: Destroying >> render\n");
    SDL_DestroyWindow(window);
    printf("INFO: Destroying >> window\n");
    fprintf(logFile, "INFO: Destroying >> window\n");
    SDL_DestroyTexture(playerTexture);
    printf("INFO: Destroying Texture >> playerTexture\n");
    fprintf(logFile, "INFO: Destroying Texture >> playerTexture\n");
    SDL_DestroyTexture(blocksTexture);
    printf("INFO: Destroying Textures >> blockTextures: Blocks\n");
    fprintf(logFile, "INFO: Destroying Textures >> blockTextures: Blocks\n");
    TTF_CloseFont(font);
    printf("INFO: Closing Font >> font.ttf\n");
    fprintf(logFile, "INFO: Closing Font >> font.ttf\n");
    SDL_Quit();
    printf("INFO: Quiting >> SDL2\n");
    fprintf(logFile, "INFO: Quiting >> SDL2\n");
    IMG_Quit();
    printf("INFO: Quiting >> SDL2_IMG\n");
    fprintf(logFile, "INFO: Quiting >> SDL2_IMG\n");
    TTF_Quit();
    printf("INFO: Quiting >> SDL2_ttf\n");
    fprintf(logFile, "INFO: Quiting >> SDL2_ttf\n");
    SDL_Quit();
    printf("INFO: Quiting >> SDL2\n");
    fprintf(logFile, "INFO: Quiting >> SDL2\n");
    printf("END: Game ended >> code 0 = SUCCESS\n");
    fprintf(logFile, "END: Game ended >> code 0 = SUCCESS\n");
    fclose(logFile);
    return 0;
}