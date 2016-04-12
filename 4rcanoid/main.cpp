#include "header.hpp"


int main(int argc, char* args[])
{
	SDL_Window* window = SDLSetup::initWindow("4RCANOID 0.021a", SCREEN_WIDTH, SCREEN_HEIGHT);
	ImageLoader imageLoader(window);
	Renderer renderer(window);
	SDL_Event event;

	Paddle paddle(renderer, imageLoader, g_greenColor, SCREEN_WIDTH/2, SCREEN_HEIGHT - 10, paddleBoundary, true);
	Ball ball(renderer, imageLoader, SCREEN_WIDTH / 2, SCREEN_HEIGHT - 26, ballSpeed, M_PI/4);
	SDL_Texture* backgroundImage = imageLoader.loadSurface(g_assetsFolder + "background.bmp", renderer);
	renderer.addBackgroundImage(backgroundImage);

	std::ifstream file("levels/test.txt");
	std::string blocksDescription;
	
	std::stringstream strStream;
	strStream << file.rdbuf();//read the file
	blocksDescription = strStream.str();

	file.close();

	BlocksGrid blocksGrid(renderer, imageLoader, blocksDescription);
	
	bool quit = false;
	bool wToku = false;
	int zycia = 3;
	Timer timer;


	while (!quit) {
		timer.start();
		SDL_PollEvent(&event); {
			switch (event.type) {
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
					case SDLK_LEFT:
						paddle.moveBy(-paddleSpeed, 0);
						if(wToku == false) ball.moveBy(-paddleSpeed, 0);
						break;
					case SDLK_RIGHT:
						paddle.moveBy(paddleSpeed, 0);
						if (wToku == false) ball.moveBy(paddleSpeed, 0);
						break;
					case SDLK_SPACE:
						if (wToku == false) wToku = true;
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
		if(wToku == true) ball.update();
		if (ball.getY() > SCREEN_HEIGHT) {
			ball.setPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 26);
			paddle.setPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 10);
			wToku = false;
			zycia--;
			if (zycia == 0) quit = true;
		}
		ball.detectCollisions(paddle);
		ball.detectCollisions(blocksGrid);
		renderer.refreshScreen();
	}
	
	SDL_DestroyTexture(backgroundImage);
	SDLSetup::exit(window, imageLoader, renderer);
	return 0;
}