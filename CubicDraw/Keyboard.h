#pragma once
#include "Dependencies.h"

class Keyboard
{
	friend class Window;
public:
	class Event
	{
	public:
		enum class Type
		{
			Press,
			Release,
			Invalid
		};
	private:
		Type type;
		unsigned char code;
	public:
		Event() : type(Type::Invalid), code(0u) {}
		Event(Type type, unsigned char code) noexcept : type(type), code(code) {}
		BOOL IsPress() const noexcept { return type == Type::Press; }
		BOOL IsRelease() const noexcept { return type == Type::Release; }

	};
public:
	Keyboard() = default;
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;
	// key event stuff
	BOOL KeyIsPressed(unsigned char keycode) const noexcept;
	Event ReadKey() noexcept;
	BOOL KeyIsEmpty() const noexcept;
	void FlushKey() noexcept;
	// char event stuff
	char ReadChar() noexcept;
	BOOL CharIsEmpty() const noexcept;
	void FlushChar() noexcept;
	void Flush() noexcept;
	// autorepeat control
	void EnableAutorepeat() noexcept;
	void DisableAutorepeat() noexcept;
	BOOL AutorepeatIsEnabled() const noexcept;
private:
	void OnKeyPressed(unsigned char keycode) noexcept;
	void OnKeyReleased(unsigned char keycode) noexcept;
	void OnChar(char character) noexcept;
	void ClearState() noexcept;
	template<typename T> static void TrimBuffer(std::queue<T>& buffer) noexcept;
	static constexpr unsigned int nKeys = 256u;
	static constexpr unsigned int bufferSize = 16u;
	BOOL autorepeatEnabled = FALSE;
	std::bitset<nKeys> keystates;
	std::queue<Event> keybuffer;
	std::queue<char> charbuffer;
};

