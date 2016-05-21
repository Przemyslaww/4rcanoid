#pragma once

class Client {

	ClientNetworkTask* clientNetworkTask;

	GameContext& gameContext;

	SOCKET tcpConnectSocketIn;
	SOCKET udpConnectSocketIn;
	SOCKET tcpConnectSocketOut;
	SOCKET udpConnectSocketOut;

	addrinfo* resultTCPin;
	addrinfo* resultUDPin;
	addrinfo* resultTCPout;
	addrinfo* resultUDPout;

	addrinfo* ptrTCPin;
	addrinfo* ptrUDPin;
	addrinfo* ptrTCPout;
	addrinfo* ptrUDPout;

	addrinfo hintsTCPin;
	addrinfo hintsUDPin;
	addrinfo hintsTCPout;
	addrinfo hintsUDPout;

	std::string ipAddress;

	void createSocket(SOCKET& m_socket, addrinfo& hints, addrinfo*& result, const std::string& ipAddress, const IPPROTO& protocol, const int& port) {
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = protocol;
		hints.ai_flags = AI_PASSIVE;

		int iResult = getaddrinfo(ipAddress.c_str(), intToStr(port).c_str(), &hints, &result);
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

	void connectToSocket(SOCKET& m_socket, addrinfo* ptr, addrinfo* result) {
		int iResult = connect(m_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;
		}
		freeaddrinfo(result);

		if (m_socket == INVALID_SOCKET) {
			WSACleanup();
			throw NetworkException("Unable to connect to server!");
		}
	}

	public:
		Client(const std::string& m_ipAddress, GameContext& m_gameContext) : gameContext(m_gameContext) {
			clientNetworkTask = NULL;
			ipAddress = m_ipAddress;
		}

		~Client() {
			if (clientNetworkTask) delete clientNetworkTask;
		}

		void createTCPSockets() {
			createSocket(tcpConnectSocketIn, hintsTCPin, resultTCPin, ipAddress, IPPROTO_TCP, DEFAULT_SERVER_PORT_TCP_OUT);
			createSocket(tcpConnectSocketOut, hintsTCPout, resultTCPout, ipAddress, IPPROTO_TCP, DEFAULT_SERVER_PORT_TCP_IN);
		}

		void createUDPSockets() {
			createSocket(udpConnectSocketIn, hintsUDPin, resultUDPin, ipAddress, IPPROTO_UDP, DEFAULT_SERVER_PORT_UDP_OUT);
			createSocket(udpConnectSocketOut, hintsUDPout, resultUDPout, ipAddress, IPPROTO_UDP, DEFAULT_SERVER_PORT_UDP_IN);
		}

		void disconnectSocket(SOCKET m_socket) {
			int iResult = shutdown(m_socket, SD_SEND);
			if (iResult == SOCKET_ERROR) {
				closesocket(m_socket);
				WSACleanup();
				throw NetworkException(std::string("Socket shutdown failed: ") + intToStr(WSAGetLastError()));
			}
		}

		void makeConnectionToServer() {
			gameContext.displayMessage("Connecting to server...");
			connectToSocket(tcpConnectSocketOut, &hintsTCPout, resultTCPout);
			connectToSocket(udpConnectSocketOut, &hintsUDPout, resultUDPout);
			connectToSocket(tcpConnectSocketIn, &hintsUDPin, resultUDPin);
			connectToSocket(udpConnectSocketIn, &hintsUDPin, resultUDPin);
			clientNetworkTask = new ClientNetworkTask(tcpConnectSocketIn, udpConnectSocketIn, tcpConnectSocketOut, udpConnectSocketOut, gameContext);
			gameContext.displayMessage("Waiting for other players...");
		}

	

};