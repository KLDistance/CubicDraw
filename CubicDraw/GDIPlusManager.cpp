//#define FULL_WINTARD
#include "Window.h"
#include "GDIPlusManager.h"
#include <algorithm>
namespace Gdiplus
{
	using std::min;
	using std::max;
}
#include <gdiplus.h>

#pragma comment(lib, "gdiplus.lib")

ULONG_PTR GDIPlusManager::token = 0;
int GDIPlusManager::refCount = 0;

GDIPlusManager::GDIPlusManager()
{
	if (this->refCount++ == 0)
	{
		Gdiplus::GdiplusStartupInput input;
		input.GdiplusVersion = 1;
		input.DebugEventCallback = nullptr;
		input.SuppressBackgroundThread = false;
		Gdiplus::GdiplusStartup(&token, &input, nullptr);
	}
}

GDIPlusManager::~GDIPlusManager()
{
	if (--refCount == 0)
	{
		Gdiplus::GdiplusShutdown(token);
	}
}

void GDIPlusManager::DrawImageFromFile(HWND hwnd, const wchar_t * filePath)
{
	Gdiplus::Image image(filePath);
	HDC screenDC = GetDC(hwnd);
	HDC memDC;
	Gdiplus::Graphics graphics(memDC);
	memDC = CreateCompatibleDC(screenDC);
	graphics.DrawImage(&image, 0, 0, DISPLAY_SCREENWIDTH, DISPLAY_SCREENHEIGHT);
	HBITMAP memBitmap = CreateCompatibleBitmap(
		screenDC, DISPLAY_SCREENWIDTH, DISPLAY_SCREENHEIGHT
	);
	SelectObject(memDC, memBitmap);
	BitBlt(screenDC, 0, 0, DISPLAY_SCREENWIDTH, DISPLAY_SCREENHEIGHT, memDC, 0, 0, SRCCOPY);
	UpdateWindow(hwnd);
}
