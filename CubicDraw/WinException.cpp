#include "WinException.h"

WinException::WinException( int line, const char* file) noexcept
	: line(line), file(file)
{

}

WinException::~WinException()
{

}

const char* WinException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl << GetOriginString();
	this->whatBuffer = oss.str();
	return this->whatBuffer.c_str();
}

const char* WinException::GetType() const noexcept
{
	return "Windows Exception";
}

int WinException::GetLine() const noexcept
{
	return this->line;
}

const std::string& WinException::GetFile() const noexcept
{
	return this->file;
}

std::string WinException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File]" << file << std::endl << "[Line]" << line;
	return oss.str();
}