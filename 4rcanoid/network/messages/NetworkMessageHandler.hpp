#pragma once

class NetworkMessageHandler {
	public:
		virtual ~NetworkMessageHandler() {}
		virtual void execute(const std::string&, GameContext&) = 0;
};

#include "MessageHandlers.hpp"
