#pragma once

class Renderer {
	SDL_Window* sdlWindowHandle;
	SDL_Renderer* sdlWindowRenderer;
	std::vector<GameObject*> renderableGameObjects;
	SDL_Texture* backgroundImage;
	TTF_Font* font;
	int fontSize;

	public:
		Renderer(SDL_Window* m_sdlWindowHandle) : sdlWindowHandle(m_sdlWindowHandle) {
			sdlWindowRenderer = SDL_CreateRenderer(sdlWindowHandle, -1, SDL_RENDERER_ACCELERATED);
			fontSize = 11;
			SDL_SetRenderDrawColor(sdlWindowRenderer, 0, 0, 0, 0);
			backgroundImage = NULL;
			font = TTF_OpenFont("assets/font.ttf", fontSize);
			if (!font) {
				printf("TTF_OpenFont: %s\n", TTF_GetError());
				// handle error
			}
		}

		void drawRect(const int& x, const int& y, const int& w, const int& h, const SDL_Color& color) {
			SDL_SetRenderDrawColor(sdlWindowRenderer, color.r, color.g, color.b, color.a);
			SDL_Rect rect;
			rect.x = x;
			rect.y = y;
			rect.w = w;
			rect.h = h;
			SDL_RenderFillRect(sdlWindowRenderer, &rect);
			SDL_SetRenderDrawColor(sdlWindowRenderer, 0, 0, 0, 0);
		}

		void drawText(const std::string& text, const int& x, const int& y, const SDL_Color& color) {
			SDL_SetRenderDrawColor(sdlWindowRenderer, color.r, color.g, color.b, color.a);
			SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, text.c_str(), color); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
			SDL_Texture* Message = SDL_CreateTextureFromSurface(sdlWindowRenderer, surfaceMessage); //now you can convert it into a texture
			SDL_Rect Message_rect; //create a rect
			Message_rect.x = x;  //controls the rect's x coordinate 
			Message_rect.y = y;// controls the rect's y coordinte
			Message_rect.w = text.length() * fontSize; // controls the width of the rect
			Message_rect.h = fontSize + 10; // controls the height of the rect
			SDL_RenderCopy(sdlWindowRenderer, Message, NULL, &Message_rect);
			SDL_FreeSurface(surfaceMessage);
			SDL_DestroyTexture(Message);
			SDL_SetRenderDrawColor(sdlWindowRenderer, 0,0,0,0);
		}

		void addBackgroundImage(SDL_Texture* m_backgroundImage) {
			backgroundImage = m_backgroundImage;
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
			if (backgroundImage)
				drawSurface(backgroundImage, 0, 0, 0, 0);
			drawObjects();
			SDL_RenderPresent(sdlWindowRenderer);
		}

		void Renderer::registerRenderable(GameObject* gameObject) {
			renderableGameObjects.push_back(gameObject);
		}

		void Renderer::drawObjects();

		void Renderer::drawSurface(SDL_Texture* surface, int x , int y , bool isHorizontal) {
			int w, h;
			SDL_QueryTexture(surface, NULL, NULL, &w, &h);
			drawSurface(surface, x, y, w, h);
		}

		void Renderer::unregisterRenderable(GameObject* gameObject) {
			auto position = std::find(renderableGameObjects.begin(), renderableGameObjects.end(), gameObject);
			if (position != renderableGameObjects.end())
				renderableGameObjects.erase(position);
		}
		void Renderer::drawSurfaceVertical(SDL_Texture* surface, int x, int y, unsigned width, unsigned height) {
			SDL_Rect stretchRect;
			stretchRect.x = x;
			stretchRect.y = y;
			stretchRect.w = width;
			stretchRect.h = height;
			if (stretchRect.x == 0 && stretchRect.y == 0 && stretchRect.w == 0 && stretchRect.h == 0)
				SDL_RenderCopyEx(sdlWindowRenderer, surface, NULL, NULL, 90, NULL, SDL_FLIP_NONE);
			else
				SDL_RenderCopyEx(sdlWindowRenderer, surface, NULL, &stretchRect, 90, NULL, SDL_FLIP_NONE);
		}
		
		void Renderer::drawSurface(SDL_Texture* surface, int x, int y, unsigned width, unsigned height) {
			SDL_Rect stretchRect;
			stretchRect.x = x;
			stretchRect.y = y;
			stretchRect.w = width;
			stretchRect.h = height;
			if (stretchRect.x == 0 && stretchRect.y == 0 && stretchRect.w == 0 && stretchRect.h == 0)
				SDL_RenderCopy(sdlWindowRenderer, surface, NULL, NULL);
			else 
				SDL_RenderCopy(sdlWindowRenderer, surface, NULL, &stretchRect);
		}
};
