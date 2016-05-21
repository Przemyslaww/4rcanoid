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
	char playerNumber;

	static std::string getPeerIp(SOCKET socket) {
		sockaddr client_info = { 0 };
		int addrsize = sizeof(client_info);
		getpeername(socket, &client_info, &addrsize);
		return inet_ntoa(((sockaddr_in*)&client_info)->sin_addr);
	}

	static std::string getLocalIp(SOCKET socket) {
		sockaddr client_info = { 0 };
		int addrsize = sizeof(client_info);
		getsockname(socket, &client_info, &addrsize);
		return inet_ntoa(((sockaddr_in*)&client_info)->sin_addr);
	}

	static void sendMessage(SOCKET socket, char messageType, const std::string& message) {
		char buffer[BUFFER_SIZE];
		buffer[0] = messageType;
		for (int i = 0; i < message.length(); i++) {
			buffer[i + 1] = message[i];
		}
		buffer[message.length()+1] = 0;
		int iSendResult = send(socket, buffer, message.length(), 0);
		if (iSendResult == SOCKET_ERROR) {
			closesocket(socket);
			WSACleanup();
			throw NetworkException("Send failed: " + intToStr(WSAGetLastError()));
		}
	}

	static std::string receiveMessage(SOCKET socket) {
		char buffer[BUFFER_SIZE] = {0};
		int iResult = recv(socket, buffer, BUFFER_SIZE, 0);
		return buffer;
	}

	static void taskJobTCPIn(SOCKET m_connectionSocket, GameContext& gameContext,
		const char& playerNumber, const char& peerPlayerNumber) {
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
		const char& playerNumber, const char& peerPlayerNumber) {
		while (true) {
			while (gameContext.getGameState() != GAME_LOBBY);
			gameContext.addPlayer(getPeerIp(m_connectionSocket));
			//send blocks description to client when player joins the game
			sendMessage(m_connectionSocket, MESSAGE_PLAYER_ACCEPTED, gameContext.getBlocksDescription());
			while (true);
		}
		
	}

	static void taskJobUDPIn(SOCKET m_connectionSocket, GameContext& gameContext,
		const char& playerNumber, const char& peerPlayerNumber) {
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
		const char& playerNumber, const char& peerPlayerNumber) {
		while (true) {
			SDL_Event event = gameContext.getCurrentSDLEvent();
			switch (event.type) {
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_LEFT:
					sendMessage(m_connectionSocket, MESSAGE_PLAYER_PRESSED_KEY, toKeyCodeByte(SDLK_LEFT) + playerNumber);
					break;
				case SDLK_RIGHT:
					sendMessage(m_connectionSocket, MESSAGE_PLAYER_PRESSED_KEY, toKeyCodeByte(SDLK_RIGHT) + playerNumber);
					break;
				case SDLK_DOWN:
					sendMessage(m_connectionSocket, MESSAGE_PLAYER_PRESSED_KEY, toKeyCodeByte(SDLK_DOWN) + playerNumber);
					break;
				case SDLK_UP:
					sendMessage(m_connectionSocket, MESSAGE_PLAYER_PRESSED_KEY, toKeyCodeByte(SDLK_UP) + playerNumber);
					break;
				case SDLK_ESCAPE:
					sendMessage(m_connectionSocket, MESSAGE_PLAYER_PRESSED_KEY, toKeyCodeByte(SDLK_ESCAPE) + playerNumber);
					break;
				case SDLK_SPACE:
					sendMessage(m_connectionSocket, MESSAGE_PLAYER_PRESSED_KEY, toKeyCodeByte(SDLK_SPACE) + playerNumber);
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
			SOCKET m_connectionSocketTCPout, SOCKET m_connectionSocketUDPout, GameContext& m_gameContext,
			const char& m_playerNumber, const char& m_peerPlayerNumber) : gameContext(m_gameContext) {
			connectionSocketTCPin = m_connectionSocketTCPin;
			connectionSocketUDPin = m_connectionSocketUDPin;
			connectionSocketTCPout = m_connectionSocketTCPout;
			connectionSocketUDPout = m_connectionSocketUDPout;
			taskThreadTCPin = NULL;
			taskThreadUDPin = NULL;
			taskThreadTCPout = NULL;
			taskThreadUDPout = NULL;
			peerPlayerNumber = m_peerPlayerNumber;
			playerNumber = m_playerNumber;
		}

		~ServerNetworkTask() {
			if (taskThreadTCPin) delete taskThreadTCPin;
			if (taskThreadUDPin) delete taskThreadUDPin;
			if (taskThreadTCPout) delete taskThreadTCPout;
			if (taskThreadUDPout) delete taskThreadUDPout;
		}

		void run() {
			taskThreadTCPin  = new std::thread(ServerNetworkTask::taskJobTCPIn, connectionSocketTCPin, gameContext, playerNumber, peerPlayerNumber);
			taskThreadUDPin  = new std::thread(ServerNetworkTask::taskJobUDPIn, connectionSocketUDPin, gameContext, playerNumber, peerPlayerNumber);
			taskThreadTCPout = new std::thread(ServerNetworkTask::taskJobTCPOut, connectionSocketTCPout, gameContext, playerNumber, peerPlayerNumber);
			taskThreadUDPout = new std::thread(ServerNetworkTask::taskJobUDPOut, connectionSocketUDPout, gameContext, playerNumber, peerPlayerNumber);
		}

		void notifyGameStateUpdate(const GAME_STATE& updatedGameState) {
			gameContext.setGameState(updatedGameState);
		}

		
};
