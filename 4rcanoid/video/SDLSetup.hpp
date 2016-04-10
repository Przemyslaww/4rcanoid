#pragma once

class SDLSetup {
	public:
		static SDL_Window* initWindow(const std::string& windowCaption, unsigned screenWidth, unsigned screenHeight) {
			if (SDL_Init(SDL_INIT_VIDEO) < 0) {
				return NULL;
			}
			else {
				SDL_Window* window = SDL_CreateWindow(windowCaption.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
				if (window == NULL) return NULL;
				return window;
			}
			return NULL;
		}

		static void exit(SDL_Window* window, const ImageLoader& imageLoader) {
			imageLoader.freeSurfaces();
			SDL_DestroyWindow(window);
			SDL_Quit();
		}
};