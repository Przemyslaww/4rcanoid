#include "../header.hpp"

void Server::createSocket(SOCKET& m_socket, addrinfo& hints, addrinfo*& result, const IPPROTO& protocol, const int& port) {
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
	m_socket = socket(result->ai_family, result->ai_socktype, protocol == IPPROTO_TCP ? result->ai_protocol : 0);
	if (m_socket == INVALID_SOCKET) {
		freeaddrinfo(result);
		throw NetworkException(std::string("Error at creating socket: ") + intToStr(WSAGetLastError()) + " port " + intToStr(port) + (protocol == IPPROTO_TCP ? " TCP " : " UDP "));
		WSACleanup();
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

void Server::bindSocket(SOCKET listenSocket, addrinfo* result) {
	int iResult = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		freeaddrinfo(result);
		closesocket(listenSocket);
		throw NetworkException(std::string("bind failed with error: ") + intToStr(WSAGetLastError()));
		WSACleanup();
	}
	freeaddrinfo(result);
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
	listenOnSocket(udpListenSocketIn);
	listenOnSocket(udpListenSocketOut);
}


		Server::Server(GameContext& m_gameContext) : gameContext(m_gameContext) {
			players = 0;
			serverRunning = true;
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
			bindSocket(tcpListenSocketIn, resultTCPin);
			bindSocket(udpListenSocketIn, resultUDPin);
			bindSocket(tcpListenSocketOut, resultTCPout);
			bindSocket(udpListenSocketOut, resultUDPout);
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

		void Server::broadcastMessage(const IPPROTO& protocol, const std::string& message, const char& playerMessageIncomingNumber) {
			for (auto& serverNetworkTask : networkTasks) {
				if (serverNetworkTask->getPlayerId() != playerMessageIncomingNumber) {
					serverNetworkTask->s_sendMessage(protocol, message[0], message);
				}
			}
		}

		void Server::stop() {
			closeSocket(tcpListenSocketIn);
			closeSocket(udpListenSocketOut);
			closeSocket(tcpListenSocketOut);
			closeSocket(udpListenSocketIn);
		}

		std::string Server::getServerIp() {
			return MessageReceiverSender::getPeerIp(tcpListenSocketIn);
		}

		//this delegates consecutive connections to separate threads
		//and the procedure itself should be fired in separate thread
		void Server::acceptConnections() {
			SOCKET clientSocketTCPin = INVALID_SOCKET;
			SOCKET clientSocketUDPin = INVALID_SOCKET;
			SOCKET clientSocketTCPout = INVALID_SOCKET;
			SOCKET clientSocketUDPout = INVALID_SOCKET;

			gameContext.setTextInPlayersBox("Listening on " + getServerIp() + "\n-----\n");
			try {
				while (true) {

					if (players >= MAX_PLAYERS) {
						//exit the lobby once we get all the players needed
						gameContext.setGameState(GAME_PLAY);
						break;
					}

					clientSocketTCPin = accept(tcpListenSocketIn, NULL, NULL);
					if (clientSocketTCPin == INVALID_SOCKET) {
						closesocket(tcpListenSocketIn);
						throw NetworkException(std::string("TCP accept failed: ") + intToStr(WSAGetLastError()));
						WSACleanup();
					}
					clientSocketUDPin = accept(udpListenSocketIn, NULL, NULL);
					if (clientSocketUDPin == INVALID_SOCKET) {
						closesocket(udpListenSocketIn);
						throw NetworkException(std::string("UDP accept failed: ") + intToStr(WSAGetLastError()));
						WSACleanup();
					}
					clientSocketTCPout = accept(tcpListenSocketOut, NULL, NULL);
					if (clientSocketTCPin == INVALID_SOCKET) {
						closesocket(tcpListenSocketIn);
						throw NetworkException(std::string("TCP accept failed: ") + intToStr(WSAGetLastError()));
						WSACleanup();
					}
					clientSocketUDPout = accept(udpListenSocketOut, NULL, NULL);
					if (clientSocketUDPin == INVALID_SOCKET) {
						closesocket(udpListenSocketIn);
						throw NetworkException(std::string("UDP accept failed: ") + intToStr(WSAGetLastError()));
						WSACleanup();
					}
					if (players < MAX_PLAYERS) {
						players++;
						//server is always player of number 0
						networkTasks.push_back(new ServerNetworkTask(clientSocketTCPin, clientSocketUDPin,
							clientSocketTCPout, clientSocketUDPout, gameContext, this, players));
						networkTasks[networkTasks.size() - 1]->run();
					}
					else {
						disconnectSocket(clientSocketTCPin);
						disconnectSocket(clientSocketUDPin);
						disconnectSocket(clientSocketTCPout);
						disconnectSocket(clientSocketUDPout);
					}
				}
				}
				catch (NetworkException& e) {
					gameContext.setTextInPlayersBox(e.getMessage());
				}
			
		}