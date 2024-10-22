#pragma once

#include "LkEngine/Core/Event/Event.h"

#include "LkEngine/Input/Keyboard.h"


namespace LkEngine {

	class KeyEvent : public LEvent
	{
	protected:
		KeyEvent(EKey InKey) : Key(Key) {}
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
			: KeyEvent(Key)
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
		KeyReleasedEvent(EKey InKey)
			: KeyEvent(Key) {}

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
		KeyTypedEvent(EKey InKey)
			: KeyEvent(Key) {}

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
