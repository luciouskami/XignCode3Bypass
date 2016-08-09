#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#include "client.hpp"

#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32")

#include <string>

namespace network
{
	client::client(unsigned short port) 
		: sock(INVALID_SOCKET), port(port)
	{

	}

	client::~client()
	{
		if (this->sock != INVALID_SOCKET)
		{
			this->uninitialize();
		}
	}

	bool client::make_interaction(interaction_handler finalize_handler)
	{
		if (!this->initialize())
		{
			this->output_wsa_error(error_type::connect);
			return false;
		}
		
		bool result = finalize_handler(this);

		this->uninitialize();
		return result;
	}
	
	int client::read(unsigned char* buffer, std::size_t assumed_size)
	{
		unsigned short data_length = 0;
		int data_length_size = this->raw_read(reinterpret_cast<unsigned char*>(&data_length), sizeof(unsigned short));

		if (!data_length_size || data_length_size != sizeof(unsigned short))
		{
			return 0;	
		}
		
		if (assumed_size && data_length != assumed_size)
		{
			return 0;
		}
		
		unsigned char data_buffer[16384];
		
		for (int data_read = 0, offset = 0, data_to_read = data_length; data_to_read > 0; data_to_read -= data_read, offset += data_read)
		{
			data_read = this->raw_read(data_buffer + offset, data_to_read);

			if (!data_read || data_read == SOCKET_ERROR)
			{
				return 0;
			}
		}

		memcpy(buffer, data_buffer, data_length);
		return data_length;
	}
	
	bool client::write(unsigned char const* buffer, std::size_t size)
	{
		unsigned short data_length = static_cast<unsigned short>(size);

		if (!this->raw_write(reinterpret_cast<unsigned char*>(&data_length), sizeof(unsigned short)))
		{
			return false;
		}

		unsigned char* data_buffer = new unsigned char[size];
		memcpy(data_buffer, buffer, size);

		if (!this->raw_write(data_buffer, size))
		{
			delete[] data_buffer;
			return false;
		}

		delete[] data_buffer;
		return true;
	}

	bool client::initialize()
	{
		if (WSAStartup(MAKEWORD(2, 0), new WSADATA))
		{
			return false;
		}

		this->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (this->sock == INVALID_SOCKET)
		{
			return false;
		}

		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(this->port);
		addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // inet_addr("127.0.0.1");
		std::fill(addr.sin_zero, addr.sin_zero + sizeof(addr.sin_zero), 0);

		return (connect(this->sock, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_in)) != SOCKET_ERROR);
	}

	bool client::uninitialize()
	{
		return (shutdown(this->sock, SD_BOTH) != SOCKET_ERROR && closesocket(this->sock) != SOCKET_ERROR && WSACleanup() == 0);
	}

	int client::raw_read(unsigned char* buffer, std::size_t size)
	{
		int bytes_read = recv(this->sock, reinterpret_cast<char*>(buffer), size, 0);
		
		if (bytes_read == 0 || bytes_read == SOCKET_ERROR)
		{
			if (bytes_read == SOCKET_ERROR)
			{
				this->output_wsa_error(error_type::read);
			}

			return bytes_read;
		}
		else if (bytes_read < static_cast<int>(size))
		{
			int remaining_bytes_read = recv(this->sock, reinterpret_cast<char*>(buffer + bytes_read), size - bytes_read, 0);
		
			if (remaining_bytes_read == 0 || remaining_bytes_read == SOCKET_ERROR)
			{
				if (remaining_bytes_read == SOCKET_ERROR)
				{
					this->output_wsa_error(error_type::read, false);
				}

				return remaining_bytes_read;
			}

			bytes_read += remaining_bytes_read;
		}

		return bytes_read;
	}

	bool client::raw_write(unsigned char const* buffer, std::size_t size)
	{
		for (int data_sent = 0, offset = 0, data_to_send = size; data_to_send > 0; data_to_send -= data_sent, offset += data_sent)
		{
			data_sent = send(this->sock, reinterpret_cast<const char*>(buffer) + offset, data_to_send, 0);
	
			if (data_sent == 0 || data_sent == SOCKET_ERROR)
			{
				if (data_sent == SOCKET_ERROR)
				{
					this->output_wsa_error(error_type::write);
				}

				return false;
			}
		}

		return true;
	}
	
	void client::output_wsa_error(error_type type, bool first)
	{
		int error_code = WSAGetLastError();

		char error_message[1024];
		memset(error_message, 0, sizeof(error_message));

		strcpy(error_message, "Failed to ");
		
		switch (type)
		{
		case error_type::connect:
			strcat(error_message, "connect to ");
			break;

		case error_type::read:
			strcat(error_message, (first ? "receive (0) data from " : "receive (1) data from "));
			break;

		case error_type::write:
			strcat(error_message, "send data to ");
			break;
		}

		strcat(error_message, "the server.\nReason: ");

		switch (error_code)
		{
		case WSAECONNRESET:
			strcat(error_message, "You've been disconnected; the server terminated the connection.");
			break;

		case WSAETIMEDOUT:
			strcat(error_message, "You've timed out; it took you more than 10 seconds to communicate with the server.");
			break;

		default:
			strcat(error_message, "Unknown. Error code: ");
			strcat(error_message, std::to_string(error_code).c_str());
			strcat(error_message, ".");
			break;
		}
		
		printf("Client networking error: %s\n", error_message);
	}
}