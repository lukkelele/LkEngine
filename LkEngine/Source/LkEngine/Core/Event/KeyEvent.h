#pragma once

#include "LkEngine/Core/Event/Event.h"
#include "LkEngine/Core/Input/Keyboard.h"


namespace LkEngine {

	/**
	 * LKeyPressedEvent
	 */
	class LKeyPressedEvent : public LEvent
	{
	public:
		LKeyPressedEvent() = delete;
		LKeyPressedEvent(const EKey InKey, const int InRepeatCount)
			: Key(InKey)
			, RepeatCount(InRepeatCount) 
		{
		}

		FORCEINLINE EKey GetKey() const { return Key; }
		FORCEINLINE int GetRepeatCount() const { return RepeatCount; }

		FORCEINLINE std::string ToString() const override
		{
			return std::format("{}: {} (repeat: {})", GetName(), Enum::ToString(Key), RepeatCount);
		}

	private:
		EKey Key = EKey::None;
		int RepeatCount = 0;

		LEVENT(KeyPressed);
	};

	/**
	 * LKeyReleasedEvent
	 */
	class LKeyReleasedEvent : public LEvent
	{
	public:
		LKeyReleasedEvent() = delete;
		LKeyReleasedEvent(const EKey InKey) : Key(InKey) {}

		FORCEINLINE std::string ToString() const override
		{
			return std::format("{}: {}", GetName(), Enum::ToString(Key));
		}

	private:
		EKey Key = EKey::None;

		LEVENT(KeyReleased);
	};

}
