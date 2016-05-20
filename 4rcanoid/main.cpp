#include "header.hpp"

PROGRAM_STATE showMainMenu(ImageLoader& imageLoader, SDL_Window* window, Renderer& renderer) {
	SDL_Event event_menu;
	SDL_Texture* backgroundImageServer = imageLoader.loadSurface(g_assetsFolder + "backgroundmenuserver.bmp", renderer);
	SDL_Texture* backgroundImageClient = imageLoader.loadSurface(g_assetsFolder + "backgroundmenuclient.bmp", renderer);

	bool quit1 = false;
	bool serverChosen = false;
	Timer timer;

	
	while (!quit1) {
		timer.start();
		SDL_PollEvent(&event_menu); {
			switch (event_menu.type) {
			case SDL_KEYDOWN:
				switch (event_menu.key.keysym.sym) {
				case SDLK_DOWN:
					serverChosen = true;
					break;
				case SDLK_UP:
					serverChosen = false;
					break;
				case SDLK_RETURN:
					quit1 = true;
					break;
				case SDLK_ESCAPE:
					return PROGRAM_EXIT;
					break;
				default:
					break;
				}

				break;
			case SDL_KEYUP:
				break;

			case SDL_QUIT:
				return PROGRAM_EXIT;
				break;

			default:
				break;
			}
		}

		SDL_RenderClear(renderer.getSDLRenderer());
		if (serverChosen)
		{
			renderer.drawSurface(backgroundImageServer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		}
		else
		{
			renderer.drawSurface(backgroundImageClient, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		}
		SDL_RenderPresent(renderer.getSDLRenderer());

		timer.limitFramerate(SCREEN_FPS, SCREEN_TICKS_PER_FRAME);
    }
	return serverChosen ? PROGRAM_SERVER : PROGRAM_CLIENT;
}

std::string getInputFromUser(const std::string& message, ImageLoader& imageLoader, SDL_Window* window, Renderer& renderer) {
	SDL_Event event_menu;
	SDL_Texture* backgroundImage = imageLoader.loadSurface(g_assetsFolder + "background.bmp", renderer);

	bool quit1 = false;
	bool serverChosen = false;
	Timer timer;

	TextInputDialog serverInfoInputDialog(SCREEN_WIDTH/2 - 300/2, SCREEN_HEIGHT/2 - 100/2, 300, 100, whiteColorSDL, blackColorSDL, message, renderer);

	while (!quit1) {

		timer.start();
		SDL_PollEvent(&event_menu); {
			serverInfoInputDialog.readUserInput(event_menu);
			switch (event_menu.type) {
			case SDL_KEYDOWN:
				switch (event_menu.key.keysym.sym) {
				case SDLK_RETURN:
					quit1 = true;
					break;
				case SDLK_ESCAPE:
					return "";
					break;
				default:
					break;
				}

				break;
			case SDL_KEYUP:
				break;

			case SDL_QUIT:
				return "";
				break;

			default:
				break;
			}
		}

		SDL_RenderClear(renderer.getSDLRenderer());
		renderer.drawSurface(backgroundImage, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		serverInfoInputDialog.redraw();
		SDL_RenderPresent(renderer.getSDLRenderer());
		timer.limitFramerate(SCREEN_FPS, SCREEN_TICKS_PER_FRAME);
	}
	return serverInfoInputDialog.getInput();
}

std::vector<NetworkConnection> listenForPlayers(ImageLoader& imageLoader, SDL_Window* window, Renderer& renderer) {
	SDL_Event event_menu;
	SDL_Texture* backgroundImage = imageLoader.loadSurface(g_assetsFolder + "background.bmp", renderer);

	bool quit1 = false;
	bool serverChosen = false;
	Timer timer;

	TextBox playersListTextBox(SCREEN_WIDTH/2 - 600/2, SCREEN_HEIGHT/2 - 500/2, 600, 500, whiteColorSDL, blackColorSDL, renderer, "Listening on 252.128.10.53, port 50252\nPlayers list:\n\n252.128.10.53 ");

	while (!quit1) {

		timer.start();
		SDL_PollEvent(&event_menu); {
			switch (event_menu.type) {
			case SDL_KEYDOWN:
				switch (event_menu.key.keysym.sym) {
				case SDLK_RETURN:
					return {};
					break;
				case SDLK_ESCAPE:
					return {};
					break;
				default:
					break;
				}

				break;
			case SDL_KEYUP:
				break;

			case SDL_QUIT:
				return {};
				break;

			default:
				break;
			}
		}

		SDL_RenderClear(renderer.getSDLRenderer());
		renderer.drawSurface(backgroundImage, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		playersListTextBox.redraw();
		SDL_RenderPresent(renderer.getSDLRenderer());
		timer.limitFramerate(SCREEN_FPS, SCREEN_TICKS_PER_FRAME);
	}
	return {};
}

int main(int argc, char* args[])
{
	SDL_Window* window = SDLSetup::initWindow("4RCANOID 0.026a", SCREEN_WIDTH, SCREEN_HEIGHT);
	ImageLoader imageLoader(window);
	Renderer renderer(window);
	SDL_Event event;

	std::string serverIp;
	std::string serverPort;

	PROGRAM_STATE programState = showMainMenu(imageLoader, window, renderer);
	if (programState == PROGRAM_CLIENT) {
		serverIp = getInputFromUser("Server IP address", imageLoader, window, renderer);
		if (serverIp == "") {
			SDLSetup::exit(window, imageLoader, renderer);
			return 0;
		}
		serverPort = getInputFromUser("Server port", imageLoader, window, renderer);
		if (serverPort == "") {
			SDLSetup::exit(window, imageLoader, renderer);
			return 0;
		}
	}
	else if (programState == PROGRAM_SERVER) {
		listenForPlayers(imageLoader, window, renderer);
	}
	else {
		SDLSetup::exit(window, imageLoader, renderer);
		return 0;
	}
	Timer timer;
	Paddle paddleDown(renderer, imageLoader, g_greenColor, SCREEN_WIDTH/2, SCREEN_HEIGHT - 10,true, paddleBoundary,true);
	Paddle paddleLeft(renderer, imageLoader, g_blueColor, 10, SCREEN_HEIGHT/2, false, paddleBoundary, true);
	Paddle paddleUp(renderer, imageLoader, g_redColor, SCREEN_WIDTH / 2, 10, true, paddleBoundary, false);
	Paddle paddleRight(renderer, imageLoader, g_yellowColor, SCREEN_WIDTH - 10, SCREEN_HEIGHT / 2, false, paddleBoundary, false);
	Ball ball(renderer, imageLoader, SCREEN_WIDTH / 2, SCREEN_HEIGHT - 26, ballSpeed, M_PI/4);
	SDL_Texture * backgroundImage = imageLoader.loadSurface(g_assetsFolder + "background2.bmp", renderer);
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
	
	

	while (!quit) {
		timer.start();
		SDL_PollEvent(&event); {
			switch (event.type) {
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
					case SDLK_LEFT:
						paddleDown.moveBy(-paddleSpeed, 0);
						paddleUp.moveBy(-paddleSpeed, 0);
						if(wToku == false && paddleDown.getX() > paddleBoundary - 48) ball.moveBy(-paddleSpeed, 0); 
						break;
					case SDLK_RIGHT:
						paddleDown.moveBy(paddleSpeed, 0);
						paddleUp.moveBy(paddleSpeed, 0);
						if (wToku == false && paddleDown.getX() < SCREEN_WIDTH - paddleBoundary + 48) ball.moveBy(paddleSpeed, 0);
						break;
					case SDLK_DOWN:
						paddleLeft.moveBy( 0, paddleSpeed);
						paddleRight.moveBy(0, paddleSpeed);
						break;
					case SDLK_UP:
						paddleLeft.moveBy(0, -paddleSpeed);
						paddleRight.moveBy(0, -paddleSpeed);
						break;
					case SDLK_ESCAPE:
						quit = true;
						break;
					case SDLK_SPACE:
						if (wToku == false) {
							wToku = true;
							ball.moveX = 6*cos(M_PI / 4);
							ball.moveY = 6*sin(M_PI / 4);
						}
						break;
					case SDLK_z:
						if (wToku == false) {

						}
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
			paddleDown.setPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 10);
			wToku = false;
			//zycia--;
			if (zycia == 0) quit = true;
		}
		ball.detectCollisions(paddleDown);
		ball.detectCollisions(paddleRight);
		ball.detectCollisions(paddleLeft);
		ball.detectCollisions(paddleUp);
		ball.detectCollisions(blocksGrid);
		renderer.refreshScreen();
	}
	
	SDL_DestroyTexture(backgroundImage);
	SDLSetup::exit(window, imageLoader, renderer);
	return 0;
}