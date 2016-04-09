#pragma once

class Renderer {
	SDL_Window* sdlWindowHandle;
	public:
		Renderer(SDL_Window* m_sdlWindowHandle) : sdlWindowHandle(m_sdlWindowHandle) {}
		void Renderer::drawRectangle() {

		}

		void Renderer::drawSurface(SDL_Surface* surface) {

		}
};
