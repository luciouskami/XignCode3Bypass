#pragma once

#include <Windows.h>

#include <codecvt>
#include <vector>

class XignWriter
{
public:
	XignWriter();
	~XignWriter();

	void clear();
	void append(unsigned char* input, std::size_t size);
	
	template <typename T>
	void write(T input);

	std::vector<unsigned char>& get();

private:
	void write_buffer(unsigned char* data, std::size_t size);

	std::size_t appends;
	std::vector<unsigned char> buffer;
};

template <typename T>
void XignWriter::write(T input)
{
	unsigned char* temp_data = reinterpret_cast<unsigned char*>(&input);
	std::copy(temp_data, temp_data + sizeof(T), std::back_inserter(this->buffer));
}