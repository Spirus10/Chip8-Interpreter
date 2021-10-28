#include <iostream>
#include <chrono>
#include <thread>
#include <SDL.h>

#include "cpu.hh"

bool ProcessInput(uint8_t* keys);

int main(int argc, char* argv[])
{
    const int SCREEN_WIDTH = 64;
    const int SCREEN_HEIGHT = 32;

    SDL_Window* window = NULL;

    SDL_Renderer* renderer = NULL;

    SDL_Texture* texture = NULL;

    Uint32 * pixels = new Uint32[64 * 32];

    Chip8 chip8 = Chip8();

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        std::cout << "Error initializing SDL! SDL Error: " << SDL_GetError() << std::endl;
    else
    {
        window = SDL_CreateWindow("Chip8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH * 10, SCREEN_HEIGHT * 10, SDL_WINDOW_SHOWN);

        if (window == NULL)
            std::cout << "Window could not be opened! SDL Error: " << SDL_GetError() << std::endl;
        else
        {
            // Get window surface
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

            // Update surface
            texture = SDL_CreateTexture(renderer,
                SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

            chip8.init();
            chip8.load_rom("pong.rom");

            bool exit = false;

            int videoPitch = sizeof(pixels[0]) * SCREEN_WIDTH;

            while (!exit)
            {
				exit = ProcessInput(chip8.keypad);

                chip8.cycle();

				if(chip8.drawFlag)
					chip8.draw(pixels);

                SDL_UpdateTexture(texture, nullptr, pixels, videoPitch);
                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, texture, nullptr, nullptr);
                SDL_RenderPresent(renderer);

                std::this_thread::sleep_for(std::chrono::milliseconds(5));


            }

        }
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;

}

bool ProcessInput(uint8_t* keys)
{
	bool quit = false;

	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
		{
			quit = true;
		} break;

		case SDL_KEYDOWN:
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
			{
				quit = true;
			} break;

			case SDLK_x:
			{
				keys[0] = 1;
			} break;

			case SDLK_1:
			{
				keys[1] = 1;
			} break;

			case SDLK_2:
			{
				keys[2] = 1;
			} break;

			case SDLK_3:
			{
				keys[3] = 1;
			} break;

			case SDLK_q:
			{
				keys[4] = 1;
			} break;

			case SDLK_w:
			{
				keys[5] = 1;
			} break;

			case SDLK_e:
			{
				keys[6] = 1;
			} break;

			case SDLK_a:
			{
				keys[7] = 1;
			} break;

			case SDLK_s:
			{
				keys[8] = 1;
			} break;

			case SDLK_d:
			{
				keys[9] = 1;
			} break;

			case SDLK_z:
			{
				keys[0xA] = 1;
			} break;

			case SDLK_c:
			{
				keys[0xB] = 1;
			} break;

			case SDLK_4:
			{
				keys[0xC] = 1;
			} break;

			case SDLK_r:
			{
				keys[0xD] = 1;
			} break;

			case SDLK_f:
			{
				keys[0xE] = 1;
			} break;

			case SDLK_v:
			{
				keys[0xF] = 1;
			} break;
			}
		} break;

		case SDL_KEYUP:
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_x:
			{
				keys[0] = 0;
			} break;

			case SDLK_1:
			{
				keys[1] = 0;
			} break;

			case SDLK_2:
			{
				keys[2] = 0;
			} break;

			case SDLK_3:
			{
				keys[3] = 0;
			} break;

			case SDLK_q:
			{
				keys[4] = 0;
			} break;

			case SDLK_w:
			{
				keys[5] = 0;
			} break;

			case SDLK_e:
			{
				keys[6] = 0;
			} break;

			case SDLK_a:
			{
				keys[7] = 0;
			} break;

			case SDLK_s:
			{
				keys[8] = 0;
			} break;

			case SDLK_d:
			{
				keys[9] = 0;
			} break;

			case SDLK_z:
			{
				keys[0xA] = 0;
			} break;

			case SDLK_c:
			{
				keys[0xB] = 0;
			} break;

			case SDLK_4:
			{
				keys[0xC] = 0;
			} break;

			case SDLK_r:
			{
				keys[0xD] = 0;
			} break;

			case SDLK_f:
			{
				keys[0xE] = 0;
			} break;

			case SDLK_v:
			{
				keys[0xF] = 0;
			} break;
			}
		} break;
		}
	}

	return quit;
}
