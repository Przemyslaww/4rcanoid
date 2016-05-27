#include "header.hpp"

std::unordered_map<char, NetworkMessageHandler*> messagesHandlers;

void initMessagesHandlers() {
	messagesHandlers.emplace(MESSAGE_PLAYER_ACCEPTED, new MessagePlayerAcceptedHandler());
	messagesHandlers.emplace(MESSAGE_GAME_START  , new MessageGameStartHandler()  );
	messagesHandlers.emplace(MESSAGE_GAME_INTERRUPTED, new MessageGameInterruptedHandler());
	messagesHandlers.emplace(MESSAGE_PLAYER_PRESSED_KEY, new MessagePlayerPressedKeyHandler());
}

void removeMessageHandlers() {
	for (auto& it : messagesHandlers) {
		delete it.second;
	}
}

void exit(SDL_Window* window, ImageLoader& imageLoader, Renderer& renderer) {
	SDLSetup::exit(window, imageLoader, renderer);
	removeMessageHandlers();
}



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
		renderer.drawText("4RCANOID alpha 0.0.26", 10, 10, whiteColorSDL);
		renderer.drawText("LazySnails Studio", 10, SCREEN_HEIGHT - 90, whiteColorSDL);
		renderer.drawText("Sebastian Makowski", 10, SCREEN_HEIGHT - 70, whiteColorSDL);
		renderer.drawText("Przemyslaw Mikulski", 10, SCREEN_HEIGHT - 50, whiteColorSDL);
		renderer.drawText("Michal Krezymon", 10, SCREEN_HEIGHT - 30, whiteColorSDL);
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

