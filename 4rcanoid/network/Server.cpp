#include "../header.hpp"

void Server::createSocket(SOCKET& m_socket, addrinfo& hints, addrinfo*& result, const IPPROTO& protocol, const int& port) {
	if (protocol == IPPROTO_TCP) {
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		if (protocol == IPPROTO_TCP)
			hints.ai_socktype = SOCK_STREAM;
		else if (protocol == IPPROTO_UDP)
			hints.ai_socktype = SOCK_DGRAM;
		hints.ai_protocol = protocol;
		hints.ai_flags = AI_PASSIVE;

		int iResult = getaddrinfo(NULL, intToStr(port).c_str(), &hints, &result);
		if (iResult != 0) {
			throw NetworkException(std::string("getaddrinfo failed: ") + intToStr(iResult));
			WSACleanup();
		}
		m_socket = INVALID_SOCKET;
		m_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (m_socket == INVALID_SOCKET || m_socket == SOCKET_ERROR) {
			freeaddrinfo(result);
			throw NetworkException(std::string("Error at creating socket TCP: ") + intToStr(WSAGetLastError()) + " port " + intToStr(port));
			WSACleanup();
		}
	}
	else {
		m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (m_socket == INVALID_SOCKET || m_socket == SOCKET_ERROR) {
			throw NetworkException(std::string("Error at creating socket UDP: ") + intToStr(WSAGetLastError()) + " port " + intToStr(port));
		}
	}
	
}

void Server::createTCPSockets() {
	createSocket(tcpListenSocketIn, hintsTCPin, resultTCPin, IPPROTO_TCP, DEFAULT_SERVER_PORT_TCP_IN);
	createSocket(tcpListenSocketOut, hintsTCPout, resultTCPout, IPPROTO_TCP, DEFAULT_SERVER_PORT_TCP_OUT);
}

void Server::createUDPSockets() {
	createSocket(udpListenSocketIn, hintsUDPin, resultUDPin, IPPROTO_UDP, DEFAULT_SERVER_PORT_UDP_IN);
	createSocket(udpListenSocketOut, hintsUDPout, resultUDPout, IPPROTO_UDP, DEFAULT_SERVER_PORT_UDP_OUT);
}

void Server::bindTCPSocket(SOCKET listenSocket, addrinfo* result) {
	int iResult = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		freeaddrinfo(result);
		closesocket(listenSocket);
		throw NetworkException(std::string("bind TCP failed with error: ") + intToStr(WSAGetLastError()));
		WSACleanup();
	}
	freeaddrinfo(result);
}

void Server::bindUDPSocket(SOCKET listenSocket, const int& port) {
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);
	if (bind(listenSocket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
		throw NetworkException(std::string("bind UDP failed with error: ") + intToStr(WSAGetLastError()));
	}
}

void Server::listenOnSocket(SOCKET listenSocket) {
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		closesocket(listenSocket);
		throw NetworkException(std::string("Listen failed with error: ") + intToStr(WSAGetLastError()));
		WSACleanup();
	}
}

void Server::listenOnSockets() {
	listenOnSocket(tcpListenSocketIn);
	listenOnSocket(tcpListenSocketOut);
}


Server::Server(GameContext& m_gameContext) : gameContext(m_gameContext) {
	players = 0;
	serverRunning = true;
	networkTasksLoopCounter = 0;
}

