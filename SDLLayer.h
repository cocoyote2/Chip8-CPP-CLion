//
// Created by Dimitri on 05/09/2025.
//

#ifndef CHIP8_CPP_CLION_SDLLAYER_H
#define CHIP8_CPP_CLION_SDLLAYER_H


#include <cstdlib>
#include <SDL3/SDL.h>
#include "Chip8.h"

class SDLLayer
{
	public:
		SDLLayer(char const* window_title, int width, int height, int texture_width, int texture_height)
		{
			if (!SDL_Init(SDL_INIT_VIDEO)) {
				SDL_Log("SDL_Init Error: %s", SDL_GetError());
				std::exit(EXIT_FAILURE);
			}

			window = SDL_CreateWindow(window_title, width, height, SDL_WINDOW_RESIZABLE);
			if (!window) {
				SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
				std::exit(EXIT_FAILURE);
			}

			renderer = SDL_CreateRenderer(window, nullptr);
			if (!renderer) {
				SDL_Log("SDL_CreateRenderer Error: %s", SDL_GetError());
				std::exit(EXIT_FAILURE);
			}

			texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, texture_width, texture_height);
			SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
			if (!texture) {
				SDL_Log("SDL_CreateTexture Error: %s", SDL_GetError());
				std::exit(EXIT_FAILURE);
			}
		}

		~SDLLayer()
		{
			SDL_DestroyTexture(texture);
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			SDL_Quit();
		}

		void Update(unsigned char* display, int pitch)
		{
			std::vector<uint32_t> pixels(Chip8::DISPLAY_WIDTH * Chip8::DISPLAY_HEIGHT, 0);

			for (int i = 0; i < Chip8::DISPLAY_WIDTH * Chip8::DISPLAY_HEIGHT; ++i) {
				pixels[i] = display[i] ? 0xFFFFFFFF : 0xFF000000;
			}

			SDL_UpdateTexture(texture, nullptr, pixels.data(), pitch);
			SDL_RenderClear(renderer);
			SDL_RenderTexture(renderer, texture, nullptr, nullptr);
			SDL_RenderPresent(renderer);
		}

		bool ProcessInput(unsigned char* keys)
		{
			bool quit = false;

			SDL_Event event;

			while (SDL_PollEvent(&event))
			{
				switch (event.type) {
					case SDL_EVENT_KEY_DOWN:
					{
						if (event.key.key == SDLK_ESCAPE) quit = true;
						int keyIndex = MapKey(event.key.key);
						if (keyIndex != -1) keys[keyIndex] = 1;
					} break;

					case SDL_EVENT_KEY_UP:
					{
						int keyIndex = MapKey(event.key.key);
						if (keyIndex != -1) keys[keyIndex] = 0;
					} break;
				}
			}

			return quit;
		}
	private:
		SDL_Window* window{};
		SDL_Renderer* renderer{};
		SDL_Texture* texture{};

		int MapKey(SDL_Keycode key)
		{
			switch (key)
			{
				case SDLK_X: return 0;
				case SDLK_1: return 1;
				case SDLK_2: return 2;
				case SDLK_3: return 3;
				case SDLK_Q: return 4;
				case SDLK_W: return 5;
				case SDLK_E: return 6;
				case SDLK_A: return 7;
				case SDLK_S: return 8;
				case SDLK_D: return 9;
				case SDLK_Z: return 0xA;
				case SDLK_C: return 0xB;
				case SDLK_4: return 0xC;
				case SDLK_R: return 0xD;
				case SDLK_F: return 0xE;
				case SDLK_V: return 0xF;
				default: return -1;
			}
		}
};




#endif //CHIP8_CPP_CLION_SDLLAYER_H