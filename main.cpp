#include <chrono>
#include <iostream>
#include <string>

#include "Chip8.h"
#include "SDLLayer.h"
using namespace std;

int main(int argc, char *argv[]) {
#ifdef __EMSCRIPTEN__
    int video_scaling = 10;
    int cycle_delay = 16;
    const char* filepath = "roms\\Breakout.ch8";
    FS.mkdir()
#else
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
        std::exit(EXIT_FAILURE);
    }
    int video_scaling = std::stoi(argv[1]);
    int cycle_delay = std::stoi(argv[2]);
    const char* filepath = argv[3];
#endif
    SDLLayer layer("Chip8", Chip8::DISPLAY_WIDTH*video_scaling, Chip8::DISPLAY_HEIGHT*video_scaling, Chip8::DISPLAY_WIDTH, Chip8::DISPLAY_HEIGHT);
    Chip8 chip8;
    chip8.LoadROM(filepath);

    bool done = false;
    int videoPitch = sizeof(uint32_t) * Chip8::DISPLAY_WIDTH;
    auto lastCycleTime = std::chrono::high_resolution_clock::now();

    while (!done)
    {
        done = layer.ProcessInput(chip8.keypad);

        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

        if (dt > cycle_delay)
        {
            lastCycleTime = currentTime;

            chip8.main_loop();

            layer.Update(chip8.display, videoPitch);
        }
    }

    return 0;
}