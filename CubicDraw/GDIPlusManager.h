#pragma once
#include "Window.h"

class GDIPlusManager
{
public:
	GDIPlusManager();
	~GDIPlusManager();
	void DrawImageFromFile(HWND hwnd, const wchar_t *filePath);
private:
	static ULONG_PTR token;
	static int refCount;
};