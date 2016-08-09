#include "session.hpp"

#include "XignCode.hpp"

#include "../XignCode Library/XignReader.hpp"
#include "../XignCode Library/XignWriter.hpp"

namespace network
{
	void session::close()
	{
		if (this->desc)
		{
			closesocket(this->desc);
		}
	}

	void session::reset(SOCKET desc, sockaddr_in const& sender)
	{
		this->desc = desc;
		this->sender = sender;
	}

	bool session::handler()
	{
		if (desc == INVALID_SOCKET)
		{
			return false;
		}

		try
		{
			printf("Connection received..\n");

			unsigned char data[8192];
			int size = this->e_read(data); 

			if (!size)
			{
				return false;
			}

			XignReader reader(data, size);

			unsigned char buffer[512];
			std::size_t buffer_size = 0;

			while (buffer_size = reader.fetch(buffer), buffer_size)
			{
				XignCode::_XignCode_heartbeat_callback_t callback = [](void* pointer, unsigned char* response_1, unsigned char* response_2, unsigned int size, int unknown) -> unsigned int
				{
					XignWriter writer;
					writer.write<unsigned int>(reinterpret_cast<unsigned int>(pointer));
					writer.append(response_1, size);
					writer.append(response_2, size);

					session::get_instance().e_write(writer.get().data(), writer.get().size());
					session::get_instance().close();
					return 0;
				};

				if (!XignCode::MakeResponse(buffer, buffer_size, callback))
				{
					return false;
				}
			}

			return true;
		}
		catch (std::string& exception_string)
		{
			return false;
		}

		return true;
	}
	
	int session::e_read(unsigned char* buffer, std::size_t assumed_size)
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
	
	bool session::e_write(unsigned char const* buffer, std::size_t size)
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

	session::session()
	{

	}

	session::~session()
	{

	}

	int session::raw_read(unsigned char* buffer, std::size_t size)
	{
		int bytes_read = 0;
		
		do
		{
			bytes_read = recv(this->desc, reinterpret_cast<char*>(buffer), size, 0);
		}
		while (bytes_read == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK);
		
		if (bytes_read == 0 || bytes_read == SOCKET_ERROR)
		{
			return bytes_read;
		}
		else if (bytes_read < static_cast<int>(size))
		{
			int remaining_bytes_read = 0;
			
			do
			{
				remaining_bytes_read = recv(this->desc, reinterpret_cast<char*>(buffer + bytes_read), size - bytes_read, 0);
			}
			while (remaining_bytes_read == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK);
			
			if (remaining_bytes_read == 0 || remaining_bytes_read == SOCKET_ERROR)
			{				
				return remaining_bytes_read;
			}

			bytes_read += remaining_bytes_read;
		}

		return bytes_read;
	}

	bool session::raw_write(unsigned char const* buffer, std::size_t size)
	{
		for (int data_sent = 0, offset = 0, data_to_send = size; data_to_send > 0; data_to_send -= data_sent, offset += data_sent)
		{
			data_sent = send(this->desc, reinterpret_cast<const char*>(buffer) + offset, data_to_send, 0);
	
			if (data_sent == 0 || data_sent == SOCKET_ERROR)
			{				
				return false;
			}
		}

		return true;
	}
}