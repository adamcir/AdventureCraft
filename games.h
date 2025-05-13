//
// Created by adam on 13.5.25.
//

#ifndef GAMES_H
#define GAMES_H

#include <SDL2/SDL.h>

#define BUTTON_OK 0
#define BUTTON_CANCEL 1


int show_mini_window_OC(const char* title, const char* text) {
    const SDL_MessageBoxButtonData buttons[] = {
        { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, BUTTON_OK, "OK" },
        {SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, BUTTON_CANCEL, "Cancel"}
    };

    const SDL_MessageBoxColorScheme colorScheme = {
        {
            { 255, 255, 255},
            { 0, 0, 0},
            { 70, 70, 70 },
            { 200, 200, 200 }
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
            { 255, 255, 255},
            { 0, 0, 0},
            { 70, 70, 70 },
            { 200, 200, 200 }
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



#endif //GAMES_H
