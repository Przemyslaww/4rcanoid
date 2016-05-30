#pragma once

class SDLSetup {
	public:
		static SDL_Window* initWindow(const std::string& windowCaption, unsigned screenWidth, unsigned screenHeight) {
			if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
				return NULL;
			}
			else {
				TTF_Init();
				SDL_Window* window = SDL_CreateWindow(windowCaption.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
				if (window == NULL) return NULL;
				//SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
				return window;
			}
			return NULL;
		}

		static void exit(SDL_Window* window, const ImageLoader& imageLoader, Renderer& renderer) {
			imageLoader.freeSurfaces();
			renderer.destroySDLRenderer();
			SDL_DestroyWindow(window);
			//SDL_Quit();
		}
};