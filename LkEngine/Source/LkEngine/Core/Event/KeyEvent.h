#pragma once

#include "LkEngine/Core/Event/Event.h"
#include "LkEngine/Core/Input/Keyboard.h"


namespace LkEngine {

	/**
	 * TODO: Refactor this. Might even remove as it is no longer used.
	 */

	class KeyEvent : public LEvent
	{
	protected:
		KeyEvent(EKey InKey) : Key(InKey) {}
	public:
		FORCEINLINE EKey GetKey() const { return Key; }

		EEventType GetEventType() const { return EEventType::Key; }
		const char* GetName() const override { return "Key"; }

	protected:
		EKey Key{};
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(EKey InKey, const int InRepeatCount)
			: KeyEvent(InKey)
			, RepeatCount(InRepeatCount) {}

		const char* GetName() const override { return "KeyPressed"; }
		EEventType GetEventType() const { return EEventType::KeyPressed; }

		FORCEINLINE int GetRepeatCount() const { return RepeatCount; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << (int)Key << " (" << RepeatCount << " repeats)";
			return ss.str();
		}

	private:
		int RepeatCount = 0;
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(EKey InKey) : KeyEvent(InKey) {}

		const char* GetName() const override { return "KeyReleased"; }
		EEventType GetEventType() const { return EEventType::KeyReleased; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << (int)Key;
			return ss.str();
		}
	};

	class KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(EKey InKey) : KeyEvent(InKey) {}

		const char* GetName() const override { return "KeyTyped"; }
		EEventType GetEventType() const { return EEventType::KeyTyped; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << (int)Key;
			return ss.str();
		}
	};

}
