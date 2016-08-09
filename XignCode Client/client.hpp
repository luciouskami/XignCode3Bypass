#pragma once

#include <WinSock2.h>
#include <Windows.h>

#include <functional>
#include <iostream>

namespace network
{
	enum class error_type
	{
		connect,
		read,
		write
	};

	class client
	{
	protected:
		typedef std::function<bool(client*)> interaction_handler;

	public:
		client(unsigned short port);
		~client();
		
		bool make_interaction(interaction_handler finalize_handler);

		int read(unsigned char* buffer, std::size_t assumed_size = 0);
		bool write(unsigned char const* buffer, std::size_t size);

	protected:
		bool initialize();
		bool uninitialize();

		int raw_read(unsigned char* buffer, std::size_t size);
		bool raw_write(unsigned char const* buffer, std::size_t size);

		void output_wsa_error(error_type type, bool first = true);

	private:
		SOCKET sock;

		std::string hostname;
		unsigned short port;
	};
}