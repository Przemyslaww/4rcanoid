#pragma once

class NetworkMessageHandler {
	public:
		virtual ~NetworkMessageHandler() {}
		virtual void execute(const std::string&, GameContext&, SOCKET) = 0;
};

#include "MessageHandlers.hpp"
