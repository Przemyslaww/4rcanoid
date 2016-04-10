#include "header.hpp"

int main(int argc, char* args[])
{
	SDL_Window* window = SDLSetup::initWindow("Test", 640, 480);
	
	ImageLoader imageLoader(window);
	Renderer renderer(window);

	SDL_Surface* paddleGreenHorizontalImage = imageLoader.loadSurface("assets/paddleGreenHorizontal.bmp");
	renderer.drawSurface(paddleGreenHorizontalImage, 270, 460, 100, 20);
	renderer.drawSurface(paddleGreenHorizontalImage, 270, 0, 100, 20);

	SDL_Delay(3000);
	SDLSetup::exit(window, imageLoader);
	return 0;
}