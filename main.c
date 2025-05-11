/* Test game in C
 * Created by (c) Adava(Adam Cír) 2025*/

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

    SDL_Surface* surface = IMG_Load("player.png");
    if (!surface) {
        show_mini_window("Game", "Image loading error!");
        return 1;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        show_mini_window("Game", "Texture creation error!");
        return 1;
    }


    Player player = {
        .rect = {
            .x = 0,
            .y = 0,
            .w = 100,
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

        if (player.rect.x < 0) player.rect.x = 0;
        if (player.rect.y < 0) player.rect.y = 0;
        if (player.rect.x > 800 - player.rect.w) player.rect.x = 800 - player.rect.w;
        if (player.rect.y > 600 - player.rect.h) player.rect.y = 600 - player.rect.h;

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 99, 71, 0.5);
        SDL_RenderCopy(renderer, texture, NULL, &player.rect);

        SDL_RenderPresent(renderer);

        SDL_Delay(16);

    }


    show_mini_window("Info", "Ending");

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
