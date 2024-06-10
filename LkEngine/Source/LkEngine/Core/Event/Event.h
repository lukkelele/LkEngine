#pragma once

#include "LkEngine/Core/Base.h"

#include <sstream>


namespace LkEngine {

	enum class EventType
	{
		Null = 0,
		WindowFocus, WindowLostFocus, WindowMoved, WindowResize, WindowClose,
		Key, KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseButtonDown, MouseMoved, MouseScrolled,

		TextureCreated, TextureDeleted,	

		SceneCreated, SceneDeleted, 

		// Physics
		ConstraintAdded, ConstraintRemoved, ConstraintAltered,
		RigidbodyAdded, RigidbodyRemoved, RigidbodyAltered,
		Collision, Separation
	};

	class Event
	{
	public:
		virtual ~Event() {}

		bool Handled = false;

		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;

		virtual std::string ToString() const { return GetName(); }
	};

	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event) : m_Event(event) {}

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
		Event& m_Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}

	using EventCallbackFn = std::function<void(Event&)>;

}
