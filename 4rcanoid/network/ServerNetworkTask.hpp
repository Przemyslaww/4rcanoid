#pragma once

class ServerNetworkTask {

	SOCKET connectionSocketTCPin;
	SOCKET connectionSocketUDPin;
	SOCKET connectionSocketTCPout;
	SOCKET connectionSocketUDPout;

	std::thread* taskThreadTCPin;
	std::thread* taskThreadUDPin;
	std::thread* taskThreadTCPout;
	std::thread* taskThreadUDPout;
	GameContext& gameContext;

	char peerPlayerNumber;

	static std::string receiveMessage(SOCKET m_connectionSocket) {
		return MessageReceiverSender::receiveMessage(m_connectionSocket);
	}

	static void sendMessage(SOCKET m_connectionSocket, const char& messageType, const std::string& message) {
		MessageReceiverSender::sendMessage(m_connectionSocket, messageType, message);
	}

	static std::string getPeerIp(SOCKET m_connectionSocket) {
		return MessageReceiverSender::getPeerIp(m_connectionSocket);
	}

	static std::string getLocalIp(SOCKET m_connectionSocket) {
		return MessageReceiverSender::getLocalIp(m_connectionSocket);
	}

	static void taskJobTCPIn(SOCKET m_connectionSocket, GameContext& gameContext,
		const char& peerPlayerNumber) {
		std::string receivedMessage;
		while (true) {
			receivedMessage = receiveMessage(m_connectionSocket);
			auto got = messagesHandlers.find(receivedMessage[0]);
			if (got != messagesHandlers.end()) {
				got->second->execute(receivedMessage, gameContext, m_connectionSocket);
			}
			else {
				throw NetworkException("Unkown message: " + receivedMessage);
			}
		}
	}

	static void taskJobTCPOut(SOCKET m_connectionSocket, GameContext& gameContext,
		const char& peerPlayerNumber) {
		while (true) {
			while (gameContext.getGameState() != GAME_LOBBY);
			gameContext.addPlayer(getPeerIp(m_connectionSocket));
			sendMessage(m_connectionSocket, MESSAGE_PLAYER_ACCEPTED, peerPlayerNumber + "|" + gameContext.getBlocksDescription());
			while (gameContext.getGameState() == GAME_LOBBY);
			sendMessage(m_connectionSocket, MESSAGE_GAME_START, "");
			while (gameContext.getGameState() == GAME_PLAY);
			sendMessage(m_connectionSocket, MESSAGE_GAME_INTERRUPTED, "");
			while (true);
		}
		
	}

	static void taskJobUDPIn(SOCKET m_connectionSocket, GameContext& gameContext,
		const char& peerPlayerNumber) {
		std::string receivedMessage;
		while (true) {
			receivedMessage = receiveMessage(m_connectionSocket);
			auto got = messagesHandlers.find(receivedMessage[0]);
			if (got != messagesHandlers.end()) {
				got->second->execute(receivedMessage, gameContext, m_connectionSocket);
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

	static void taskJobUDPOut(SOCKET m_connectionSocket, GameContext& gameContext,
		const char& peerPlayerNumber) {
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
		ServerNetworkTask(SOCKET m_connectionSocketTCPin, SOCKET m_connectionSocketUDPin,
			SOCKET m_connectionSocketTCPout, SOCKET m_connectionSocketUDPout, GameContext& m_gameContext, const char& m_peerPlayerNumber) : gameContext(m_gameContext) {
			connectionSocketTCPin = m_connectionSocketTCPin;
			connectionSocketUDPin = m_connectionSocketUDPin;
			connectionSocketTCPout = m_connectionSocketTCPout;
			connectionSocketUDPout = m_connectionSocketUDPout;
			taskThreadTCPin = NULL;
			taskThreadUDPin = NULL;
			taskThreadTCPout = NULL;
			taskThreadUDPout = NULL;
			peerPlayerNumber = m_peerPlayerNumber;
		}

		~ServerNetworkTask() {
			if (taskThreadTCPin) delete taskThreadTCPin;
			if (taskThreadUDPin) delete taskThreadUDPin;
			if (taskThreadTCPout) delete taskThreadTCPout;
			if (taskThreadUDPout) delete taskThreadUDPout;
		}

		void run() {
			taskThreadTCPin  = new std::thread(ServerNetworkTask::taskJobTCPIn, connectionSocketTCPin, gameContext, peerPlayerNumber);
			taskThreadUDPin  = new std::thread(ServerNetworkTask::taskJobUDPIn, connectionSocketUDPin, gameContext, peerPlayerNumber);
			taskThreadTCPout = new std::thread(ServerNetworkTask::taskJobTCPOut, connectionSocketTCPout, gameContext, peerPlayerNumber);
			taskThreadUDPout = new std::thread(ServerNetworkTask::taskJobUDPOut, connectionSocketUDPout, gameContext, peerPlayerNumber);
		}

		void notifyGameStateUpdate(const GAME_STATE& updatedGameState) {
			gameContext.setGameState(updatedGameState);
		}

		
};
