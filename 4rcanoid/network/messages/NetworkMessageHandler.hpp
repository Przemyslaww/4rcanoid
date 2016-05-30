#pragma once

class NetworkMessageHandler {
	public:
		virtual ~NetworkMessageHandler() {}
		virtual void execute(NetworkTask*, const std::string&, GameContext&) = 0;
};

#include "MessageHandlers.hpp"
