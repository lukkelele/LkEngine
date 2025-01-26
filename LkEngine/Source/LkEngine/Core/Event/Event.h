#pragma once

#include "LkEngine/Core/Core.h"

#include <sstream>


namespace LkEngine {

	enum class EEventType
	{
		Null = 0,
		Key, KeyPressed, KeyReleased,
		MouseButtonPressed, MouseButtonReleased, MouseButtonDown, MouseMoved, MouseScrolled,
		WindowFocus, WindowLostFocus, WindowMoved, WindowResize, WindowClose,

		ConstraintAdded, ConstraintRemoved, ConstraintAltered,
		RigidbodyAdded, RigidbodyRemoved, RigidbodyAltered,
		Collision, Separation
	};

	class LEvent
	{
	public:
		virtual ~LEvent() = default;

		virtual EEventType GetType() const = 0;
		virtual const char* GetName() const = 0;

		virtual std::string ToString() const { return GetName(); }

	public:
		bool bHandled = false;
	};

	class LEventDispatcher
	{
		template<typename T>
		using TEventFn = std::function<bool(T&)>;
	public:
		LEventDispatcher(LEvent& InEvent) 
			: EventRef(InEvent) 
		{
		}

		template<typename T>
		bool Dispatch(TEventFn<T> InFunc)
		{
			if (!EventRef.bHandled)
			{
				EventRef.bHandled = InFunc(*(T*)&EventRef);
				LK_CORE_TRACE_TAG("EventDispatcher", "Executing: '{}'", EventRef.GetName());
				return true;
			}
			return false;
		}

	private:
		LEvent& EventRef;
	};

	inline std::ostream& operator<<(std::ostream& OStream, const LEvent& InEvent)
	{
		return OStream << InEvent.ToString();
	}

	using FEventCallback = std::function<void(LEvent&)>;

}
