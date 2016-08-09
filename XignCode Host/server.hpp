#pragma once

#include <Windows.h>

namespace network
{
	#define SOCKET_TIMEOUT 5

	class server
	{
	public:
		server();
		~server();

		bool initialize(unsigned short port);
		bool uninitialize();

		void begin_listen();

	private:
		SOCKET sock;
	};
}