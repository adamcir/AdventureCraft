//games.h lib for games by Adam Cír

#ifndef GAMES_H
#define GAMES_H

#define BUTTON_OK 0
#define BUTTON_CANCEL 1

typedef struct {
    char message[128];
    int timer;
    int active;
} StatusMessage;

StatusMessage statusMessage = {0};

int show_mini_window_OC(const char* title, const char* text);
int show_mini_window(const char* title, const char* text);
void render_inGameMenu(SDL_Renderer* renderer, TTF_Font* font, SDL_Texture* buttonTexture, const char *options[], int optionsCount, int* selectedOption);
int cursor_in_rect(int mouseX, int mouseY, SDL_Rect* rect);
void render_overlay(SDL_Renderer* renderer);
void show_status_message(StatusMessage* status, const char* message, int duration);
void render_status_bar(SDL_Renderer* renderer, TTF_Font* font, StatusMessage* status, int window_width, int window_height);


#endif
