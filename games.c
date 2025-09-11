//games.c lib code file for games by Adam Cír

#include <SDL_ttf.h>
#include <SDL2/SDL.h>

#define BUTTON_OK 0
#define BUTTON_CANCEL 1

typedef struct {
    char message[128];
    int timer;
    int active;
} StatusMessage;

int show_mini_window_OC(const char* title, const char* text) {
    const SDL_MessageBoxButtonData buttons[] = {
        { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, BUTTON_OK, "OK" },
        {SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, BUTTON_CANCEL, "Cancel"}
    };

    const SDL_MessageBoxColorScheme colorScheme = {
        {
            { 0, 0, 0},
            { 255, 255, 255},
            { 70, 70, 70 },
            { 100, 100, 100 }
        }
    };

    const SDL_MessageBoxData messageboxdata = {
        SDL_MESSAGEBOX_INFORMATION,
        NULL,
        title,
        text,
        SDL_arraysize(buttons),
        buttons,
        &colorScheme
        };
    int buttonid;

    SDL_ShowMessageBox(&messageboxdata, &buttonid);

    return buttonid;
}

int show_mini_window(const char* title, const char* text) {
    const SDL_MessageBoxButtonData buttons[] = {
        { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, BUTTON_OK, "OK" },
    };

    const SDL_MessageBoxColorScheme colorScheme = {
        {
            { 0, 0, 0},
            { 255, 255, 255},
            { 70, 70, 70 },
            { 100, 100, 100 }
        }
    };

    const SDL_MessageBoxData messageboxdata = {
        SDL_MESSAGEBOX_INFORMATION,
        NULL,
        title,
        text,
        SDL_arraysize(buttons),
        buttons,
        &colorScheme
        };
    int buttonid;

    SDL_ShowMessageBox(&messageboxdata, &buttonid);

    return buttonid;
}

int cursor_in_rect(int mouseX, int mouseY, SDL_Rect* rect) {
    return (mouseX >= rect->x &&
            mouseX < rect->x + rect->w &&
            mouseY >= rect->y &&
            mouseY < rect->y + rect->h);
}

void render_overlay(SDL_Renderer* renderer) {
    SDL_Rect fullScreen;
    SDL_GetRendererOutputSize(renderer, &fullScreen.w, &fullScreen.h);
    fullScreen.x = 0;
    fullScreen.y = 0;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
    SDL_RenderFillRect(renderer, &fullScreen);
}

void render_inGameMenu(SDL_Renderer* renderer, TTF_Font* font, SDL_Texture* buttonTexture, const char *options[], int optionsCount, int* selectedOption) {
    render_overlay(renderer);

    SDL_Rect buttonRect_noClicked = {0, 0, 200, 20};
    SDL_Rect buttonRect_clicked = {0, 20, 200, 20};

    SDL_Color textcolorBlack = {0, 0, 0, 255};
    SDL_Color textcolorWhite = {255, 255, 255, 255};

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    int screenWidth, screenHeight;
    SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

    int buttonWidth = 1000;
    int buttonHeight = 100;
    int spacing = 20;
    int totalHeight = (buttonHeight + spacing) * optionsCount - spacing;
    int startY = (screenHeight - totalHeight) / 2;
    int startX = (screenWidth - buttonWidth) / 2;

    *selectedOption = -1;

    for (int i = 0; i < optionsCount; i++) {
        SDL_Rect buttonPos = {
            startX,
            startY + i * (buttonHeight + spacing),
            buttonWidth,
            buttonHeight
        };

        int isHovered = cursor_in_rect(mouseX, mouseY, &buttonPos);

        if (isHovered) {
            *selectedOption = i;
        }

        int textWidth, textHeight;
        TTF_SizeText(font, options[i], &textWidth, &textHeight);

        SDL_Surface* fontSurface = TTF_RenderText_Solid(font, options[i],
                                                        isHovered ? textcolorWhite : textcolorBlack);
        SDL_Texture* fontTexture = SDL_CreateTextureFromSurface(renderer, fontSurface);

        SDL_Rect textRect = {
            buttonPos.x + (buttonWidth - textWidth) / 2,
            buttonPos.y + (buttonHeight - textHeight) / 2,
            textWidth,
            textHeight
        };

        if (isHovered) {
            SDL_RenderCopy(renderer, buttonTexture, &buttonRect_clicked, &buttonPos);
        } else {
            SDL_RenderCopy(renderer, buttonTexture, &buttonRect_noClicked, &buttonPos);
        }

        SDL_RenderCopy(renderer, fontTexture, NULL, &textRect);

        SDL_FreeSurface(fontSurface);
        SDL_DestroyTexture(fontTexture);
    }
}

void show_status_message(StatusMessage* status, const char* message, int duration) {
    strcpy(status->message, message);
    status->timer = duration;
    status->active = 1;
}

void render_status_bar(SDL_Renderer* renderer, TTF_Font* font, StatusMessage* status, int window_width, int window_height) {
    if (!status->active) return;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 200);

    int statusWidth = 500;
    int statusHeight = 50;
    SDL_Rect statusBg = {
        window_width - statusWidth - 20,
        20,
        statusWidth,
        statusHeight
    };

    SDL_RenderFillRect(renderer, &statusBg);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &statusBg);

    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, status->message, textColor);

    if (textSurface) {
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        if (textTexture) {
            int textWidth, textHeight;
            SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);

            SDL_Rect textRect = {
                statusBg.x + (statusWidth - textWidth) / 2,
                statusBg.y + (statusHeight - textHeight) / 2,
                textWidth,
                textHeight
            };

            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
            SDL_DestroyTexture(textTexture);
        }

        SDL_FreeSurface(textSurface);
    }

    status->timer--;
    if (status->timer <= 0) {
        status->active = 0;
    }
}
