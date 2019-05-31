#pragma once
#include "Dependencies.h"
#include "Window.h"
#include "WndTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "ExpandingImGui.h"
#include "Lottery.h"
#include "OpenXLSX\OpenXLSX.h"

class App
{
public:
	App();
	int Go();

	// loading and intro
	int IntroPart();
	// lottery part
	void LotteryPart();
	// loading sub thread function
	static DWORD __stdcall LoadingThread(LPVOID lpParameters);

	Window wnd;
	WndTimer timer;
	WndTimer runtimer;
	ImguiManager imgui;
	HANDLE hLoadingThread;
	
	// lottery machine
	Lottery *lottMachine = nullptr;
	// triggered strecthed imgui window
	ExpandingImGui *expandingImGui = nullptr;
	// xlsx name books
	OpenXLSX::XLSheet *xlsxSheet = nullptr;
	// name array without check
	std::vector<std::string> nameList;

	// states, 0 is idle, 1 is rotating, 2 is font display
	int state = 0;
	std::string tmpName;

	std::vector<std::unique_ptr<class Drawable>> drawables;
	static constexpr size_t nDrawables = 128;
	Camera cam;
	PointLight light;

	// control speed
	float speed_factor = 0.8f;
	float accelerate = 1.0f;
	const float accelerateIncrementConst = 0.02f;
	float accelerateIncrement = accelerateIncrementConst;
	const float accelerateMax = 8.00f;
	const float accelerateMin = 0.4f;
	const float ElapseMs = 6.0f;

	// transition blank timing
	const float transitionBlankTiming = 6.0f;
};

