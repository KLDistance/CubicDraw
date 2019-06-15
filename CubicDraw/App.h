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
#include "Surface.h"

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
	// play audio
	static DWORD __stdcall PlayAudio(LPVOID lpParameters);

	Window wnd;
	WndTimer timer;
	WndTimer runtimer;
	ImguiManager imgui;
	HANDLE hLoadingThread;
	HANDLE hAudioThread;
	HANDLE hAudioNotifier;

	// logo1 texture
	ID3D11ShaderResourceView *pLogo1ShaderResourceView = nullptr;
	ID3D11ShaderResourceView *pLogo2ShaderResourceView = nullptr;
	ID3D11ShaderResourceView *pLogo3ShaderResourceView = nullptr;
	
	// lottery machine
	Lottery *lottMachine = nullptr;
	// triggered strecthed imgui window
	ExpandingImGui *expandingImGui = nullptr;
	// xlsx name books
	OpenXLSX::XLSheet *xlsxSheet = nullptr;
	// name array without check
	std::vector<std::string> nameList;

	// logo state
	int logoState = 0;
	// states, 0 is preload, 1 is idle, 2 is rotating, 3 is font display
	int state = 0;
	// audio pick indicator
	int audioPick = 1;
	// temp name
	std::string tmpName;

	std::vector<std::unique_ptr<class Drawable>> drawables;
	static constexpr size_t nDrawables = 128;
	Camera cam;
	PointLight light;

	// logo timing
	const float logoSuspensionTiming = 6.0f;

	// control speed
	float speed_factor = 0.8f;
	float accelerate = 0.8f;
	const float accelerateIncrementConst = 0.035f;
	float accelerateIncrement = accelerateIncrementConst;
	const float accelerateMax = 8.00f;
	const float accelerateMin = 0.4f;
	const float ElapseMs = 1.0f;

	// transition blank timing
	const float transitionBlankTiming = 2.0f;
};

