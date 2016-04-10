#pragma once

class Renderer {
	SDL_Window* sdlWindowHandle;
	public:
		Renderer(SDL_Window* m_sdlWindowHandle) : sdlWindowHandle(m_sdlWindowHandle) {}
		void Renderer::drawRectangle() {

		}

		void Renderer::drawSurface(SDL_Surface* surface, int x = 0, int y = 0) {
			drawSurface(surface, x, y, surface->w, surface->h);
		}

		void Renderer::drawSurface(SDL_Surface* surface, int x, int y, unsigned width, unsigned height) {
			SDL_Surface* windowSurface = SDL_GetWindowSurface(sdlWindowHandle);
			SDL_Rect stretchRect;
			stretchRect.x = x;
			stretchRect.y = y;
			stretchRect.w = width;
			stretchRect.h = height;
			SDL_BlitScaled(surface, NULL, windowSurface, &stretchRect);
			SDL_UpdateWindowSurface(sdlWindowHandle);
		}
};
