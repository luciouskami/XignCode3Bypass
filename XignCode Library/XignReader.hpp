#pragma once

#include <Windows.h>

#include <codecvt>
#include <vector>

class XignReader
{
public:
	XignReader(unsigned char* input, std::size_t size);
	~XignReader();

	std::size_t fetch(unsigned char* output);

	template <typename T>
	T read();
	
	std::vector<unsigned char>& get();
	
private:
	bool read_buffer(unsigned char* buffer, std::size_t size);

	bool check_index(std::size_t nSize);

	std::size_t index;
	std::vector<unsigned char> buffer;
};

template <typename T>
T XignReader::read()
{
	if (this->check_index(sizeof(T)))
	{
		this->index += sizeof(T);
		return *reinterpret_cast<T*>(this->buffer.data() + this->index - sizeof(T));
	}

	return static_cast<T>(0);
}