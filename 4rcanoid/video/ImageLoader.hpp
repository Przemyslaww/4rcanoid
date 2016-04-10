#pragma once

class ImageLoader {
	std::vector<SDL_Texture*> loadedSurfaces;
	SDL_Window* sdlWindowHandle;
	public:

		ImageLoader(SDL_Window* m_sdlWindowHandle) : sdlWindowHandle(m_sdlWindowHandle) {}

		SDL_Texture* loadSurface(const std::string& filePath, Renderer& renderer) {
			SDL_Surface* loadedSurface = SDL_LoadBMP(filePath.c_str());
			SDL_Texture* loadedTexture = SDL_CreateTextureFromSurface(renderer.getSDLRenderer(), loadedSurface);
			SDL_FreeSurface(loadedSurface);
			loadedSurfaces.push_back(loadedTexture);
			return loadedTexture;
		}



		void freeSurfaces() const {
			for (auto& surface : loadedSurfaces) {
				SDL_DestroyTexture(surface);
			}
		}
};