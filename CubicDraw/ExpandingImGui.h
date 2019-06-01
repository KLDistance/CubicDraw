#pragma once
#include "Dependencies.h"
#include "imgui.h"

class ExpandingImGui
{
private:
	const float centerX = (float)DISPLAY_SCREENWIDTH / 2;
	const float centerY = (float)DISPLAY_SCREENHEIGHT / 2;
	const float strecthedWidth = (float)DISPLAY_SCREENWIDTH - 100.0f;
	const float strecthedHeight = (float)DISPLAY_SCREENHEIGHT - 100.0f;
	const float stepX = 32.0f;
	const float stepY = 22.0f;
	float presSizeX = 0.0f;
	float presSizeY = 0.0f;

	// single font size
	const float fontSize = 80.0f;
	float lastFontX = 0.0f;
	float lastFontY = 0.0f;

	// calculate the center to put fonts
	void FontCenterAlignment(UINT str_len);
public:
	ExpandingImGui();
	void TriggerCenterImGui(std::string contents, UINT isTrigger);
};