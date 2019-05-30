#pragma once
#include "Dependencies.h"
#include "dxerr.h"

class WinException : public std::exception
{
public:
	WinException(int line, const char *file) noexcept;
	~WinException();

	const char* what() const noexcept override;
	virtual const char* GetType() const noexcept;
	int GetLine() const noexcept;
	const std::string& GetFile() const noexcept;
	std::string GetOriginString() const noexcept;
private:
	int line;
	std::string file;
protected:
	mutable std::string whatBuffer;
};

