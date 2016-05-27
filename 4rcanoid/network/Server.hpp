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

	std::mutex mutex;
	std::mutex mutexUDP;

	int players;

	bool serverRunning;

	int networkTasksLoopCounter;

	std::vector<ServerNetworkTask*> networkTasks;

	ThreadSafeQueue<ServerNetworkTask*> tasksToRemove;

	void createSocket(SOCKET& m_socket, addrinfo& hints, addrinfo*& result, const IPPROTO& protocol, const int& port);
	void createTCPSockets();
	void createUDPSockets();
	void bindSocket(SOCKET listenSocket, addrinfo* result);
	void listenOnSocket(SOCKET listenSocket);
	public:
		Server(GameContext& m_gameContext);
		~Server();
		int initialize();
		std::string getServerIp();
		void createServerSockets();
		void bindSockets();
		void stop();
		void closeSocket(SOCKET clientSocket);
		void listenOnSockets();
		void disconnectSocket(SOCKET clientSocket);
		void broadcastMessage(const IPPROTO& protocol, const std::string& message, const char& playerMessageIncomingNumber);
		void run();
		void listenUDP();
		void goBackToLobby();
};