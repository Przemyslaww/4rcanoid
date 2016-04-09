#include "header.hpp"

int main(int argc, char* args[])
{
	SDL_Window* window = SDLSetup::initWindow("Test", 640, 480);
	SDL_Delay(3000);
	SDLSetup::exit(window);
	return 0;
}