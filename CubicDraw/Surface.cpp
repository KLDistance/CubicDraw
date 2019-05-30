#define FULL_WINTARD
#include "Surface.h"
#include <algorithm>
namespace Gdiplus
{
	using std::min;
	using std::max;
}
#include <sstream>

#pragma comment(lib, "gdiplus.lib")

Surface::Surface(unsigned int width, unsigned int height) noexcept
	:
	pBuffer(std::make_unique<Color[]>(width * height)),
	width(width),
	height(height)
{}

Surface::Surface(Surface && source) noexcept
	:
	pBuffer(std::move(source.pBuffer)),
	width(source.width),
	height(source.height)
{}

Surface & Surface::operator=(Surface && donor) noexcept
{
	this->width = donor.width;
	this->height = donor.height;
	this->pBuffer = std::move(donor.pBuffer);
	donor.pBuffer = nullptr;
	return *this;
}

void Surface::Clear(Color fillValue) noexcept
{
	memset(pBuffer.get(), fillValue.dword, width * height * sizeof(Color));
}

void Surface::PutPixel(unsigned int x, unsigned int y, Color c) noexcept
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < this->width);
	assert(y < this->height);
	this->pBuffer[y * this->width + x] = c;
}

Surface::Color Surface::GetPixel(unsigned int x, unsigned int y) const noexcept
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < this->width);
	assert(y < this->height);
	return this->pBuffer[y * this->width + x];
}

unsigned int Surface::GetWidth() const noexcept
{
	return this->width;
}

unsigned int Surface::GetHeight() const noexcept
{
	return this->height;
}

Surface::Color * Surface::GetBufferPtr() noexcept
{
	return this->pBuffer.get();
}

const Surface::Color* Surface::GetBufferPtr() const noexcept
{
	return this->pBuffer.get();
}

const Surface::Color * Surface::GetBufferPtrConst() const noexcept
{
	return this->pBuffer.get();
}

Surface Surface::FromFile(const std::string & name)
{
	unsigned int width = 0;
	unsigned int height = 0;
	std::unique_ptr<Color[]> pBuffer;
	// convert filename to wide string for gdi+
	wchar_t wideName[512];
	mbstowcs_s(nullptr, wideName, name.c_str(), _TRUNCATE);

	Gdiplus::Bitmap bitmap(wideName);
	if (bitmap.GetLastStatus() != Gdiplus::Status::Ok)
	{
		std::stringstream ss;
		ss << "Loading image [" << name << "]: failed to load.";
		throw Exception(__LINE__, __FILE__, ss.str());
	}
	
	width = bitmap.GetWidth();
	height = bitmap.GetHeight();
	pBuffer = std::make_unique<Color[]>(width * height);

	for (unsigned int y = 0; y < height; y++)
	{
		for (unsigned int x = 0; x < width; x++)
		{
			Gdiplus::Color c;
			bitmap.GetPixel(x, y, &c);
		}
	}
	return Surface(width, height, std::move(pBuffer));
}

void Surface::Save(const std::string & filename)
{
	auto GetEncoderClsid = [&filename](const WCHAR *format, CLSID *pClsid) -> void
	{
		UINT num = 0;		// number of image encoders
		UINT size = 0;		// size of the image encoder array in bytes

		Gdiplus::ImageCodecInfo *pImageCodecInfo = nullptr;
		Gdiplus::GetImageEncodersSize(&num, &size);
		if (!size)
		{
			std::stringstream ss;
			ss << "Saving surface to [" << filename << "]: failed to get encoder; size == 0.";
			throw Exception(__LINE__, __FILE__, ss.str());
		}

		pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
		if (!pImageCodecInfo)
		{
			std::stringstream ss;
			ss << "Saving surface to [" << filename << "]: failed to get encoder; failed to allocate memory.";
			throw Exception(__LINE__, __FILE__, ss.str());
		}
		Gdiplus::DllExports::GdipGetImageEncoders(num, size, pImageCodecInfo);
		for (UINT j = 0; j < num; j++)
		{
			if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
			{
				*pClsid = pImageCodecInfo[j].Clsid;
				free(pImageCodecInfo);
				return;
			}
		}

		free(pImageCodecInfo);
		std::stringstream ss;
		ss << "Saving surface to [" << filename << "]: failed to get encoder; failed to find matching encoder.";
		throw Exception(__LINE__, __FILE__, ss.str());
	};

	CLSID bmpID;
	GetEncoderClsid(L"image/bmp", &bmpID);

	// convert filename to wide string for gdi+
	wchar_t wideName[512];
	mbstowcs_s(nullptr, wideName, filename.c_str(), _TRUNCATE);

	Gdiplus::Bitmap bitmap(width, height, width * sizeof(Color), PixelFormat32bppARGB, (BYTE*)pBuffer.get());
	if (bitmap.Save(wideName, &bmpID, nullptr) != Gdiplus::Status::Ok)
	{
		std::stringstream ss;
		ss << "Saving surface to [" << filename << "]: failed to save.";
		throw Exception(__LINE__, __FILE__, ss.str());
	}
}

void Surface::Copy(const Surface & src) noexcept
{
	assert(this->width == src.width);
	assert(this->height == src.height);
	memcpy(pBuffer.get(), src.pBuffer.get(), this->width * this->height * sizeof(Color));
}

Surface::Surface(unsigned int width, unsigned int height, std::unique_ptr<Color[]> pBufferParam) noexcept
	: width(width), height(height), pBuffer(std::move(pBufferParam))
{}

Surface::Exception::Exception(int line, const char * file, std::string note) noexcept
	: WinException(line, file), note(std::move(note))
{}

const char* Surface::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << WinException::what() << std::endl <<
		"[note] " << GetNote();
	this->whatBuffer = oss.str();
	return this->whatBuffer.c_str();
}

const char* Surface::Exception::GetType() const noexcept
{
	return "KLDistance Graphics Exception";
}

const std::string & Surface::Exception::GetNote() const noexcept
{
	return this->note;
}
