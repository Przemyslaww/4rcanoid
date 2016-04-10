#pragma once

class Renderer {
	SDL_Window* sdlWindowHandle;
	SDL_Renderer* sdlWindowRenderer;
	std::vector<GameObject*> renderableGameObjects;

	public:
		Renderer(SDL_Window* m_sdlWindowHandle) : sdlWindowHandle(m_sdlWindowHandle) {
			sdlWindowRenderer = SDL_CreateRenderer(sdlWindowHandle, -1, SDL_RENDERER_ACCELERATED);
			SDL_SetRenderDrawColor(sdlWindowRenderer, 0, 0, 0, 0);
		}

		void Renderer::drawSurface(SDL_Texture* surface, int x = 0, int y = 0) {
			int w, h;
			SDL_QueryTexture(surface, NULL, NULL, &w, &h);
			drawSurface(surface, x, y, w, h);
		}

		void Renderer::destroySDLRenderer() {
			SDL_DestroyRenderer(sdlWindowRenderer);
		}

		SDL_Renderer* getSDLRenderer() {
			return sdlWindowRenderer;
		}

		void Renderer::refreshScreen() {
			SDL_RenderClear(sdlWindowRenderer);
			drawObjects();
			SDL_RenderPresent(sdlWindowRenderer);
		}

		void Renderer::registerRenderable(GameObject* gameObject) {
			renderableGameObjects.push_back(gameObject);
		}

		void Renderer::drawObjects();

		void Renderer::unregisterRenderable(GameObject* gameObject) {
			auto position = std::find(renderableGameObjects.begin(), renderableGameObjects.end(), gameObject);
			if (position != renderableGameObjects.end())
				renderableGameObjects.erase(position);
		}

		
		void Renderer::drawSurface(SDL_Texture* surface, int x, int y, unsigned width, unsigned height) {
			SDL_Rect stretchRect;
			stretchRect.x = x;
			stretchRect.y = y;
			stretchRect.w = width;
			stretchRect.h = height;
			//SDL_BlitScaled(surface, NULL, windowSurface, &stretchRect);
			//SDL_UpdateWindowSurface(sdlWindowHandle);
			SDL_RenderCopy(sdlWindowRenderer, surface, NULL, &stretchRect);
		}
};
