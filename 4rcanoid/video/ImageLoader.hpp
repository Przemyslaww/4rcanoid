#pragma once

class ImageLoader {
	std::vector<SDL_Surface*> loadedSurfaces;
	SDL_Window* sdlWindowHandle;
	public:

		ImageLoader(SDL_Window* m_sdlWindowHandle) : sdlWindowHandle(m_sdlWindowHandle) {}

		SDL_Surface* loadSurface(const std::string& filePath) {
			SDL_Surface* loadedSurface = SDL_LoadBMP(filePath.c_str());
			SDL_Surface* screenSurface = SDL_GetWindowSurface(sdlWindowHandle);
			loadedSurfaces.push_back(loadedSurface);
			return loadedSurface;
		}



		void freeSurfaces() const {
			for (auto& surface : loadedSurfaces) {
				SDL_FreeSurface(surface);
			}
		}
};