#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>
#include <chrono>

#include "chip8.h"

const SDL_Scancode KEYMAP[16] = {
    SDL_SCANCODE_X, // 0
    SDL_SCANCODE_1, // 1
    SDL_SCANCODE_2, // 2
    SDL_SCANCODE_3, // 3
    SDL_SCANCODE_Q, // 4
    SDL_SCANCODE_W, // 5
    SDL_SCANCODE_E, // 6
    SDL_SCANCODE_A, // 7
    SDL_SCANCODE_S, // 8
    SDL_SCANCODE_D, // 9
    SDL_SCANCODE_Z, // A
    SDL_SCANCODE_C, // B
    SDL_SCANCODE_4, // C
    SDL_SCANCODE_R, // D
    SDL_SCANCODE_F, // E
    SDL_SCANCODE_V  // F
};

void handleInput(Chip8 &chip8, bool &quit)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT)
        {
            quit = true;
        }
        else if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP)
        {
            uint8_t state = (event.type == SDL_EVENT_KEY_DOWN) ? 1 : 0;
            for (int i = 0; i < 16; ++i)
            {
                // Updated for SDL3: event.key.scancode replacing event.key.keysym.scancode
                if (event.key.scancode == KEYMAP[i])
                {
                    chip8.keypad[i] = state;
                }
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    {
        std::cerr << "Failed to initialize SDL3: " << SDL_GetError() << '\n';
        return 1;
    }

    constexpr int SCALE = 10; // Scale 64x32 display to 640x320

    SDL_Window *window = SDL_CreateWindow(
        "CHIP-8 Emulator",
        64 * SCALE, 32 * SCALE,
        0);

    if (!window)
    {
        std::cerr << "Failed to create window: " << SDL_GetError() << '\n';
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer)
    {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << '\n';
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Texture *texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        64, 32);

    Chip8 chip8;

    chip8.loadROM("roms/Rush Hour [Hap, 2006].ch8");

    bool quit = false;

    // Timing parameters
    constexpr double CPU_FREQUENCY = 500.0;  // ~500 Hz CPU
    constexpr double TIMER_FREQUENCY = 60.0; // 60 Hz Timers
    constexpr auto CPU_INTERVAL = std::chrono::duration<double>(1.0 / CPU_FREQUENCY);
    constexpr auto TIMER_INTERVAL = std::chrono::duration<double>(1.0 / TIMER_FREQUENCY);

    auto lastCpuTime = std::chrono::high_resolution_clock::now();
    auto lastTimerTime = std::chrono::high_resolution_clock::now();

    uint32_t pixelBuffer[64 * 32];

    while (!quit)
    {
        auto currentTime = std::chrono::high_resolution_clock::now();

        handleInput(chip8, quit);

        // 1. Run CPU Cycles at ~500 Hz
        if (currentTime - lastCpuTime >= CPU_INTERVAL)
        {
            chip8.cycle();
            lastCpuTime = currentTime;
        }

        // 2. Decrement Delay and Sound Timers at 60 Hz
        if (currentTime - lastTimerTime >= TIMER_INTERVAL)
        {
            if (chip8.delayTimer > 0)
            {
                --chip8.delayTimer;
            }
            if (chip8.soundTimer > 0)
            {
                --chip8.soundTimer;
                // Optional: Play a sound/beep here while soundTimer > 0
            }
            lastTimerTime = currentTime;
        }

        // 3. Render Display
        for (int y = 0; y < 32; ++y)
        {
            for (int x = 0; x < 64; ++x)
            {
                pixelBuffer[y * 64 + x] = chip8.display[x][y] ? 0xFFFFFFFF : 0x000000FF;
            }
        }

        SDL_UpdateTexture(texture, nullptr, pixelBuffer, 64 * sizeof(uint32_t));
        SDL_RenderClear(renderer);

        SDL_RenderTexture(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        SDL_Delay(1);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}