#pragma once

class PortsHandler {
	public:
		static int getSocketPortNumber(SOCKET m_socket) {
			struct sockaddr_in sin;
			int sinlen = sizeof(sin);
			memset(&sin, 0, sizeof(sin));
			getsockname(m_socket, (struct sockaddr*) &sin, &sinlen);
			return ntohs(sin.sin_port);
		}

};
