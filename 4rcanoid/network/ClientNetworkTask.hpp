#pragma once

class ClientNetworkTask : public NetworkTask {
	SOCKET connectionSocketTCPin;
	SOCKET connectionSocketUDPin;
	SOCKET connectionSocketTCPout;
	SOCKET connectionSocketUDPout;

	std::thread taskThreadTCPin;
	std::thread taskThreadUDPin;
	std::thread taskThreadTCPout;
	std::thread taskThreadUDPout;
	GameContext& gameContext;

	int listeningUdpPort;

	Client* client;

	char playerNumber;

	std::string receiveTCPMessage(SOCKET m_connectionSocket) {
		return MessageReceiverSender::receiveTCPMessage(m_connectionSocket);
	}

	//don't care about the source ip (in case of client)
	std::string receiveUDPMessage(SOCKET m_connectionSocket) {
		return MessageReceiverSender::receiveUDPMessage(m_connectionSocket, DEFAULT_SERVER_PORT_UDP_OUT, client->getServerIp()).second; 
	}

	void sendTCPMessage(SOCKET m_connectionSocket, const char& messageType, const std::string& message) {
		MessageReceiverSender::sendTCPMessage(m_connectionSocket,
			MessageReceiverSender::createMessage(messageType, message));
	}

	void sendUDPMessage(SOCKET m_connectionSocket, const char& messageType, const std::string& message) {
		//printf("Sending UDP...\n");
		MessageReceiverSender::sendUDPMessage(m_connectionSocket, DEFAULT_SERVER_PORT_UDP_IN, client->getServerIp(),
			MessageReceiverSender::createMessage(messageType, message));
	}

	std::string getPeerIp(SOCKET m_connectionSocket) {
		return MessageReceiverSender::getPeerIpTCP(m_connectionSocket);
	}

	std::string getLocalIp(SOCKET m_connectionSocket) {
		return MessageReceiverSender::getLocalIp(m_connectionSocket);
	}

	void taskJobTCPIn() {
		std::string receivedMessage;
		try {
			while (client->getIsClientRunning()) {
				receivedMessage = receiveTCPMessage(connectionSocketTCPin);
				if (receivedMessage != "") {
					auto got = messagesHandlers.find(receivedMessage[0]);

					if (got != messagesHandlers.end()) {
						got->second->execute(this, receivedMessage, gameContext);
					}
					else {
						throw NetworkException("Unknown message: " + receivedMessage);
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
		catch (NetworkException& e) {
			printf(("Exception: " + e.getMessage() + "\n").c_str());
			WSACleanup();
		}
		
	}

	void taskJobTCPOut() {
		try {
			sendTCPMessage(connectionSocketTCPout, MESSAGE_PLAYER_LISTENING_UDP_PORT, intToStr(listeningUdpPort));
		}
		catch (NetworkException& e) {
			printf((e.getMessage() + "\n").c_str());
		}
		
		//while (client->getIsClientRunning());
	}

	void taskJobUDPIn() {
		std::string receivedMessage;
		/* */
		try {
			while (client->getIsClientRunning()) {
				//printf("Receiving UDP..\n");
				receivedMessage = receiveUDPMessage(connectionSocketUDPin);
				if (receivedMessage != "") {
					auto got = messagesHandlers.find(receivedMessage[0]);
					if (got != messagesHandlers.end()) {
						got->second->execute(this, receivedMessage, gameContext);
					}
					else {
						throw NetworkException("Unknown message: " + receivedMessage);
					}
				}
			}
		}
		catch (NetworkException& e) {
			printf((e.getMessage() + "\n").c_str());
		}
		/* */
	}

	void taskJobUDPOut() {
		try {
			while (client->getIsClientRunning()) {
				SDL_Event event = gameContext.getCurrentSDLEvent();
				switch (event.type) {
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
					case SDLK_LEFT:
						sendUDPMessage(connectionSocketUDPout, MESSAGE_PLAYER_PRESSED_KEY, toKeyCodeByte(SDLK_LEFT) + gameContext.getPlayerNumber());
						break;
					case SDLK_RIGHT:
						sendUDPMessage(connectionSocketUDPout, MESSAGE_PLAYER_PRESSED_KEY, toKeyCodeByte(SDLK_RIGHT) + gameContext.getPlayerNumber());
						break;
					case SDLK_DOWN:
						sendUDPMessage(connectionSocketUDPout, MESSAGE_PLAYER_PRESSED_KEY, toKeyCodeByte(SDLK_DOWN) + gameContext.getPlayerNumber());
						break;
					case SDLK_UP:
						sendUDPMessage(connectionSocketUDPout, MESSAGE_PLAYER_PRESSED_KEY, toKeyCodeByte(SDLK_UP) + gameContext.getPlayerNumber());
						break;
					case SDLK_ESCAPE:
						sendUDPMessage(connectionSocketUDPout, MESSAGE_PLAYER_PRESSED_KEY, toKeyCodeByte(SDLK_ESCAPE) + gameContext.getPlayerNumber());
						break;
					case SDLK_SPACE:
						sendUDPMessage(connectionSocketUDPout, MESSAGE_PLAYER_PRESSED_KEY, toKeyCodeByte(SDLK_SPACE) + gameContext.getPlayerNumber());
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
		catch (NetworkException& e) {
			printf((e.getMessage() + "\n").c_str());
		}
		
	}


public:
	ClientNetworkTask(SOCKET m_connectionSocketTCPin, SOCKET m_connectionSocketUDPin,
		SOCKET m_connectionSocketTCPout, SOCKET m_connectionSocketUDPout, GameContext& m_gameContext,
		int m_listeningUdpPort, Client* m_client)
		: NetworkTask(PROGRAM_CLIENT), gameContext(m_gameContext) {
		connectionSocketTCPin = m_connectionSocketTCPin;
		connectionSocketUDPin = m_connectionSocketUDPin;
		connectionSocketTCPout = m_connectionSocketTCPout;
		connectionSocketUDPout = m_connectionSocketUDPout;
		client = m_client;
		listeningUdpPort = m_listeningUdpPort;
	}

	~ClientNetworkTask() {

	}

	void run() {
		taskThreadTCPin = std::thread(&ClientNetworkTask::taskJobTCPIn, this);
		taskThreadUDPin = std::thread(&ClientNetworkTask::taskJobUDPIn, this);
		taskThreadTCPout = std::thread(&ClientNetworkTask::taskJobTCPOut, this);
		taskThreadUDPout = std::thread(&ClientNetworkTask::taskJobUDPOut, this);
	}

	void join() {
		if (taskThreadTCPin.joinable()) taskThreadTCPin.join();
		printf("taskThreadTCPin stopped\n");
		if (taskThreadTCPout.joinable()) taskThreadTCPout.join();
		printf("taskThreadTCPout stopped\n");
		if (taskThreadUDPin.joinable()) taskThreadUDPin.join();
		printf("taskThreadUDPin stopped\n");
		if (taskThreadUDPout.joinable()) taskThreadUDPout.join();
		printf("taskThreadUDPout stopped\n");
	}

	void notifyGameStateUpdate(const GAME_STATE& updatedGameState) {
		gameContext.setGameState(updatedGameState);
	}

};