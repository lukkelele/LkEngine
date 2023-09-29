#pragma once
#include "LkEngine/Core/Base.h"


namespace LkEngine {

	enum class EventType
	{
		Null = 0,
		WindowFocus, WindowLostFocus, WindowMoved, WindowResize, WindowClose,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
		// Physics
		ConstraintAdded, ConstraintRemoved, ConstraintAltered,
		RigidbodyAdded, RigidbodyRemoved, RigidbodyAltered,
		Collision, Separation
	};

	class Event
	{
	public:
		virtual ~Event() = default;

		bool Handled = false;

		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual std::string ToString() const { return GetName(); }
		virtual bool HandleEvent() = 0;
	};
}
