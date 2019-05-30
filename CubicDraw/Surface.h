#pragma once
#include "Window.h"
#include "WinException.h"
#include <string>
#include <assert.h>
#include <memory>
#include <gdiplus.h>

class Surface
{
public:
	class Color
	{
	public:
		unsigned int dword;
		constexpr Color() noexcept : dword() {}
		constexpr Color(const Color &col) noexcept : dword(col.dword) {}
		constexpr Color(unsigned int dw) noexcept : dword(dw) {}
		constexpr Color(unsigned char x, unsigned char r, unsigned char g, unsigned char b) noexcept
			: dword((x << 24u) | (r << 16u) | (g << 8u) | b) {}
		constexpr Color(unsigned char r, unsigned char g, unsigned char b) noexcept
			: dword((r << 16u) | (g << 8u) | b) {}
		constexpr Color(Color col, unsigned char x) noexcept : dword((x << 24u) | col.dword) {}
		Color& operator=(Color color) noexcept
		{
			this->dword = color.dword;
			return *this;
		}
		constexpr unsigned char GetX() const noexcept
		{
			return this->dword >> 24u;
		}
		constexpr unsigned char GetA() const noexcept
		{
			return GetX();
		}
		constexpr unsigned char GetR() const noexcept
		{
			return (this->dword >> 16u) & 0xFFu;
		}
		constexpr unsigned char GetG() const noexcept
		{
			return (this->dword >> 8u) & 0xFFu;
		}
		constexpr unsigned char GetB() const noexcept
		{
			return this->dword & 0xFFu;
		}
		void SetX(unsigned char x) noexcept
		{
			this->dword = (this->dword & 0x00FFFFFFu) | (x << 24u);
		}
		void SetA(unsigned char a) noexcept
		{
			SetX(a);
		}
		void SetR(unsigned char r) noexcept
		{
			this->dword = (this->dword & 0xFF00FFFFu) | (r << 16u);
		}
		void SetG(unsigned char g) noexcept
		{
			this->dword = (this->dword & 0xFFFF00FFu) | (g << 8u);
		}
		void SetB(unsigned char b) noexcept
		{
			this->dword = (this->dword & 0xFFFFFF00) | b;
		}
	};
public:
	class Exception : public WinException
	{
	public: 
		Exception(int line, const char *file, std::string note) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		const std::string& GetNote() const noexcept;
	private:
		std::string note;
	};

	Surface(unsigned int width, unsigned int height) noexcept;
	Surface(Surface&& source) noexcept;
	Surface(Surface&) = delete;
	Surface& operator=(Surface &&donor) noexcept;
	Surface& operator=(const Surface&) = delete;
	~Surface() = default;
	void Clear(Color fillValue) noexcept;
	void PutPixel(unsigned int x, unsigned int y, Color c) noexcept;
	Color GetPixel(unsigned int x, unsigned int y) const noexcept;
	unsigned int GetWidth() const noexcept;
	unsigned int GetHeight() const noexcept;
	Color* GetBufferPtr() noexcept;
	const Color* GetBufferPtr() const noexcept;
	const Color* GetBufferPtrConst() const noexcept;
	static Surface FromFile(const std::string &name);
	void Save(const std::string &filename);
	void Copy(const Surface &src) noexcept;
private:
	Surface(unsigned int width, unsigned int height, std::unique_ptr<Color[]> pBufferParam) noexcept;

	std::unique_ptr<Color[]> pBuffer;
	unsigned int width;
	unsigned int height;
};