#include "XignWriter.hpp"

XignWriter::XignWriter()
{
	this->clear();	
}

XignWriter::~XignWriter()
{
	this->clear();		
}

void XignWriter::clear()
{
	this->appends = 0;
	this->buffer.clear();
}

void XignWriter::append(unsigned char* input, std::size_t size)
{
	this->write<std::size_t>(size);
	this->write_buffer(input, size);;
}
	
std::vector<unsigned char>& XignWriter::get()
{
	return this->buffer;
}

void XignWriter::write_buffer(unsigned char* data, std::size_t size)
{
	std::copy(data, data + size, std::back_inserter(this->buffer));
}