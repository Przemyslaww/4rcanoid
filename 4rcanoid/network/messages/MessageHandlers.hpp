#pragma once

class MessagePlayerAcceptedHandler : public NetworkMessageHandler {
	public:
		void execute(NetworkTask* caller, const std::string& message, GameContext& gameContext);
};

class MessagePlayerListeningUdpPortHandler : public NetworkMessageHandler {
	public:
		void execute(NetworkTask* caller, const std::string& message, GameContext& gameContext);
};

class MessageGameStartHandler : public NetworkMessageHandler {
	public:
		void execute(NetworkTask* caller, const std::string& message, GameContext& gameContext);
};

class MessageGameInterruptedHandler : public NetworkMessageHandler {
public:
	void execute(NetworkTask* caller, const std::string& message, GameContext& gameContext);
};

class MessagePlayerPressedKeyHandler : public NetworkMessageHandler {
	public:
		void execute(NetworkTask* caller, const std::string& message, GameContext& gameContext);
};