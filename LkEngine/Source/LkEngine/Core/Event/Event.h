#pragma once

#include "LkEngine/Core/Core.h"

#include <sstream>


namespace LkEngine {

	enum class EEventType
	{
		Null = 0,
		Key, KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseButtonDown, MouseMoved, MouseScrolled,
		WindowFocus, WindowLostFocus, WindowMoved, WindowResize, WindowClose,

		TextureCreated, TextureDeleted,	

		SceneCreated, SceneDeleted, 

		// Physics
		ConstraintAdded, ConstraintRemoved, ConstraintAltered,
		RigidbodyAdded, RigidbodyRemoved, RigidbodyAltered,
		Collision, Separation
	};

	class LEvent
	{
	public:
		virtual ~LEvent() {}

		bool Handled = false;

		virtual EEventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;

		virtual std::string ToString() const { return GetName(); }
	};

	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(LEvent& Event) : m_Event(Event) 
		{
		}

		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			if (!m_Event.Handled)
			{
				m_Event.Handled = func(*(T*)&m_Event);
				LK_CORE_TRACE_TAG("EventDispatcher", "Executing event \"{}\"", m_Event.GetName());
				return true;
			}
			return false;
		}

	private:
		LEvent& m_Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const LEvent& e)
	{
		return os << e.ToString();
	}

	using FEventCallback = std::function<void(LEvent&)>;

}
