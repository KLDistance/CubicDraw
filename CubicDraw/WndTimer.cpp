#include "WndTimer.h"

WndTimer::WndTimer()
{
	this->last = std::chrono::steady_clock::now();
}

float WndTimer::Mark()
{
	const auto old = this->last;
	this->last = std::chrono::steady_clock::now();
	const std::chrono::duration<float> frameTime = last - old;
	return frameTime.count();
}

float WndTimer::Peek() const
{
	return std::chrono::duration<float>(std::chrono::steady_clock::now() - this->last).count();
}