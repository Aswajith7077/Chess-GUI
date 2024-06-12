#include <SDL2/SDL.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const int FRAME_WIDTH = 64;  // Width of each frame in the sprite sheet
const int FRAME_HEIGHT = 64; // Height of each frame in the sprite sheet
const int NUM_FRAMES = 4;    // Total number of frames in the sprite sheet

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("SDL Animation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Surface* spriteSheetSurface = SDL_LoadBMP("spritesheet.bmp");
    SDL_Texture* spriteSheetTexture = SDL_CreateTextureFromSurface(renderer, spriteSheetSurface);
    SDL_FreeSurface(spriteSheetSurface);

    SDL_Rect frameRects[NUM_FRAMES];
    for (int i = 0; i < NUM_FRAMES; ++i) {
        frameRects[i].x = i * FRAME_WIDTH;
        frameRects[i].y = 0;
        frameRects[i].w = FRAME_WIDTH;
        frameRects[i].h = FRAME_HEIGHT;
    }

    int frameIndex = 0;
    int animationTimer = 0;
    const int animationDelay = 100; // Delay between frame updates (in milliseconds)

    bool quit = false;
    SDL_Event event;

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        animationTimer += SDL_GetTicks();

        if (animationTimer >= animationDelay) {
            animationTimer = 0;
            frameIndex = (frameIndex + 1) % NUM_FRAMES;
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, spriteSheetTexture, &frameRects[frameIndex], nullptr);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(spriteSheetTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
