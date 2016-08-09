#include <WinSock2.h>
#pragma comment(lib, "ws2_32")

#include "server.hpp"
#include "session.hpp"

#include <thread>

namespace network
{
	server::server() : sock(INVALID_SOCKET)
	{

	}

	server::~server()
	{
		if (this->sock != INVALID_SOCKET)
		{
			this->uninitialize();
		}
	}

	bool server::initialize(unsigned short port)
	{
		if (WSAStartup(MAKEWORD(2, 0), new WSADATA))
		{
			return false;
		}

		sock = socket(AF_INET, SOCK_STREAM, 0);

		if (this->sock == INVALID_SOCKET)
		{
			return false;
		}

		int opt = 1;
		if (setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&opt), sizeof(opt)) == SOCKET_ERROR)
		{
			return false;
		}

		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = INADDR_ANY;
		std::fill(addr.sin_zero, addr.sin_zero + sizeof(addr.sin_zero), 0);

		if (bind(this->sock, reinterpret_cast<const sockaddr*>(&addr), sizeof(sockaddr_in)) == SOCKET_ERROR)
		{
			return false;
		}

		if (listen(this->sock, SOMAXCONN) == SOCKET_ERROR)
		{
			return false;
		}

		return true;
	}

	bool server::uninitialize()
	{
		return (closesocket(this->sock) != SOCKET_ERROR && !WSACleanup());
	}

	void server::begin_listen()
	{
		for (;;)
		{
			sockaddr_in sender;
			int len = sizeof(sockaddr_in);

			SOCKET desc = accept(this->sock, reinterpret_cast<sockaddr*>(&sender), &len);

			if (desc == INVALID_SOCKET)
			{
				continue;
			}

			timeval timeout = { SOCKET_TIMEOUT, 0 };

			if (setsockopt(desc, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&timeout), sizeof(timeout)) != SOCKET_ERROR &&
				setsockopt(desc, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char*>(&timeout), sizeof(timeout)) != SOCKET_ERROR)
			{
				session::get_instance().close();
				session::get_instance().reset(desc, sender);
				
				if (!session::get_instance().handler())
				{
					closesocket(desc);
				}
			}
		}
	}
}