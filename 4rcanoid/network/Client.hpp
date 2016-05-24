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
	void createSocket(SOCKET& m_socket, addrinfo& hints, addrinfo*& result, const std::string& ipAddress, const IPPROTO& protocol, const int& port);
	void connectToSocket(SOCKET& m_socket, addrinfo* ptr, addrinfo* result);

	public:
		Client(const std::string& m_ipAddress, GameContext& m_gameContext);
		~Client();
		void createTCPSockets();
		void createUDPSockets();
		void disconnectSocket(SOCKET m_socket);
		void makeConnectionToServer();
};