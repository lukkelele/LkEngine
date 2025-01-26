#pragma once

#include "LkEngine/Core/Event/Event.h"
#include "LkEngine/Core/Input/Keyboard.h"


namespace LkEngine {

	/**
	 * LKeyEvent
	 * 
	 *  Base key event.
	 */
	class LKeyEvent : public LEvent
	{
	protected:
		LKeyEvent(const EKey InKey) 
			: Key(InKey) 
		{
		}

	public:
		FORCEINLINE EKey GetKey() const { return Key; }

		FORCEINLINE EEventType GetType() const { return EEventType::Key; }
		FORCEINLINE const char* GetName() const override { return "Key"; }

	protected:
		EKey Key = EKey::None;
	};


	class LKeyPressedEvent : public LKeyEvent
	{
	public:
		LKeyPressedEvent(const EKey InKey, const int InRepeatCount)
			: LKeyEvent(InKey)
			, RepeatCount(InRepeatCount) 
		{
		}

		FORCEINLINE const char* GetName() const override { return "KeyPressed"; }
		FORCEINLINE EEventType GetType() const { return EEventType::KeyPressed; }

		FORCEINLINE int GetRepeatCount() const { return RepeatCount; }

		FORCEINLINE std::string ToString() const override
		{
			return std::format("{}: {} (repeated: {})", GetName(), Enum::ToString(Key), RepeatCount);
		}

	private:
		int RepeatCount = 0;
	};


	class LKeyReleasedEvent : public LKeyEvent
	{
	public:
		LKeyReleasedEvent(const EKey InKey) 
			: LKeyEvent(InKey) 
		{
		}

		FORCEINLINE const char* GetName() const override { return "KeyReleased"; }
		FORCEINLINE EEventType GetType() const { return EEventType::KeyReleased; }

		FORCEINLINE std::string ToString() const override
		{
			return std::format("{}: {}", GetName(), Enum::ToString(Key));
		}
	};

}
