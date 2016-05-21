#pragma once

class MessagePlayerAcceptedHandler : public NetworkMessageHandler {
	public:
		void execute(const std::string& message, GameContext& gameContext,
			SOCKET m_socket) {
			
		}
};

class MessagePlayerKickedHandler : public NetworkMessageHandler {
	public:
		void execute(const std::string& message, GameContext& gameContext,
			SOCKET m_socket) {

		}
};

class MessagePlayerPressedKeyHandler : public NetworkMessageHandler {
	public:
		void execute(const std::string& message, GameContext& gameContext,
			SOCKET m_socket) {

		}
};