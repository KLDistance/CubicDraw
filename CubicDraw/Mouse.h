#pragma once
#include "Dependencies.h"

class Mouse
{
	friend class Window;
public:
	class Event
	{
	public:
		enum class Type
		{
			LPress, LRelease, RPress, RRelease,
			WheelUp, WheelDown,
			Move, Enter, Leave,
			Invalid
		};
	private:
		Type type;
		BOOL leftIsPressed, rightIsPressed;
		int x, y;
	public:
		Event() noexcept : type(Type::Invalid), leftIsPressed(FALSE), rightIsPressed(FALSE), x(0), y(0) {}
		Event(Type type, const Mouse& parent) noexcept :
			type(type), leftIsPressed(parent.leftIsPressed), rightIsPressed(parent.rightIsPressed), x(parent.x), y(parent.y) {}
		BOOL IsValid() const noexcept { return type != Type::Invalid; }
		Type GetType() const noexcept { return type; }
		std::pair<int, int> GetPos() const noexcept { return{ this->x,this->y }; }
		int GetPosX() const noexcept { return this->x; }
		int GetPosY() const noexcept { return this->y; }
		BOOL LeftIsPressed() const noexcept { return this->leftIsPressed; }
		BOOL RightisPressed() const noexcept { return this->rightIsPressed; }
	};

	Mouse() = default;
	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;
	std::pair<int, int> GetPos() const noexcept;
	int GetPosX() const noexcept;
	int GetPosY() const noexcept;
	BOOL IsInWindow() const noexcept;
	BOOL LeftisPressed() const noexcept;
	BOOL RightIsPressed() const noexcept;
	Mouse::Event Read() noexcept;
	BOOL IsEmpty() const noexcept { return buffer.empty(); }
	void Flush() noexcept;

private:
	void OnMouseMove(int x, int y) noexcept;
	void OnMouseLeave() noexcept;
	void OnMouseEnter() noexcept;
	void OnLeftPressed(int x, int y) noexcept;
	void OnLeftReleased(int x, int y) noexcept;
	void OnRightPressed(int x, int y) noexcept;
	void OnRightReleased(int x, int y) noexcept;
	void OnWheelUp(int x, int y) noexcept;
	void OnWheelDown(int x, int y) noexcept;
	void OnWheelDelta(int x, int y, int delta) noexcept;
	void TrimBuffer() noexcept;

	static constexpr unsigned int bufferSize = 16u;
	int x, y;
	BOOL leftIsPressed = FALSE, rightIsPressed = FALSE, isInWindow = FALSE;
	int wheelDeltaCarry = 0;
	std::queue<Event> buffer;
};

