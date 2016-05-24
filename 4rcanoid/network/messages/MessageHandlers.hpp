#pragma once

class MessagePlayerAcceptedHandler : public NetworkMessageHandler {
	public:
		void execute(const std::string& message, GameContext& gameContext,
			SOCKET m_socket) {
			if (gameContext.getProgramState() == PROGRAM_CLIENT) {
				gameContext.setPlayerNumber(message[1]);
				auto pos = message.find("|");      
				std::string str = message.substr(pos + 1);
				gameContext.setBlocksFromDescription(message);
				gameContext.displayMessage("Waiting for other players...");
			} 
		}
};

class MessageGameStartHandler : public NetworkMessageHandler {
	public:
	void execute(const std::string& message, GameContext& gameContext,
		SOCKET m_socket) {
		gameContext.setGameState(GAME_PLAY);
	}
};

class MessageGameInterruptedHandler : public NetworkMessageHandler {
public:
	void execute(const std::string& message, GameContext& gameContext,
		SOCKET m_socket) {
		gameContext.setGameState(GAME_LOBBY);
	}
};

class MessagePlayerPressedKeyHandler : public NetworkMessageHandler {
	public:
		void execute(const std::string& message, GameContext& gameContext,
			SOCKET m_socket) {
			char playerNumber = message[2];
			char typedKey = message[1];
			if (playerNumber == PLAYER_LEFT_NUMBER || playerNumber == PLAYER_RIGHT_NUMBER) {
				if (typedKey == UP_KEY)
					gameContext.movePlayerPaddleBy(playerNumber, 0, -paddleSpeed);
				else if (typedKey == DOWN_KEY)
					gameContext.movePlayerPaddleBy(playerNumber, 0, paddleSpeed);
			}
			else if (playerNumber == PLAYER_DOWN_NUMBER || playerNumber == PLAYER_UP_NUMBER) {
				if (typedKey == LEFT_KEY)
					gameContext.movePlayerPaddleBy(playerNumber, -paddleSpeed, 0);
				else if (typedKey == RIGHT_KEY)
					gameContext.movePlayerPaddleBy(playerNumber, paddleSpeed, 0);
			}
			
		}
};