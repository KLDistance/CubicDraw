#include "Mouse.h"

std::pair<int, int> Mouse::GetPos() const noexcept
{
	return { this->x, this->y };
}

int Mouse::GetPosX() const noexcept
{
	return this->x;
}

int Mouse::GetPosY() const noexcept
{
	return this->y;
}

BOOL Mouse::IsInWindow() const noexcept
{
	return this->isInWindow;
}

BOOL Mouse::LeftisPressed() const noexcept
{
	return this->leftIsPressed;
}

BOOL Mouse::RightIsPressed() const noexcept
{
	return this->rightIsPressed;
}

Mouse::Event Mouse::Read() noexcept
{
	if (this->buffer.size() > 0u)
	{
		Mouse::Event e = this->buffer.front();
		this->buffer.pop();
		return e;
	}
	else
	{
		return Mouse::Event();
	}
}

void Mouse::Flush() noexcept
{
	this->buffer = std::queue<Event>();
}

void Mouse::OnMouseMove(int newx, int newy) noexcept
{
	this->x = newx;
	this->y = newy;
	this->buffer.push(Mouse::Event(Mouse::Event::Type::Move, *this));
}

void Mouse::OnMouseLeave() noexcept
{
	this->isInWindow = FALSE;
	this->buffer.push(Mouse::Event(Mouse::Event::Type::Leave, *this));
	this->TrimBuffer();
}

void Mouse::OnMouseEnter() noexcept
{
	this->isInWindow = TRUE;
	this->buffer.push(Mouse::Event(Mouse::Event::Type::Enter, *this));
	this->TrimBuffer();
}

void Mouse::OnLeftPressed(int x, int y) noexcept
{
	this->leftIsPressed = TRUE;
	this->buffer.push(Mouse::Event(Mouse::Event::Type::LPress, *this));
	this->TrimBuffer();
}

void Mouse::OnLeftReleased(int x, int y) noexcept
{
	this->leftIsPressed = FALSE;
	this->buffer.push(Mouse::Event(Mouse::Event::Type::LRelease, *this));
	this->TrimBuffer();
}

void Mouse::OnRightPressed(int x, int y) noexcept
{
	this->rightIsPressed = TRUE;
	this->buffer.push(Mouse::Event(Mouse::Event::Type::RPress, *this));
	this->TrimBuffer();
}

void Mouse::OnRightReleased(int x, int y) noexcept
{
	this->rightIsPressed = FALSE;
	this->buffer.push(Mouse::Event(Mouse::Event::Type::RRelease, *this));
	this->TrimBuffer();
}

void Mouse::OnWheelUp(int x, int y) noexcept
{
	this->buffer.push(Mouse::Event(Mouse::Event::Type::WheelUp, *this));
	TrimBuffer();
}

void Mouse::OnWheelDown(int x, int y) noexcept
{
	this->buffer.push(Mouse::Event(Mouse::Event::Type::WheelUp, *this));
	TrimBuffer();
}

void Mouse::OnWheelDelta(int x, int y, int delta) noexcept
{
	this->wheelDeltaCarry += delta;
	// generate events for every 120
	while (this->wheelDeltaCarry >= WHEEL_DELTA)
	{
		this->wheelDeltaCarry -= WHEEL_DELTA;
		this->OnWheelUp(x, y);
	}
	while (this->wheelDeltaCarry <= -WHEEL_DELTA)
	{
		this->wheelDeltaCarry += WHEEL_DELTA;
		this->OnWheelDown(x, y);
	}
}

void Mouse::TrimBuffer() noexcept
{
	while (this->buffer.size() > this->bufferSize)
	{
		this->buffer.pop();
	}
}