#include "header.hpp"

int main(int argc, char* args[])
{
	SDL_Window* window = SDLSetup::initWindow("4RCANOID 0.021a", 640, 480);
	ImageLoader imageLoader(window);
	Renderer renderer(window);
	SDL_Event event;

	Paddle paddle(renderer, imageLoader, g_greenColor, 320, 470, 200, true);
	std::vector<Block> blocks;

	
	Ball ball(renderer, imageLoader, 200, 50, 1.0, 0.65);
	Block greenBlock(renderer, imageLoader, g_greenColor, 100, 100);
	
	bool quit = false;
	Timer timer;

	while (!quit) {
		timer.start();
		SDL_PollEvent(&event); {
			switch (event.type) {
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
					case SDLK_LEFT:
						paddle.moveBy(-7, 0);
						break;
					case SDLK_RIGHT:
						paddle.moveBy(7, 0);
						break;
					default:
						break;
					}
										
					break;
				case SDL_KEYUP:
					break;

				case SDL_QUIT:
					quit = true;
					break;

				default:
					break;
			}
		}
		timer.limitFramerate(SCREEN_FPS, SCREEN_TICKS_PER_FRAME);
		ball.rotateMovementDirectionBy(0.01);
		ball.update();
		renderer.refreshScreen();
	}
	
	SDLSetup::exit(window, imageLoader, renderer);
	return 0;
}