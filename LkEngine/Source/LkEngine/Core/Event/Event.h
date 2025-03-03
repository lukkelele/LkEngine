#pragma once

#include "LkEngine/Core/Core.h"

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

namespace LkEngine {

	/**
	 * EEventType
	 * 
	 *  Type of event.
	 */
	enum class EEventType : uint32_t
	{
		None = 0,
		WindowFocus, 
		WindowLostFocus, 
		WindowMoved, 
		WindowResize, 
		WindowClose,
		KeyPressed, 
		KeyReleased,
		MouseButtonPressed, 
		MouseButtonReleased, 
		MouseButtonHeld, 
		MouseMoved, 
		MouseScrolled,
		COUNT
	};
	LK_ENUM_CLASS(EEventType);
	LK_ENUM_RANGE_BY_COUNT(EEventType, EEventType::COUNT);

	/**
	 * EEventCategory
	 * 
	 *  Category for different type of events.
	 */
	enum class EEventCategory : uint32_t
	{
		None = 0,
		Input,
		COUNT
	};
	LK_ENUM_CLASS(EEventCategory);
	LK_ENUM_RANGE_BY_COUNT(EEventCategory, EEventCategory::COUNT);


	/**
	 * LEVENT
	 * 
	 *  Implements required event functionality.
	 */
	#define LEVENT(EventName, EventType, ...) \
		public: \
		using StaticEventClass = EventName; \
		static ::LkEngine::EEventType GetStaticType() { return EEventType::EventType; } \
		virtual ::LkEngine::EEventType GetType() const override { return GetStaticType(); } \
		virtual const char* GetName() const override { return #EventType; } \

	/**
	 * LEvent
	 * 
	 *  Base event class.
	 *
	 *  TODO: Derive from LObject and replace all 
	 *        std::shared_ptr occurences with TObjectPtr.
	 */
	class LEvent
	{
	public:
		virtual ~LEvent() = default;

		virtual EEventType GetType() const = 0;
		virtual const char* GetName() const = 0;
		virtual std::string ToString() const { return GetName(); }

		FORCEINLINE bool IsHandled() const { return bHandled; }

	public:
		bool bHandled = false;
	};

	inline std::ostream& operator<<(std::ostream& OStream, const LEvent& InEvent)
	{
		return OStream << InEvent.ToString();
	}

	using FEventCallback = std::function<void(LEvent&)>;
	using FEventHandler = std::function<void(LEvent&)>;

	/**
	 * LEventDispatcher
	 */
	class LEventDispatcher
	{
		template<typename EventType>
		using EventFn = std::function<bool(EventType&)>;
	public:
		LEventDispatcher(LEvent& InEvent) 
			: EventRef(InEvent) 
		{
		}

		template<typename T>
		FORCEINLINE bool Dispatch(EventFn<T> InEventFunc)
		{
			if (!EventRef.bHandled)
			{
				EventRef.bHandled = InEventFunc(*(T*)&EventRef);
				LK_CORE_TRACE_TAG("EventDispatcher", "Executing: '{}'", EventRef.GetName());
				return true;
			}
			return false;
		}

	private:
		LEvent& EventRef;
	};

}
