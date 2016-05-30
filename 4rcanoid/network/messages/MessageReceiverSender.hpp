#pragma once

class MessageReceiverSender {

	private:

		

	public:
		static std::vector<std::string> getLocalIps(SOCKET socket) {
			char ac[80];
			if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR) {
				return{};
			}
			struct hostent *phe = gethostbyname(ac);
			if (phe == 0) {
				return{};
			}

			std::vector<std::string> result;

			for (int i = 0; phe->h_addr_list[i] != 0; ++i) {
				struct in_addr addr;
				memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
				result.push_back(inet_ntoa(addr));
			}
			return result;
		}


		static std::string getPeerIpTCP(SOCKET socket) {
			sockaddr client_info = { 0 };
			int addrsize = sizeof(client_info);
			getpeername(socket, &client_info, &addrsize);
			return inet_ntoa(((sockaddr_in*)&client_info)->sin_addr);
		}

		static std::string getLocalIp(SOCKET socket) {
			return getLocalIps(socket)[0];
		}

		static void sendTCPMessage(SOCKET socket, const std::string& message) {
			char buffer[BUFFER_SIZE];

			for (int i = 0; i < message.length(); i++) {
				buffer[i] = message[i];
			}
			buffer[message.length()] = 0;
			int iSendResult = send(socket, buffer, BUFFER_SIZE, 0);
			if (iSendResult == SOCKET_ERROR) {
				closesocket(socket);
				throw NetworkException("Send TCP failed: " + intToStr(WSAGetLastError()));
				WSACleanup();
			}
		}

		static void sendUDPMessage(SOCKET socket, const int& port, const std::string& ipAddress, const std::string& message) {

			struct sockaddr_in si_other;
			memset((char *)&si_other, 0, sizeof(si_other));
			si_other.sin_family = AF_INET;
			si_other.sin_port = htons(port);
			si_other.sin_addr.S_un.S_addr = inet_addr(ipAddress.c_str());

			char buffer[BUFFER_SIZE];
			for (int i = 0; i < message.length(); i++) {
				buffer[i] = message[i];
			}
			buffer[message.length()] = 0;
			int iSendResult = sendto(socket, buffer, message.length(), 0, (sockaddr*)&si_other, sizeof(si_other));
			if (iSendResult == SOCKET_ERROR) {
				closesocket(socket);
				throw NetworkException("Send UDP failed: " + intToStr(WSAGetLastError()));
				WSACleanup();
			}
		}

		static std::string createMessage(const char& messageType, const std::string& messageContent) {
			std::string result = charToOneCharString(messageType);
			result += messageContent;
			return result;
		}

		static std::string receiveTCPMessage(SOCKET socket) {
			char buffer[BUFFER_SIZE] = { 0 };
			int iResult = recv(socket, buffer, BUFFER_SIZE, 0);
			if (iResult == SOCKET_ERROR) {
				throw NetworkException("Receive TCP failed: " + intToStr(WSAGetLastError()));
			} 
			return buffer;
		}

		static std::string getIpFromSockaddr(sockaddr* info) {
			char *ip = inet_ntoa(((sockaddr_in*)info)->sin_addr);
			return ip;
		}

		static std::pair<std::string, std::string> receiveUDPMessage(SOCKET socket, const int& port, const std::string& ipAddress = "") {
			struct sockaddr_in clientInfo;
		
			int slen = sizeof(clientInfo);

			if (ipAddress != "") {
				memset((char *)&clientInfo, 0, sizeof(clientInfo));
				clientInfo.sin_family = AF_INET;
				clientInfo.sin_port = htons(port);
				clientInfo.sin_addr.S_un.S_addr = inet_addr(ipAddress.c_str());
			}
		
			char buffer[BUFFER_SIZE];
			int result = recvfrom(socket, buffer, BUFFER_SIZE, 0, (sockaddr*)&clientInfo,
				&slen);

			if (result == SOCKET_ERROR) {
				printf("DATA UDP: slen %d, buffer_size %d, port %d, address %s\n", slen, BUFFER_SIZE, port, ipAddress.c_str());
				throw NetworkException("recvfrom() failed with error code: " + intToStr(WSAGetLastError()));
			}

			return std::pair<std::string, std::string>(getIpFromSockaddr((sockaddr*)&clientInfo), buffer);
		}
};