#pragma once

class GameContext {
	
	
	SDL_Window* window;
	Renderer* renderer;
	ImageLoader* imageLoader;
	TextBox* playersTextBox;
	GAME_STATE* gameState;
	PROGRAM_STATE* programState;
	SDL_Event* event;
	std::string blocksDescription;
	BlocksGrid* blocksGrid;
	char* playerNumber;

	Paddle* paddleLeft;
	Paddle* paddleRight;
	Paddle* paddleUp;
	Paddle* paddleDown;

	public:
	GameContext(SDL_Window* m_window, const std::string& m_blocksDescription,
		PROGRAM_STATE* m_programState, Renderer* m_renderer,
		ImageLoader* m_imageLoader, TextBox* m_playersTextBox,
		GAME_STATE* m_gameState, SDL_Event* m_event, char* m_playerNumber,
		Paddle* m_paddleLeft, Paddle* m_paddleRight, Paddle* m_paddleUp, Paddle* m_paddleDown, BlocksGrid* m_blocksGrid) {
		window = m_window;
		renderer = m_renderer;
		imageLoader = m_imageLoader;
		playersTextBox = m_playersTextBox;
		gameState = m_gameState;
		programState = m_programState;
		blocksDescription = m_blocksDescription;
		event = m_event;
		playerNumber = m_playerNumber;
		paddleLeft = m_paddleLeft;
		paddleRight = m_paddleRight;
		paddleUp = m_paddleUp;
		paddleDown = m_paddleDown;
		blocksGrid = m_blocksGrid;
	}

	GAME_STATE getGameState() const { return *gameState;  }
	PROGRAM_STATE getProgramState() const { return *programState; }
	void setGameState(const GAME_STATE& m_gameState) { *gameState = m_gameState; }
	void movePlayerPaddleBy(const char& playerNumber, const int& x, const int& y) {
		if (playerNumber == PLAYER_DOWN_NUMBER) paddleDown->moveBy(x, y);
		else if (playerNumber == PLAYER_LEFT_NUMBER) paddleLeft->moveBy(x, y);
		else if (playerNumber == PLAYER_RIGHT_NUMBER) paddleRight->moveBy(x, y);
		else if (playerNumber == PLAYER_UP_NUMBER) paddleUp->moveBy(x, y);
	}

	std::string getBlocksDescription() {
		return blocksDescription;
	}

	SDL_Event getCurrentSDLEvent() {
		return *event;
	}

	void setPlayerNumber(const char& m_playerNumber) {
		*playerNumber = m_playerNumber;
	}

	char getPlayerNumber() const {
		return *playerNumber;
	}

	void setBlocksFromDescription(const std::string& m_blocksDescription) {
		blocksDescription = m_blocksDescription;
		blocksGrid->setBlocksDescription(*renderer, *imageLoader, m_blocksDescription);
	}

	void addPlayer(const std::string& playerName) {
		playersTextBox->setText(playersTextBox->getText() + "\n" + playerName);
	}

	void removePlayer(const std::string& playerName) {
		//TODO - remove given player from the players list (in case of connection loss for example)
	}

	void displayMessage(const std::string& message) {

	}

};
