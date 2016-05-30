#pragma once

class Client {

	ClientNetworkTask* clientNetworkTask;

	GameContext& gameContext;

	SOCKET tcpConnectSocketIn;
	SOCKET udpConnectSocketIn;
	SOCKET tcpConnectSocketOut;
	SOCKET udpConnectSocketOut;

	WSAData wsaData;

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

	bool clientRunning;

	int listeningUdpPort;

	std::string ipAddress;
	void createSocket(SOCKET& m_socket, addrinfo& hints, addrinfo*& result, const std::string& ipAddress, const IPPROTO& protocol, const int& port);
	void connectToSocket(SOCKET& m_socket, addrinfo* ptr, addrinfo* result);
	void bindUDPSocket(SOCKET& listenSocket, const int& port);
	public:
		Client(const std::string& m_ipAddress, GameContext& m_gameContext);
		~Client();
		std::string getServerIp();
		void initialize();
		void createTCPSockets();
		void createUDPSockets();
		void disconnectSocket(SOCKET m_socket);
		void makeConnectionToServer();
		void stop();
		bool getIsClientRunning();
		void bindSockets();
};