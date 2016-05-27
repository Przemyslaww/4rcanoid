#pragma once

class ServerNetworkTask {

	SOCKET connectionSocketTCPin;
	SOCKET connectionSocketTCPout;

	std::thread taskThreadTCPin;
	std::thread taskThreadTCPout;
	std::thread taskThreadUDPin;
	std::thread taskThreadUDPout;
	GameContext& gameContext;

	bool validNetworkTask;

	Server* server;

	char peerPlayerNumber;
	std::string peerIpAddress;

	ThreadSafeQueue<std::string> tcpMessagesToBroadcast;
	ThreadSafeQueue<std::string> udpMessagesToBroadcast;
	ThreadSafeQueue<std::string> udpIncomingMessagesQueue;
	ThreadSafeQueue<std::string> udpOutcomingMessagesQueue;

	static std::string receiveMessage(SOCKET m_connectionSocket, ServerNetworkTask* networkTask) {
		if (networkTask->validNetworkTask)
			return MessageReceiverSender::receiveTCPMessage(m_connectionSocket);
	}
	

	static void sendMessage(SOCKET m_connectionSocket, ServerNetworkTask* networkTask, const char& messageType, const std::string& message) {
		if (networkTask->validNetworkTask)
			MessageReceiverSender::sendTCPMessage(m_connectionSocket, MessageReceiverSender::createMessage(messageType, message));
	}

	static std::string getPeerIp(SOCKET m_connectionSocket) {
		return MessageReceiverSender::getPeerIpTCP(m_connectionSocket);
	}

	static std::string getLocalIp(SOCKET m_connectionSocket) {
		return MessageReceiverSender::getLocalIp(m_connectionSocket);
	}

	static void taskJobTCPIn(ServerNetworkTask* networkTask) {
		std::string receivedMessage;
		while (true) {
			receivedMessage = receiveMessage(networkTask->connectionSocketTCPin, networkTask);
			if (receivedMessage != "") {
				auto got = messagesHandlers.find(receivedMessage[0]);
				if (got != messagesHandlers.end()) {
					got->second->execute(receivedMessage, networkTask->gameContext);
					networkTask->server->broadcastMessage(IPPROTO_TCP, receivedMessage, networkTask->peerPlayerNumber);
				}
				else {
					throw NetworkException("Unkown message: " + receivedMessage);
				}
			}
			else {
				//client disconnected
				networkTask->gameContext.removePlayer(getPeerIp(networkTask->connectionSocketTCPin));
				networkTask->server->goBackToLobby();
				networkTask->validNetworkTask = false;
				break;
			}
			
		}
	}

	static void taskJobTCPOut(ServerNetworkTask* networkTask) {
		while (true) {
			networkTask->gameContext.addPlayer(getPeerIp(networkTask->connectionSocketTCPout));
			sendMessage(networkTask->connectionSocketTCPout, networkTask, MESSAGE_PLAYER_ACCEPTED, networkTask->peerPlayerNumber + "|" + networkTask->gameContext.getBlocksDescription());
			networkTask->gameContext.addPlayer("sent message");
			while (networkTask->gameContext.getGameState() == GAME_LOBBY);
			sendMessage(networkTask->connectionSocketTCPout, networkTask, MESSAGE_GAME_START, "");
			while (networkTask->gameContext.getGameState() == GAME_PLAY);
			sendMessage(networkTask->connectionSocketTCPout, networkTask, MESSAGE_GAME_INTERRUPTED, "");
			networkTask->gameContext.removePlayer(getPeerIp(networkTask->connectionSocketTCPout));
			while (true);
		}
		
	}