void clientProgram(GameContext& gameContext) {
	SDL_Texture* backgroundImage = gameContext.getImageLoader()->loadSurface(g_assetsFolder + "background.bmp", *gameContext.getRenderer());

	bool quit = false;
	Timer timer;

	std::string ipAddress = getInputFromUser("Server IP address", *gameContext.getImageLoader(), gameContext.getWindow(), *gameContext.getRenderer());
	
	Client client(ipAddress, gameContext);
	try {
		client.initialize();
		client.createTCPSockets();
		client.createUDPSockets();
	}
	catch (NetworkException& e) {
		gameContext.setTextInPlayersBox("An application encountered an unexpected error: \n"
			+ e.getMessage() +
			"\n\nFor further help please contact us:\n przmiku1@gmail.com\n");
	}

	/* */
	std::thread connectThread([&]() {
		client.makeConnectionToServer();
	});
	/* */
	while (!quit) {

		timer.start();
		SDL_PollEvent(gameContext.getCurrentSDLEventPointer()); {
			if (gameContext.getGameState() == GAME_PLAY) {
				switch (gameContext.getCurrentSDLEvent().type) {
				case SDL_KEYDOWN:
					switch (gameContext.getCurrentSDLEvent().key.keysym.sym) {
					case SDLK_UP:
						if (gameContext.getPlayerNumber() == PLAYER_RIGHT_NUMBER)
							gameContext.getRightPaddle()->moveBy(0, -paddleSpeed);
						else if (gameContext.getPlayerNumber() == PLAYER_LEFT_NUMBER)
							gameContext.getLeftPaddle()->moveBy(0, -paddleSpeed);

						break;
					case SDLK_DOWN:
						if (gameContext.getPlayerNumber() == PLAYER_RIGHT_NUMBER)
							gameContext.getRightPaddle()->moveBy(0, paddleSpeed);
						else if (gameContext.getPlayerNumber() == PLAYER_LEFT_NUMBER)
							gameContext.getLeftPaddle()->moveBy(0, paddleSpeed);

						break;
					case SDLK_RIGHT:
						if (gameContext.getPlayerNumber() == PLAYER_DOWN_NUMBER)
							gameContext.getDownPaddle()->moveBy(paddleSpeed, 0);
						else if (gameContext.getPlayerNumber() == PLAYER_UP_NUMBER)
							gameContext.getUpPaddle()->moveBy(paddleSpeed, 0);

						break;
					case SDLK_LEFT:
						if (gameContext.getPlayerNumber() == PLAYER_DOWN_NUMBER)
							gameContext.getDownPaddle()->moveBy(-paddleSpeed, 0);
						else if (gameContext.getPlayerNumber() == PLAYER_UP_NUMBER)
							gameContext.getUpPaddle()->moveBy(-paddleSpeed, 0);

						break;
					case SDLK_RETURN:

						break;
					case SDLK_ESCAPE:
						quit = true;
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
			else if (gameContext.getGameState() == GAME_LOBBY) {
				switch (gameContext.getCurrentSDLEvent().type) {
				case SDL_KEYDOWN:
					switch (gameContext.getCurrentSDLEvent().key.keysym.sym) {
					case SDLK_RETURN:

						break;
					case SDLK_ESCAPE:
						quit = true;
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

		}

		SDL_RenderClear(gameContext.getRenderer()->getSDLRenderer());
		gameContext.getRenderer()->drawSurface(backgroundImage, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		if (gameContext.getGameState() == GAME_LOBBY) {
			//drawing when in lobby
			gameContext.getPlayersListTextBox()->redraw();
		}
		else if (gameContext.getGameState() == GAME_PLAY) {
			//drawing when in actual gameplay
			gameContext.getLeftPaddle()->draw();
			gameContext.getRightPaddle()->draw();
			gameContext.getDownPaddle()->draw();
			gameContext.getUpPaddle()->draw();
			gameContext.getBlocksGrid()->draw();
			for (int i = 0; i < gameContext.getBalls()->size(); i++) {
				gameContext.getBalls()->at(i).draw();
				gameContext.getBalls()->at(i).detectCollisions(*gameContext.getLeftPaddle());
				gameContext.getBalls()->at(i).detectCollisions(*gameContext.getRightPaddle());
				gameContext.getBalls()->at(i).detectCollisions(*gameContext.getUpPaddle());
				gameContext.getBalls()->at(i).detectCollisions(*gameContext.getDownPaddle());
				gameContext.getBalls()->at(i).detectCollisions(*gameContext.getBlocksGrid());
				gameContext.getBalls()->at(i).update();
			}

		}

		SDL_RenderPresent(gameContext.getRenderer()->getSDLRenderer());
		timer.limitFramerate(SCREEN_FPS, SCREEN_TICKS_PER_FRAME);
	}

	client.stop();
	connectThread.join();
}



void serverProgram(GameContext& gameContext) {
	SDL_Texture* backgroundImage = gameContext.getImageLoader()->loadSurface(g_assetsFolder + "background.bmp", *gameContext.getRenderer());
	bool quit = false;
	Timer timer;
	Server server(gameContext);
	try {
		server.initialize();
		server.createServerSockets();
		server.bindSockets();
		server.listenOnSockets();
	}
	catch (NetworkException& e) {
		gameContext.setTextInPlayersBox("An application encountered an unexpected error: \n"
			+ e.getMessage() +
			"\n\nFor further help please contact us:\n przmiku1@gmail.com\n");
	}

	std::thread acceptThread([&]() {
		server.run();
	});

	std::thread listenUDPThread([&]() {
		server.listenUDP();
	});

	while (!quit) {

		timer.start();
		SDL_PollEvent(gameContext.getCurrentSDLEventPointer()); {
			if (gameContext.getGameState() == GAME_PLAY) {
				switch (gameContext.getCurrentSDLEvent().type) {
				case SDL_KEYDOWN:
					switch (gameContext.getCurrentSDLEvent().key.keysym.sym) {
					case SDLK_UP:
						if (gameContext.getPlayerNumber() == PLAYER_RIGHT_NUMBER)
							gameContext.getRightPaddle()->moveBy(0, -paddleSpeed);
						else if (gameContext.getPlayerNumber() == PLAYER_LEFT_NUMBER)
							gameContext.getLeftPaddle()->moveBy(0, -paddleSpeed);

						break;
					case SDLK_DOWN:
						if (gameContext.getPlayerNumber() == PLAYER_RIGHT_NUMBER)
							gameContext.getRightPaddle()->moveBy(0, paddleSpeed);
						else if (gameContext.getPlayerNumber() == PLAYER_LEFT_NUMBER)
							gameContext.getLeftPaddle()->moveBy(0, paddleSpeed);

						break;
					case SDLK_RIGHT:
						if (gameContext.getPlayerNumber() == PLAYER_DOWN_NUMBER)
							gameContext.getDownPaddle()->moveBy(paddleSpeed, 0);
						else if (gameContext.getPlayerNumber() == PLAYER_UP_NUMBER)
							gameContext.getUpPaddle()->moveBy(paddleSpeed, 0);

						break;
					case SDLK_LEFT:
						if (gameContext.getPlayerNumber() == PLAYER_DOWN_NUMBER)
							gameContext.getDownPaddle()->moveBy(-paddleSpeed, 0);
						else if (gameContext.getPlayerNumber() == PLAYER_UP_NUMBER)
							gameContext.getUpPaddle()->moveBy(-paddleSpeed, 0);

						break;
					case SDLK_RETURN:

						break;
					case SDLK_ESCAPE:
						quit = true;
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
			else if (gameContext.getGameState() == GAME_LOBBY) {
				switch (gameContext.getCurrentSDLEvent().type) {
				case SDL_KEYDOWN:
					switch (gameContext.getCurrentSDLEvent().key.keysym.sym) {
					case SDLK_RETURN:

						break;
					case SDLK_ESCAPE:
						quit = true;
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
			
		}

		SDL_RenderClear(gameContext.getRenderer()->getSDLRenderer());
		gameContext.getRenderer()->drawSurface(backgroundImage, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		if (gameContext.getGameState() == GAME_LOBBY) {
			//drawing when in lobby
			gameContext.getPlayersListTextBox()->redraw();
		}
		else if (gameContext.getGameState() == GAME_PLAY) {
			//drawing when in actual gameplay
			gameContext.getLeftPaddle()->draw();
			gameContext.getRightPaddle()->draw();
			gameContext.getDownPaddle()->draw();
			gameContext.getUpPaddle()->draw();
			gameContext.getBlocksGrid()->draw();
			for (int i = 0; i < gameContext.getBalls()->size(); i++) {
				gameContext.getBalls()->at(i).draw();
				gameContext.getBalls()->at(i).detectCollisions(*gameContext.getLeftPaddle());
				gameContext.getBalls()->at(i).detectCollisions(*gameContext.getRightPaddle());
				gameContext.getBalls()->at(i).detectCollisions(*gameContext.getUpPaddle());
				gameContext.getBalls()->at(i).detectCollisions(*gameContext.getDownPaddle());
				gameContext.getBalls()->at(i).detectCollisions(*gameContext.getBlocksGrid());
				gameContext.getBalls()->at(i).update();
			}

		}
			
		SDL_RenderPresent(gameContext.getRenderer()->getSDLRenderer());
		timer.limitFramerate(SCREEN_FPS, SCREEN_TICKS_PER_FRAME);
	}
	server.stop();
	acceptThread.join();
	listenUDPThread.join();
	return;
}

int main(int argc, char* args[])
{
	SDL_Window* window = SDLSetup::initWindow("4RCANOID 0.026a", SCREEN_WIDTH, SCREEN_HEIGHT);
	ImageLoader imageLoader(window);
	Renderer renderer(window);
	SDL_Event event;
	initMessagesHandlers();
	std::string serverIp;
	
	PROGRAM_STATE programState = showMainMenu(imageLoader, window, renderer);
    if (programState == PROGRAM_EXIT) {
		exit(window, imageLoader, renderer);
		return 0;
	}
	Timer timer;
	Paddle paddleDown(renderer, imageLoader, g_greenColor, SCREEN_WIDTH/2, SCREEN_HEIGHT - 10,true, paddleBoundary,true);
	Paddle paddleLeft(renderer, imageLoader, g_blueColor, 10, SCREEN_HEIGHT/2, false, paddleBoundary, true);
	Paddle paddleUp(renderer, imageLoader, g_redColor, SCREEN_WIDTH / 2, 10, true, paddleBoundary, false);
	Paddle paddleRight(renderer, imageLoader, g_yellowColor, SCREEN_WIDTH - 10, SCREEN_HEIGHT / 2, false, paddleBoundary, false);
	std::vector<Ball> balls = {
		Ball(renderer, imageLoader, SCREEN_WIDTH / 2, SCREEN_HEIGHT - 26, ballSpeed, M_PI / 4)
	};

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
	
	GAME_STATE gameState = GAME_LOBBY;

	char playerNumber = 0;

	TextBox playersListTextBox(SCREEN_WIDTH / 2 - 600 / 2, SCREEN_HEIGHT / 2 - 500 / 2, 600, 500, whiteColorSDL, blackColorSDL, renderer, "");
	GameContext gameContext(window, blocksDescription, &programState, &renderer,
		&imageLoader, &playersListTextBox, &gameState, &event, &playerNumber,
		&paddleLeft, &paddleRight, &paddleUp, &paddleDown, &blocksGrid, &balls);

	if (programState == PROGRAM_SERVER) {
		serverProgram(gameContext);
	}
	else if (programState == PROGRAM_CLIENT) {
		clientProgram(gameContext);
	}

	exit(window, imageLoader, renderer);
	return 0;
}