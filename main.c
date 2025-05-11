/* Test game in C
 * Created by (c) Adava(Adam Cír) 2025*/

#include <SDL2/SDL.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

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
        show_mini_window("Init", "Init error!");
        return 1;
    }

    SDL_Rect rect = { 100, 100, 200, 100 };
    SDL_Rect rect2 = { 100, 200, 200, 100 };

    window = SDL_CreateWindow(
        "Game",                  // název okna
        SDL_WINDOWPOS_CENTERED,          // pozice X
        SDL_WINDOWPOS_CENTERED,          // pozice Y
        WINDOW_WIDTH,                    // šířka
        WINDOW_HEIGHT,                   // výška
        SDL_WINDOW_SHOWN                 // flags
    );

    if (!window) {
        show_mini_window("Window", "Window error!");
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        show_mini_window("Renderer", "Renderer error!");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    while (running) {
        SDL_Event event;
        // Zpracování událostí
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = SDL_FALSE;
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 171, 0, 1);
        SDL_RenderDrawRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 255, 124, 0, 1);
        SDL_RenderDrawRect(renderer, &rect2);
        SDL_RenderPresent(renderer);
    }


    show_mini_window("Info", "Ending");

    // Úklid a ukončení
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
