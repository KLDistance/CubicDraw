#pragma once
#include "Dependencies.h"
#include "Window.h"
#include "WndTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "ExpandingImGui.h"
#include "OpenXLSX\OpenXLSX.h"

class App
{
public:
	App();
	int Go();
	// set key state
	void SetKeyState(int input_state);
private:
	// loading and intro
	int Intro();
	// lottery part
	void Lottery();
	// loading sub thread function
	static DWORD __stdcall LoadingThread(LPVOID lpParameters);

	Window wnd;
	WndTimer timer;
	ImguiManager imgui;
	HANDLE hLoadingThread;

	// triggered strecthed imgui window
	ExpandingImGui *expandingImGui = nullptr;
	// xlsx name books
	OpenXLSX::XLSheet *xlsxSheet = nullptr;
	// name array without check
	std::vector<std::string> nameList;

	// states, 0 is idle, 1 is rotating, 2 is font display
	int state = 0;

	std::vector<std::unique_ptr<class Drawable>> drawables;
	static constexpr size_t nDrawables = 128;
	Camera cam;
	PointLight light;

	float speed_factor = 1.0f;
	float accelerate = 1.0f;
};

