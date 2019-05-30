#include "Keyboard.h"

BOOL Keyboard::KeyIsPressed(unsigned char keycode) const noexcept
{
	return this->keystates[keycode];
}

Keyboard::Event Keyboard::ReadKey() noexcept
{
	if (this->keybuffer.size() > 0u)
	{
		Keyboard::Event e = this->keybuffer.front();
		this->keybuffer.pop();
		return e;
	}
	else
	{
		return Keyboard::Event();
	}
}

BOOL Keyboard::KeyIsEmpty() const noexcept
{
	return this->keybuffer.empty();
}

char Keyboard::ReadChar() noexcept
{
	if (this->charbuffer.size() > 0u)
	{
		unsigned char charcode = this->charbuffer.front();
		charbuffer.pop();
		return charcode;
	}
	else
	{
		return 0;
	}
}

BOOL Keyboard::CharIsEmpty() const noexcept
{
	return this->charbuffer.empty();
}

void Keyboard::FlushKey() noexcept
{
	this->keybuffer = std::queue<Event>();
}

void Keyboard::FlushChar() noexcept
{
	this->charbuffer = std::queue<char>();
}

void Keyboard::Flush() noexcept
{
	this->FlushKey();
	this->FlushChar();
}

void Keyboard::EnableAutorepeat() noexcept
{
	this->autorepeatEnabled = TRUE;
}

void Keyboard::DisableAutorepeat() noexcept
{
	this->autorepeatEnabled = FALSE;
}

BOOL Keyboard::AutorepeatIsEnabled() const noexcept
{
	return this->autorepeatEnabled;
}

void Keyboard::OnKeyPressed(unsigned char keycode) noexcept
{
	this->keystates[keycode] = TRUE;
	this->keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Press, keycode));
	Keyboard::TrimBuffer(this->keybuffer);
}

void Keyboard::OnKeyReleased(unsigned char keycode) noexcept
{
	this->keystates[keycode] = FALSE;
	this->keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Release, keycode));
	Keyboard::TrimBuffer(this->keybuffer);
}

void Keyboard::OnChar(char character) noexcept
{
	this->charbuffer.push(character);
	Keyboard::TrimBuffer(this->charbuffer);
}

void Keyboard::ClearState() noexcept
{
	this->keystates.reset();
}

template<typename T> 
void Keyboard::TrimBuffer(std::queue<T>& buffer) noexcept
{
	while (buffer.size() > bufferSize)
	{
		buffer.pop();
	}
}
