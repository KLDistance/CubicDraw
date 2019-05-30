#pragma once
#include "Dependencies.h"

class WndTimer
{
public:
	WndTimer();
	float Mark();
	float Peek() const;
private:
	std::chrono::steady_clock::time_point last;
};

