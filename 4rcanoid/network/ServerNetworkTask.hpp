#pragma once

class ServerNetworkTask : public NetworkTask {

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

	int clientListeningUdpPort;

	ThreadSafeQueue<std::string> tcpMessagesToBroadcast;
	ThreadSafeQueue<std::string> udpMessagesToBroadcast;
	ThreadSafeQueue<std::string> udpIncomingMessagesQueue;
	ThreadSafeQueue<std::string> udpOutcomingMessagesQueue;

	std::string receiveMessage(SOCKET m_connectionSocket) {
		if (validNetworkTask)
			return MessageReceiverSender::receiveTCPMessage(m_connectionSocket);
	}
	

	void sendMessage(SOCKET m_connectionSocket, const std::string& message) {
		if (validNetworkTask)
			MessageReceiverSender::sendTCPMessage(m_connectionSocket, message);
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
			while (server->getIsServerRunning()) {
				receivedMessage = receiveMessage(connectionSocketTCPin);
				if (receivedMessage != "") {
					auto got = messagesHandlers.find(receivedMessage[0]);
					if (got != messagesHandlers.end()) {
						got->second->execute(this, receivedMessage, gameContext);
						server->broadcastMessage(IPPROTO_TCP, receivedMessage, peerPlayerNumber);
					}
					else {
						throw NetworkException("Unknown message: " + receivedMessage);
					}
				}
				else {
					//client disconnected
					gameContext.removePlayer(getPeerIp(connectionSocketTCPin));
					server->goBackToLobby();
					validNetworkTask = false;
					break;
				}

			}
		}
		catch (NetworkException& e) {
			printf((e.getMessage() + "\n").c_str());
			//client disconnected
			gameContext.removePlayer(getPeerIp(connectionSocketTCPin));
			server->goBackToLobby();
			validNetworkTask = false;
		}
		
	}

	void taskJobTCPOut() {
		try {
			while (server->getIsServerRunning()) {
				gameContext.addPlayer(getPeerIp(connectionSocketTCPout));
				std::string message = charToOneCharString(MESSAGE_PLAYER_ACCEPTED) + peerPlayerNumber + "|" + gameContext.getBlocksDescription();
				printf("Message: %d \n", message[0]);
				sendMessage(connectionSocketTCPout, message);
				break;
			}
		}
		catch (NetworkException& e) {
			printf((e.getMessage() + "\n").c_str());
		}
		
		
	}

	/* * /
	void taskJobUDPIn() {
		std::string receivedMessage;
		try {
			while (server->getIsServerRunning()) {
				if (!udpIncomingMessagesQueue.empty()) {
					receivedMessage = udpIncomingMessagesQueue.pop();
					auto got = messagesHandlers.find(receivedMessage[0]);
					if (got != messagesHandlers.end()) {
						got->second->execute(this, receivedMessage, gameContext);
						server->broadcastMessage(IPPROTO_UDP, receivedMessage, peerPlayerNumber);
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
		
	}
	/* */
	
	public:
		ServerNetworkTask(SOCKET m_connectionSocketTCPin, SOCKET m_connectionSocketUDPin,
			SOCKET m_connectionSocketTCPout, SOCKET m_connectionSocketUDPout, const std::string& m_peerIpAddress,
			GameContext& m_gameContext, Server* m_server, const char& m_peerPlayerNumber)
			: NetworkTask(PROGRAM_SERVER), gameContext(m_gameContext) {
			connectionSocketTCPin = m_connectionSocketTCPin;
			connectionSocketTCPout = m_connectionSocketTCPout;
			peerPlayerNumber = m_peerPlayerNumber;
			server = m_server;
			validNetworkTask = true;
			peerIpAddress = m_peerIpAddress;
			clientListeningUdpPort = -1;
		}

		void setClientListeningUdpPort(const int& m_port) {
			clientListeningUdpPort = m_port;
		}

		int getListeningUdpPort() {
			return clientListeningUdpPort;
		}

		void closeSockets() {
			closesocket(connectionSocketTCPin);
			closesocket(connectionSocketTCPout);
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

		void s_sendMessage(const IPPROTO& protocol, const std::string& message) {
			if (protocol == IPPROTO_TCP)
				MessageReceiverSender::sendTCPMessage(connectionSocketTCPout, message);
		}

		bool getIsValidTask() {
			return validNetworkTask;
		}

		char getPlayerId() const {
			return peerPlayerNumber;
		}



		void run() {
			taskThreadTCPin  = std::thread(&ServerNetworkTask::taskJobTCPIn, this);
			//taskThreadUDPin  = std::thread(&ServerNetworkTask::taskJobUDPIn, this);
			taskThreadTCPout = std::thread(&ServerNetworkTask::taskJobTCPOut, this);
			//taskThreadUDPout = std::thread(&ServerNetworkTask::taskJobUDPOut, this);
		}

		void notifyGameStateUpdate(const GAME_STATE& updatedGameState) {
			gameContext.setGameState(updatedGameState);
		}
};
