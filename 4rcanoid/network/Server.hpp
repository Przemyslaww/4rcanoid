#pragma once

class Server {
	
	WSADATA wsaData;
	GameContext& gameContext;

	SOCKET tcpListenSocketIn;
	SOCKET udpListenSocketIn;
	SOCKET tcpListenSocketOut;
	SOCKET udpListenSocketOut;

	addrinfo* resultTCPin;
	addrinfo* resultUDPin;
	addrinfo* resultTCPout;
	addrinfo* resultUDPout;

	addrinfo hintsTCPin;
	addrinfo hintsUDPin;
	addrinfo hintsTCPout;
	addrinfo hintsUDPout;

	int players;

	std::vector<ServerNetworkTask*> networkTasks;

	void createSocket(SOCKET& m_socket, addrinfo& hints, addrinfo*& result, const IPPROTO& protocol, const int& port) {
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = protocol;
		hints.ai_flags = AI_PASSIVE;

		int iResult = getaddrinfo(NULL, intToStr(port).c_str(), &hints, &result);
		if (iResult != 0) {
			WSACleanup();
			throw NetworkException(std::string("getaddrinfo failed: ") + intToStr(iResult));
		}
		m_socket = INVALID_SOCKET;
		m_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (m_socket == INVALID_SOCKET) {
			freeaddrinfo(result);
			WSACleanup();
			throw NetworkException(std::string("Error at socket(): ") + intToStr(WSAGetLastError()));
		}
	}

	void createTCPSockets() {
		createSocket(tcpListenSocketIn, hintsTCPin, resultTCPin, IPPROTO_TCP, DEFAULT_SERVER_PORT_TCP_IN);
		createSocket(tcpListenSocketOut, hintsTCPout, resultTCPout, IPPROTO_TCP, DEFAULT_SERVER_PORT_TCP_OUT);
	}

	void createUDPSockets() {
		createSocket(udpListenSocketIn, hintsUDPin, resultUDPin, IPPROTO_UDP, DEFAULT_SERVER_PORT_UDP_IN);
		createSocket(udpListenSocketOut, hintsUDPout, resultUDPout, IPPROTO_UDP, DEFAULT_SERVER_PORT_UDP_OUT);
	}

	void bindSocket(SOCKET listenSocket, addrinfo* result) {
		int iResult = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			freeaddrinfo(result);
			closesocket(listenSocket);
			WSACleanup();
			throw NetworkException(std::string("bind failed with error: ") + intToStr(WSAGetLastError()));
		}
		freeaddrinfo(result);
	}

	void listenOnSocket(SOCKET listenSocket) {
		if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
			closesocket(listenSocket);
			WSACleanup();
			throw NetworkException(std::string("Listen failed with error: ") + intToStr(WSAGetLastError()));
		}
	}

	public:
		Server(GameContext& m_gameContext) : gameContext(m_gameContext) {
			players = 0;
		}

		~Server() {
			for (auto& networkTask : networkTasks) delete networkTask;
		}

		int initialize() {
			return WSAStartup(MAKEWORD(2, 2), &wsaData);
		}
		
		void createServerSockets() {
			createTCPSockets();
			createUDPSockets();
		}

		void bindSockets() {
			bindSocket(tcpListenSocketIn, resultTCPin);
			bindSocket(udpListenSocketIn, resultUDPin);
			bindSocket(tcpListenSocketOut, resultTCPout);
			bindSocket(udpListenSocketOut, resultUDPout);
		}
			

		void disconnectSocket(SOCKET clientSocket) {
			int iResult = shutdown(clientSocket, SD_SEND);
			if (iResult == SOCKET_ERROR) {
				closesocket(clientSocket);
				WSACleanup();
				throw NetworkException(std::string("Socket shutdown failed: ") + intToStr(WSAGetLastError()));
			}
		}

		//this delegates consecutive connections to separate threads
		//and the procedure itself should be fired in separate thread
		void acceptConnections() {
			SOCKET clientSocketTCPin = INVALID_SOCKET;
			SOCKET clientSocketUDPin = INVALID_SOCKET;
			SOCKET clientSocketTCPout = INVALID_SOCKET;
			SOCKET clientSocketUDPout = INVALID_SOCKET;
			while (true) {
				if (players >= MAX_PLAYERS) {
					//exit the lobby once we get all the players needed
					gameContext.setGameState(GAME_PLAY);
					break;
				}
				clientSocketTCPin = accept(tcpListenSocketIn, NULL, NULL);
				if (clientSocketTCPin == INVALID_SOCKET) {
					closesocket(tcpListenSocketIn);
					WSACleanup();
					throw NetworkException(std::string("TCP accept failed: ") + intToStr(WSAGetLastError()));
				}
				clientSocketUDPin = accept(udpListenSocketIn, NULL, NULL);
				if (clientSocketUDPin == INVALID_SOCKET) {
					closesocket(udpListenSocketIn);
					WSACleanup();
					throw NetworkException(std::string("UDP accept failed: ") + intToStr(WSAGetLastError()));
				}
				clientSocketTCPout = accept(tcpListenSocketOut, NULL, NULL);
				if (clientSocketTCPin == INVALID_SOCKET) {
					closesocket(tcpListenSocketIn);
					WSACleanup();
					throw NetworkException(std::string("TCP accept failed: ") + intToStr(WSAGetLastError()));
				}
				clientSocketUDPout = accept(udpListenSocketOut, NULL, NULL);
				if (clientSocketUDPin == INVALID_SOCKET) {
					closesocket(udpListenSocketIn);
					WSACleanup();
					throw NetworkException(std::string("UDP accept failed: ") + intToStr(WSAGetLastError()));
				}
				if (players < MAX_PLAYERS) {
					players++;
					//server is always player of number 0
					networkTasks.push_back(new ServerNetworkTask(clientSocketTCPin, clientSocketUDPin,
						clientSocketTCPout, clientSocketUDPout, gameContext, 0, players));
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

};