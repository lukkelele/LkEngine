#pragma once

#include "LkEngine/Core/Event/Event.h"

#include "LkEngine/Input/Keyboard.h"


namespace LkEngine {

	class KeyEvent : public LEvent
	{
	protected:
		KeyEvent(KeyCode keycode) 
			: m_KeyCode(keycode) {}

	public:
		inline KeyCode GetKeyCode() const { return m_KeyCode; }

		EEventType GetEventType() const { return EEventType::Key; }
		const char* GetName() const override { return "Key"; }

	protected:
		KeyCode m_KeyCode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(KeyCode keycode, int repeatCount)
			: KeyEvent(keycode)
			, m_RepeatCount(repeatCount) {}

		inline int GetRepeatCount() const { return m_RepeatCount; }

		const char* GetName() const override { return "KeyPressed"; }
		EEventType GetEventType() const { return EEventType::KeyPressed; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << (int)m_KeyCode << " (" << m_RepeatCount << " repeats)";
			return ss.str();
		}

	private:
		int m_RepeatCount;
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(KeyCode keycode)
			: KeyEvent(keycode) {}

		const char* GetName() const override { return "KeyReleased"; }
		EEventType GetEventType() const { return EEventType::KeyReleased; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << (int)m_KeyCode;
			return ss.str();
		}
	};

	class KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(KeyCode keycode)
			: KeyEvent(keycode) {}

		const char* GetName() const override { return "KeyTyped"; }
		EEventType GetEventType() const { return EEventType::KeyTyped; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << (int)m_KeyCode;
			return ss.str();
		}
	};

}
