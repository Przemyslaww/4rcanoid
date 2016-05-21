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

	public:
	GameContext(SDL_Window* m_window, const std::string& m_blocksDescription,
		PROGRAM_STATE* m_programState, Renderer* m_renderer,
		ImageLoader* m_imageLoader, TextBox* m_playersTextBox,
		GAME_STATE* m_gameState, SDL_Event* m_event) {
		window = m_window;
		renderer = m_renderer;
		imageLoader = m_imageLoader;
		playersTextBox = m_playersTextBox;
		gameState = m_gameState;
		programState = m_programState;
		blocksDescription = m_blocksDescription;
		event = m_event;
	}

	GAME_STATE getGameState() const { return *gameState;  }
	PROGRAM_STATE getProgramState() const { return *programState; }
	void setGameState(const GAME_STATE& m_gameState) { *gameState = m_gameState; }

	std::string getBlocksDescription() {
		return blocksDescription;
	}

	SDL_Event getCurrentSDLEvent() {
		return *event;
	}

	void addPlayer(const std::string& playerName) {
		playersTextBox->setText(playersTextBox->getText() + "\n" + playerName);
	}

	void removePlayer(const std::string& playerName) {
		//TODO - remove given player from the players list (in case of connection loss for example)
	}

};
