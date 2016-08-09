#include "XignReader.hpp"

XignReader::XignReader(unsigned char* input, std::size_t size) 
	: index(0), buffer(input, input + size)
{

}

XignReader::~XignReader()
{

}

std::size_t XignReader::fetch(unsigned char* output)
{
	std::size_t size = this->read<std::size_t>();

	if (!size)
	{
		return 0;
	}

	if (!this->read_buffer(output, size))
	{
		return 0;
	}

	return size;
}

	
std::vector<unsigned char>& XignReader::get()
{
	return this->buffer;
}
	
bool XignReader::read_buffer(unsigned char* buffer, std::size_t size)
{
	if (!this->check_index(size))
	{
		return false;
	}
		
	memcpy(buffer, this->buffer.data() + this->index, size);
	this->index += size;
	return true;
}

bool XignReader::check_index(std::size_t nSize)
{
	return ((this->index + nSize) <= static_cast<std::size_t>(this->buffer.size()));
}