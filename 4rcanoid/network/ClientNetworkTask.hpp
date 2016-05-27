#pragma once

class ClientNetworkTask {
	SOCKET connectionSocketTCPin;
	SOCKET connectionSocketUDPin;
	SOCKET connectionSocketTCPout;
	SOCKET connectionSocketUDPout;

	std::thread taskThreadTCPin;
	std::thread taskThreadUDPin;
	std::thread taskThreadTCPout;
	std::thread taskThreadUDPout;
	GameContext& gameContext;

	Client* client;

	char playerNumber;

	static std::string receiveMessage(SOCKET m_connectionSocket) {
		return MessageReceiverSender::receiveTCPMessage(m_connectionSocket);
	}

	static void sendMessage(SOCKET m_connectionSocket, const char& messageType, const std::string& message) {
		MessageReceiverSender::sendTCPMessage(m_connectionSocket,
			MessageReceiverSender::createMessage(messageType, message));
	}

	static std::string getPeerIp(SOCKET m_connectionSocket) {
		return MessageReceiverSender::getPeerIpTCP(m_connectionSocket);
	}

	static std::string getLocalIp(SOCKET m_connectionSocket) {
		return MessageReceiverSender::getLocalIp(m_connectionSocket);
	}

	static void taskJobTCPIn(SOCKET m_connectionSocket, SOCKET m_answerSocket, GameContext& gameContext) {
		gameContext.addPlayer("called");
		std::string receivedMessage;
		while (true) {
			gameContext.addPlayer("trying to receive");
			receivedMessage = receiveMessage(m_connectionSocket);
			gameContext.addPlayer("received");
			if (receivedMessage != "") {
				auto got = messagesHandlers.find(receivedMessage[0]);
				if (got != messagesHandlers.end()) {
					got->second->execute(receivedMessage, gameContext);
				}
				else {
					throw NetworkException("Unkown message: " + receivedMessage);
				}
			}
			else {
				//disconnected from server
				gameContext.setGameState(GAME_LOBBY);
				gameContext.setTextInPlayersBox("Disconnected from server.\nRestart an application and try again.");
				break;
			}
			
		}
	}

	static void taskJobTCPOut(SOCKET m_connectionSocket, SOCKET m_answerSocket, GameContext& gameContext) {
		while (true);
	}

	static void taskJobUDPIn(SOCKET m_connectionSocket, SOCKET m_answerSocket, GameContext& gameContext) {
		std::string receivedMessage;
		while (true) {
			receivedMessage = receiveMessage(m_connectionSocket);
			auto got = messagesHandlers.find(receivedMessage[0]);
			if (got != messagesHandlers.end()) {
				got->second->execute(receivedMessage, gameContext);
			}
			else {
				throw NetworkException("Unknown message: " + receivedMessage);
			}
		}
	}

	static std::string toKeyCodeByte(const long long unsigned& keyCode) {
		if (keyCode == SDLK_LEFT) return "" + LEFT_KEY;
		if (keyCode == SDLK_RIGHT) return "" + RIGHT_KEY;
		if (keyCode == SDLK_UP) return "" + UP_KEY;
		if (keyCode == SDLK_DOWN) return "" + DOWN_KEY;
		if (keyCode == SDLK_ESCAPE) return "" + ESCAPE_KEY;
		if (keyCode == SDLK_SPACE) return "" + SPACE_KEY;
		return "" + 255;
	}

	static void taskJobUDPOut(SOCKET m_connectionSocket, SOCKET m_answerSocket, GameContext& gameContext) {
		while (true) {
			SDL_Event event = gameContext.getCurrentSDLEvent();
			switch (event.type) {
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_LEFT:
					sendMessage(m_connectionSocket, MESSAGE_PLAYER_PRESSED_KEY, toKeyCodeByte(SDLK_LEFT) + gameContext.getPlayerNumber());
					break;
				case SDLK_RIGHT:
					sendMessage(m_connectionSocket, MESSAGE_PLAYER_PRESSED_KEY, toKeyCodeByte(SDLK_RIGHT) + gameContext.getPlayerNumber());
					break;
				case SDLK_DOWN:
					sendMessage(m_connectionSocket, MESSAGE_PLAYER_PRESSED_KEY, toKeyCodeByte(SDLK_DOWN) + gameContext.getPlayerNumber());
					break;
				case SDLK_UP:
					sendMessage(m_connectionSocket, MESSAGE_PLAYER_PRESSED_KEY, toKeyCodeByte(SDLK_UP) + gameContext.getPlayerNumber());
					break;
				case SDLK_ESCAPE:
					sendMessage(m_connectionSocket, MESSAGE_PLAYER_PRESSED_KEY, toKeyCodeByte(SDLK_ESCAPE) + gameContext.getPlayerNumber());
					break;
				case SDLK_SPACE:
					sendMessage(m_connectionSocket, MESSAGE_PLAYER_PRESSED_KEY, toKeyCodeByte(SDLK_SPACE) + gameContext.getPlayerNumber());
					break;
				default:
					break;
				}
				break;
			default:
				break;

			}
		}
	}


public:
	ClientNetworkTask(SOCKET m_connectionSocketTCPin, SOCKET m_connectionSocketUDPin,
		SOCKET m_connectionSocketTCPout, SOCKET m_connectionSocketUDPout, GameContext& m_gameContext, Client* m_client) : gameContext(m_gameContext) {
		connectionSocketTCPin = m_connectionSocketTCPin;
		connectionSocketUDPin = m_connectionSocketUDPin;
		connectionSocketTCPout = m_connectionSocketTCPout;
		connectionSocketUDPout = m_connectionSocketUDPout;
		client = m_client;
	}

	~ClientNetworkTask() {

	}

	void run() {
		taskThreadTCPin = std::thread(ClientNetworkTask::taskJobTCPIn, connectionSocketTCPin, connectionSocketTCPout, gameContext);
		taskThreadUDPin = std::thread(ClientNetworkTask::taskJobUDPIn, connectionSocketUDPin, connectionSocketUDPout, gameContext);
		taskThreadTCPout = std::thread(ClientNetworkTask::taskJobTCPOut, connectionSocketTCPout, connectionSocketTCPin, gameContext);
		taskThreadUDPout = std::thread(ClientNetworkTask::taskJobUDPOut, connectionSocketUDPout, connectionSocketUDPin, gameContext);
	}

	void notifyGameStateUpdate(const GAME_STATE& updatedGameState) {
		gameContext.setGameState(updatedGameState);
	}

};