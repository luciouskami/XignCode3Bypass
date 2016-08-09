#pragma once

#include <Windows.h>

#include <iostream>
#include <memory>

namespace network
{
	class session
	{
	public:
		static session& get_instance()
		{
			static session instance;
			return instance;
		}
		
		void close();
		void reset(SOCKET desc, sockaddr_in const& sender);

		bool handler();
		
		int e_read(unsigned char* buffer, std::size_t assumed_size = 0);
		bool e_write(unsigned char const* buffer, std::size_t size);

	private:
		session();
		~session();

		int raw_read(unsigned char* buffer, std::size_t size);
		bool raw_write(unsigned char const* buffer, std::size_t size);
		
		SOCKET desc;
		sockaddr_in sender;
	};
}