	static void taskJobUDPIn(ServerNetworkTask* networkTask) {
		std::string receivedMessage;
		while (true) {
			if (!networkTask->udpIncomingMessagesQueue.empty()) {
				receivedMessage = networkTask->udpIncomingMessagesQueue.pop();
				auto got = messagesHandlers.find(receivedMessage[0]);
				if (got != messagesHandlers.end()) {
					got->second->execute(receivedMessage, networkTask->gameContext);
					networkTask->server->broadcastMessage(IPPROTO_UDP, receivedMessage, networkTask->peerPlayerNumber);
				}
				else {
					throw NetworkException("Unknown message: " + receivedMessage);
				}
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



	static void taskJobUDPOut(ServerNetworkTask* networkTask) {
		while (true) {
			SDL_Event event = networkTask->gameContext.getCurrentSDLEvent();
			switch (event.type) {
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_LEFT:
					networkTask->pushUDPMessageToSend(MessageReceiverSender::createMessage(MESSAGE_PLAYER_PRESSED_KEY, toKeyCodeByte(SDLK_LEFT) + networkTask->gameContext.getPlayerNumber()));
					break;
				case SDLK_RIGHT:
					networkTask->pushUDPMessageToSend(MessageReceiverSender::createMessage(MESSAGE_PLAYER_PRESSED_KEY, toKeyCodeByte(SDLK_RIGHT) + networkTask->gameContext.getPlayerNumber()));
					break;
				case SDLK_DOWN:
					networkTask->pushUDPMessageToSend(MessageReceiverSender::createMessage(MESSAGE_PLAYER_PRESSED_KEY, toKeyCodeByte(SDLK_DOWN) + networkTask->gameContext.getPlayerNumber()));
					break;
				case SDLK_UP:
					networkTask->pushUDPMessageToSend(MessageReceiverSender::createMessage(MESSAGE_PLAYER_PRESSED_KEY, toKeyCodeByte(SDLK_UP) + networkTask->gameContext.getPlayerNumber()));
					break;
				case SDLK_ESCAPE:
					networkTask->pushUDPMessageToSend(MessageReceiverSender::createMessage(MESSAGE_PLAYER_PRESSED_KEY, toKeyCodeByte(SDLK_ESCAPE) + networkTask->gameContext.getPlayerNumber()));
					break;
				case SDLK_SPACE:
					networkTask->pushUDPMessageToSend(MessageReceiverSender::createMessage(MESSAGE_PLAYER_PRESSED_KEY, toKeyCodeByte(SDLK_SPACE) + networkTask->gameContext.getPlayerNumber()));
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
			SOCKET m_connectionSocketTCPout, SOCKET m_connectionSocketUDPout, const std::string& m_peerIpAddress,
			GameContext& m_gameContext, Server* m_server, const char& m_peerPlayerNumber) : gameContext(m_gameContext) {
			connectionSocketTCPin = m_connectionSocketTCPin;
			connectionSocketTCPout = m_connectionSocketTCPout;
			peerPlayerNumber = m_peerPlayerNumber;
			server = m_server;
			validNetworkTask = true;
			peerIpAddress = m_peerIpAddress;
		}

		void pushUDPMessageToBroadcast(const std::string& message) {
			udpMessagesToBroadcast.emplace(message);
		}

		void pushTCPMessageToBroadcast(const std::string& message) {
			tcpMessagesToBroadcast.emplace(message);
		}

		void pushUDPMessageToReceive(const std::string& message) {
			udpIncomingMessagesQueue.emplace(message);
		}

		void pushUDPMessageToSend(const std::string& message) {
			udpOutcomingMessagesQueue.emplace(message);
		}

		std::string popUDPMessageToSend() {
			return udpOutcomingMessagesQueue.pop();
		}

		bool hasOutcomingUDPMessages() {
			return !udpOutcomingMessagesQueue.empty();
		}

		std::string getClientIpAddress() {
			return peerIpAddress;
		}

		~ServerNetworkTask() {

		}

		void s_sendMessage(const IPPROTO& protocol, const char& messageType, const std::string& message) {
			if (protocol == IPPROTO_TCP)
				MessageReceiverSender::sendTCPMessage(connectionSocketTCPout, MessageReceiverSender::createMessage(messageType, message));
			else if (protocol == IPPROTO_UDP)
				pushUDPMessageToSend(MessageReceiverSender::createMessage(messageType, message));
		}

		bool getIsValidTask() {
			return validNetworkTask;
		}

		char getPlayerId() const {
			return peerPlayerNumber;
		}



		void run() {
			taskThreadTCPin  = std::thread(ServerNetworkTask::taskJobTCPIn, this);
			taskThreadUDPin  = std::thread(ServerNetworkTask::taskJobUDPIn, this);
			taskThreadTCPout = std::thread(ServerNetworkTask::taskJobTCPOut, this);
			taskThreadUDPout = std::thread(ServerNetworkTask::taskJobUDPOut, this);
		}

		void notifyGameStateUpdate(const GAME_STATE& updatedGameState) {
			gameContext.setGameState(updatedGameState);
		}
};