Server::~Server() {
	for (auto& networkTask : networkTasks) delete networkTask;
}

		int Server::initialize() {
			int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
			if (result == 0) {
				return 0;
			}
			else throw NetworkException("Error at WSAStartup: " + intToStr(result));
		}

		void Server::createServerSockets() {
			createTCPSockets();
			createUDPSockets();
		}

		void Server::bindSockets() {
			bindTCPSocket(tcpListenSocketIn, resultTCPin);
			bindUDPSocket(udpListenSocketIn, DEFAULT_SERVER_PORT_UDP_IN);
			bindTCPSocket(tcpListenSocketOut, resultTCPout);
			bindUDPSocket(udpListenSocketOut, DEFAULT_SERVER_PORT_UDP_OUT);
		}

		void Server::closeSocket(SOCKET clientSocket) {
			closesocket(clientSocket);
		}

		void Server::disconnectSocket(SOCKET clientSocket) {
			int iResult = shutdown(clientSocket, SD_SEND);
			if (iResult == SOCKET_ERROR) {
				closesocket(clientSocket);
				throw NetworkException(std::string("Socket shutdown failed: ") + intToStr(WSAGetLastError()));
				WSACleanup();
			}
		}

		bool Server::getIsServerRunning() {
			return serverRunning;
		}

		void Server::broadcastMessage(const IPPROTO& protocol, const std::string& message, const char& playerMessageIncomingNumber) {
			try {
				std::lock_guard<std::mutex> networkTasksCollectionLock(mutexUDP);
				for (auto& serverNetworkTask : networkTasks) {
					if (serverNetworkTask->getPlayerId() != playerMessageIncomingNumber) {
						if (protocol == IPPROTO_TCP)
							serverNetworkTask->s_sendMessage(protocol, message);
						else if (protocol == IPPROTO_UDP) {
							MessageReceiverSender::sendUDPMessage(udpListenSocketOut,
								serverNetworkTask->getListeningUdpPort(),
								serverNetworkTask->getClientIpAddress(),
								message);
							//printf("Sent UDP %d\n", message.length());
						}
					}
				}
			}
			catch (NetworkException& e) {
				printf((e.getMessage() + "\n").c_str());
			}
			
		}

		void Server::goBackToLobby() {
			std::lock_guard<std::mutex> lockGuard(mutex);
			players--;
			gameContext.setGameState(GAME_LOBBY);
			std::string gameInterruptedMessage = charToOneCharString(MESSAGE_GAME_INTERRUPTED);
			broadcastMessage(IPPROTO_TCP, gameInterruptedMessage, ALL_PLAYERS);
		}

		void Server::stop() {
			printf("Server stopped\n");
			serverRunning = false;
			std::lock_guard<std::mutex> networkTasksCollectionLock(mutexUDP);
			for (auto& networkTask : networkTasks) {
				networkTask->closeSockets();
				networkTask->join();
			}
			networkTasks.clear();
			closeSocket(tcpListenSocketIn);
			closeSocket(udpListenSocketOut);
			closeSocket(tcpListenSocketOut);
			closeSocket(udpListenSocketIn);
		}

		void Server::sendUDP() {
				try {
					while (getIsServerRunning()) {
						SDL_Event event = gameContext.getCurrentSDLEvent();
						switch (event.type) {
						case SDL_KEYDOWN:
							switch (event.key.keysym.sym) {
							case SDLK_LEFT:
								broadcastMessage(IPPROTO_UDP, MessageReceiverSender::createMessage(MESSAGE_PLAYER_PRESSED_KEY, toKeyCodeByte(SDLK_LEFT) + gameContext.getPlayerNumber()), ALL_PLAYERS);
								break;
							case SDLK_RIGHT:
								broadcastMessage(IPPROTO_UDP, MessageReceiverSender::createMessage(MESSAGE_PLAYER_PRESSED_KEY, toKeyCodeByte(SDLK_RIGHT) + gameContext.getPlayerNumber()), ALL_PLAYERS);
								break;
							case SDLK_DOWN:
								broadcastMessage(IPPROTO_UDP, MessageReceiverSender::createMessage(MESSAGE_PLAYER_PRESSED_KEY, toKeyCodeByte(SDLK_DOWN) + gameContext.getPlayerNumber()), ALL_PLAYERS);
								break;
							case SDLK_UP:
								broadcastMessage(IPPROTO_UDP, MessageReceiverSender::createMessage(MESSAGE_PLAYER_PRESSED_KEY, toKeyCodeByte(SDLK_UP) + gameContext.getPlayerNumber()), ALL_PLAYERS);
								break;
							case SDLK_ESCAPE:
								broadcastMessage(IPPROTO_UDP, MessageReceiverSender::createMessage(MESSAGE_PLAYER_PRESSED_KEY, toKeyCodeByte(SDLK_ESCAPE) + gameContext.getPlayerNumber()), ALL_PLAYERS);
								break;
							case SDLK_SPACE:
								broadcastMessage(IPPROTO_UDP, MessageReceiverSender::createMessage(MESSAGE_PLAYER_PRESSED_KEY, toKeyCodeByte(SDLK_SPACE) + gameContext.getPlayerNumber()), ALL_PLAYERS);
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

		std::string Server::getServerIp() {
			return MessageReceiverSender::getLocalIp(tcpListenSocketIn);
		}

		ServerNetworkTask* Server::getNetworkTaskAssignedToIp(const std::string& m_ipAddress) {
			std::lock_guard<std::mutex> networkTasksCollectionLock(mutexUDP);
			for (int i = 0; i < networkTasks.size(); i++) {
				if (networkTasks[i]->getClientIpAddress() == m_ipAddress) return networkTasks[i];
			}
			return NULL;
		}

		void Server::listenUDP() {
			try {
				while (getIsServerRunning()) {
						auto result = MessageReceiverSender::receiveUDPMessage(udpListenSocketIn, DEFAULT_SERVER_PORT_UDP_IN);
						if (result.second != "") {
							auto got = messagesHandlers.find(result.second[0]);
							if (got != messagesHandlers.end()) {
								auto networkTask = getNetworkTaskAssignedToIp(result.first);
								got->second->execute(NULL, result.second, gameContext);
								broadcastMessage(IPPROTO_UDP, result.second, networkTask != NULL ? networkTask->getPlayerId() : ALL_PLAYERS);
							}
							else {
								throw NetworkException("Unknown message: " + intToStr(result.second[0]));
							}
						}
					}
			}
			catch (NetworkException& e) {
				printf((e.getMessage() + "\n").c_str());
			}

		}

		//this delegates consecutive connections to separate threads
		//and the procedure itself should be fired in separate thread
		void Server::run() {
			SOCKET clientSocketTCPin = INVALID_SOCKET;
			SOCKET clientSocketTCPout = INVALID_SOCKET;

			auto ipList = MessageReceiverSender::getLocalIps(tcpListenSocketIn);
			std::string welcomeMessage = "Listening on ";
			for (auto& ipAddress : ipList) {
				welcomeMessage += "[" + ipAddress + "]\n";
			}
			welcomeMessage += "\n--------------------------------------------------\n";
			gameContext.setTextInPlayersBox(welcomeMessage);
			gameContext.addPlayer(getServerIp());
			try {
				while (serverRunning) {

					if (players+1 >= MAX_PLAYERS) {
						//exit the lobby once we get all the players needed
						gameContext.setGameState(GAME_PLAY);
						broadcastMessage(IPPROTO_TCP, charToOneCharString(MESSAGE_GAME_START), ALL_PLAYERS);
						while (gameContext.getGameState() != GAME_LOBBY && serverRunning);// {
							//normal server work mode - in gameplay
							/* * /
							for (int i = 0; i < networkTasks.size(); i++) {
								if (!networkTasks[i]->getIsValidTask()) {
									std::lock_guard<std::mutex> networkTasksCollectionLock(mutexUDP);
									delete networkTasks[i];
									auto it = std::find(networkTasks.begin(), networkTasks.end(), networkTasks[i]);
									if (it != networkTasks.end()) {
										networkTasks.erase(it);
										i = 0;
										continue;
									}
								}
							}
							/* */
						//}
					}
					else {
						clientSocketTCPin = accept(tcpListenSocketIn, NULL, NULL);
						if (clientSocketTCPin == INVALID_SOCKET) {
							closesocket(tcpListenSocketIn);
							throw NetworkException(std::string("TCP in accept failed: ") + intToStr(WSAGetLastError()));
							WSACleanup();
						}
						clientSocketTCPout = accept(tcpListenSocketOut, NULL, NULL);
						if (clientSocketTCPout == INVALID_SOCKET) {
							closesocket(tcpListenSocketOut);
							throw NetworkException(std::string("TCP out accept failed: ") + intToStr(WSAGetLastError()));
							WSACleanup();
						}
						if (players+1 < MAX_PLAYERS) {
							players++;
							networkTasks.push_back(new ServerNetworkTask(clientSocketTCPin, udpListenSocketIn,
								clientSocketTCPout, udpListenSocketOut, 
								MessageReceiverSender::getPeerIpTCP(clientSocketTCPin) ,
								gameContext, this, players+1));
							networkTasks[networkTasks.size() - 1]->run();
						}
						else {
							disconnectSocket(clientSocketTCPin);
							disconnectSocket(clientSocketTCPout);
						}
					}
				}

				}
				catch (NetworkException& e) {
					gameContext.setTextInPlayersBox(e.getMessage());
				}

				
			
		}