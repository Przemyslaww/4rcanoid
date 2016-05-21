#pragma once

class MessageReceiverSender {
	public:
		static std::string getPeerIp(SOCKET socket) {
			sockaddr client_info = { 0 };
			int addrsize = sizeof(client_info);
			getpeername(socket, &client_info, &addrsize);
			return inet_ntoa(((sockaddr_in*)&client_info)->sin_addr);
		}

		static std::string getLocalIp(SOCKET socket) {
			sockaddr client_info = { 0 };
			int addrsize = sizeof(client_info);
			getsockname(socket, &client_info, &addrsize);
			return inet_ntoa(((sockaddr_in*)&client_info)->sin_addr);
		}

		static void sendMessage(SOCKET socket, char messageType, const std::string& message) {
			char buffer[BUFFER_SIZE];
			buffer[0] = messageType;
			for (int i = 0; i < message.length(); i++) {
				buffer[i + 1] = message[i];
			}
			buffer[message.length() + 1] = 0;
			int iSendResult = send(socket, buffer, message.length(), 0);
			if (iSendResult == SOCKET_ERROR) {
				closesocket(socket);
				WSACleanup();
				throw NetworkException("Send failed: " + intToStr(WSAGetLastError()));
			}
		}

		static std::string receiveMessage(SOCKET socket) {
			char buffer[BUFFER_SIZE] = { 0 };
			int iResult = recv(socket, buffer, BUFFER_SIZE, 0);
			return buffer;
		}
};