#pragma once
#include "Dependencies.h"
#include "Window.h"
#include "WndTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"

class App
{
public:
	App();
	int Go();
private:
	void DoFrame();
private:
	Window wnd;
	WndTimer timer;
	ImguiManager imgui;

	std::vector<std::unique_ptr<class Drawable>> drawables;
	static constexpr size_t nDrawables = 128;
	Camera cam;
	PointLight light;

	float speed_factor = 1.0f;
};

