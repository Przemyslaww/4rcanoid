#include "../header.hpp"

void Client::createSocket(SOCKET& m_socket, addrinfo& hints, addrinfo*& result, const std::string& ipAddress, const IPPROTO& protocol, const int& port) {
	if (protocol == IPPROTO_TCP) {
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = protocol == IPPROTO_TCP ? SOCK_STREAM : SOCK_DGRAM;
		hints.ai_protocol = protocol;

		int iResult = getaddrinfo(ipAddress.c_str(), intToStr(port).c_str(), &hints, &result);
		if (iResult != 0) {
			throw NetworkException(std::string("getaddrinfo failed: ") + intToStr(iResult));
			WSACleanup();
		}
		m_socket = INVALID_SOCKET;
		m_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (m_socket == INVALID_SOCKET || m_socket == SOCKET_ERROR) {
			freeaddrinfo(result);
			throw NetworkException(std::string("Error at socket TCP: ") + intToStr(WSAGetLastError()));
			WSACleanup();
		}
	}
	else {
		m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (m_socket == SOCKET_ERROR || m_socket == INVALID_SOCKET) {
			throw NetworkException(std::string("Error at socket UDP: ") + intToStr(WSAGetLastError()));
		}
	}
		

}

void Client::bindUDPSocket(SOCKET& listenSocket, const int& port) {
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);
	int Result = bind(listenSocket, (SOCKADDR *)& server, sizeof(server));
	if (Result == SOCKET_ERROR) {
		throw NetworkException(std::string("bind UDP failed with error: ") + intToStr(WSAGetLastError()));
	}
}

void Client::bindSockets() {
	bindUDPSocket(udpConnectSocketIn, 0);
	listeningUdpPort = PortsHandler::getSocketPortNumber(udpConnectSocketIn);
	bindUDPSocket(udpConnectSocketOut, 0);
}

void Client::connectToSocket(SOCKET& m_socket, addrinfo* ptr, addrinfo* result) {
	int iResult = connect(m_socket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		throw NetworkException("Unable to connect to server! Error: " + intToStr(WSAGetLastError()));
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
	freeaddrinfo(result);

	if (m_socket == INVALID_SOCKET) {
		throw NetworkException("Unable to connect to server! Error: " + intToStr(WSAGetLastError()));
		WSACleanup();
	}
}


		Client::Client(const std::string& m_ipAddress, GameContext& m_gameContext) : gameContext(m_gameContext) {
			clientNetworkTask = NULL;
			ipAddress = m_ipAddress;
			clientRunning = true;
			listeningUdpPort = -1;
		}

		Client::~Client() {
			if (clientNetworkTask) delete clientNetworkTask;
		}

		void Client::createTCPSockets() {
			createSocket(tcpConnectSocketIn, hintsTCPin, resultTCPin, ipAddress, IPPROTO_TCP, DEFAULT_SERVER_PORT_TCP_OUT);
			createSocket(tcpConnectSocketOut, hintsTCPout, resultTCPout, ipAddress, IPPROTO_TCP, DEFAULT_SERVER_PORT_TCP_IN);
		}

		void Client::createUDPSockets() {
			createSocket(udpConnectSocketIn, hintsUDPin, resultUDPin, ipAddress, IPPROTO_UDP, DEFAULT_SERVER_PORT_UDP_OUT);
			createSocket(udpConnectSocketOut, hintsUDPout, resultUDPout, ipAddress, IPPROTO_UDP, DEFAULT_SERVER_PORT_UDP_IN);
		}

		void Client::disconnectSocket(SOCKET m_socket) {
			int iResult = shutdown(m_socket, SD_SEND);
			if (iResult == SOCKET_ERROR) {
				closesocket(m_socket);
				throw NetworkException(std::string("Socket shutdown failed: ") + intToStr(WSAGetLastError()));
				WSACleanup();
			}
		}

		void Client::makeConnectionToServer() {
			gameContext.setTextInPlayersBox("Connecting to server...");
			try {
				connectToSocket(tcpConnectSocketOut, &hintsTCPout, resultTCPout);
				connectToSocket(tcpConnectSocketIn, &hintsTCPin, resultTCPin);
				clientNetworkTask = new ClientNetworkTask(tcpConnectSocketIn, udpConnectSocketIn, tcpConnectSocketOut, udpConnectSocketOut, gameContext, listeningUdpPort, this);
				clientNetworkTask->run();
			}
			catch (NetworkException& e) {
				gameContext.setTextInPlayersBox("Unable to connect to server!\nPlease restart an application and try again.\n\n" +
					e.getMessage());
				WSACleanup();
			}
			
		}

		void Client::initialize() {
			int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
			if (iResult != 0) {
				throw NetworkException("WSAStartup failed with error: " + intToStr(iResult));
			}
		}

		void Client::stop() {
			closesocket(udpConnectSocketIn);
			closesocket(udpConnectSocketOut);
			closesocket(tcpConnectSocketIn);
			closesocket(tcpConnectSocketOut);
			clientRunning = false;
			if (clientNetworkTask) clientNetworkTask->join();
			printf("Client stopped\n");
		}

		bool Client::getIsClientRunning() {
			return clientRunning;
		}

		std::string Client::getServerIp() {
			return ipAddress;
		}