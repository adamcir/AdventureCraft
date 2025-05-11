/* Test game in C
 * Created by (c) Adava(Adam Cír) 2025*/

#include <SDL2/SDL.h>

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
        show_mini_window("Init", "Init error!");
        return 1;
    }


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

    Player player = {
        .rect = {
            .x = 400,
            .y = 300,
            .w = 50,
            .h = 50
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
        if (key_status[SDL_SCANCODE_W] || key_status[SDL_SCANCODE_UP]) {
            player.rect.y -= player.speed;
        }
        if (key_status[SDL_SCANCODE_S] || key_status[SDL_SCANCODE_DOWN]) {
            player.rect.y += player.speed;
        }
        if (key_status[SDL_SCANCODE_A] || key_status[SDL_SCANCODE_LEFT]) {
            player.rect.x -= player.speed;
        }
        if (key_status[SDL_SCANCODE_D] || key_status[SDL_SCANCODE_RIGHT]) {
            player.rect.x += player.speed;
        }

        // Omezení pohybu na hranice okna
        if (player.rect.x < 0) player.rect.x = 0;
        if (player.rect.y < 0) player.rect.y = 0;
        if (player.rect.x > 800 - player.rect.w) player.rect.x = 800 - player.rect.w;
        if (player.rect.y > 600 - player.rect.h) player.rect.y = 600 - player.rect.h;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 99, 71, 0.5);
        SDL_RenderFillRect(renderer, &player.rect);

        SDL_RenderPresent(renderer);

        SDL_Delay(16);

    }


    show_mini_window("Info", "Ending");

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
