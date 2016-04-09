#pragma once

class ImageLoader {
	std::vector<SDL_Surface*> loadedSurfaces;
	public:
		SDL_Surface* loadSurface(const std::string& filePath) {
			SDL_Surface* loadedSurface = IMG_Load(filePath.c_str());
		}

		void freeSurfaces() const {
			for (auto& surface : loadedSurfaces) {
				SDL_FreeSurface(surface);
			}
		}
};