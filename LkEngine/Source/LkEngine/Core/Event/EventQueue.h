#pragma once

#include <mutex>

#include "Event.h"


namespace LkEngine {

	template<typename InvokerType>
	struct TEventQueueEntry
	{
		const char* Name = nullptr;
		EEventType Type = EEventType::None;
		std::size_t TypeHash = 0;
		std::size_t Idx = -1;
		InvokerType Event;

		TEventQueueEntry(const char* InName, 
						 const EEventType& InType, 
						 const std::type_info& InTypeInfo, 
						 const std::size_t InIdx,
						 const InvokerType& InEvent)
			: Name(InName)
			, Type(InType)
			, TypeHash(InTypeInfo.hash_code())
			, Idx(InIdx)
			, Event(InEvent)
		{
			LK_CORE_ASSERT(Idx != -1, "No index passed to event queue entry");
		}

		TEventQueueEntry(TEventQueueEntry&& Other)
			: Name(Other.Name)
			, Type(Other.Type)
			, TypeHash(Other.TypeHash)
			, Idx(Other.Idx)
			, Event(std::move(Other.Event))
		{
			LK_CORE_ASSERT(Idx != -1, "No index passed to event queue entry");
		}

		TEventQueueEntry& operator=(const TEventQueueEntry& Other)
		{
			Name = Other.Name;
			Type = Other.Type;
			TypeHash = Other.TypeHash;
			Idx = Other.Idx;
			Event = Other.Event;
			return *this;
		}

		FORCEINLINE bool operator==(const TEventQueueEntry& Other) const 
		{ 
			return ((Idx == Other.Idx) 
					&& (Type == Other.Type) 
					&& (TypeHash == Other.TypeHash) 
					&& (Name == Other.Name) 
					&& (Name != nullptr));
		}

		FORCEINLINE bool operator!=(const TEventQueueEntry& Other) const 
		{ 
			return !(*this == Other); 
		}
	};

	/**
	 * LEventQueue
	 *
	 *  Queue structure for events.
	 */
	class LEventQueue : public LObject
	{
		using EventInvoker = std::function<void()>;
		using EventElement = TEventQueueEntry<EventInvoker>;
		using EventContainer = std::vector<EventElement>;
	public:
		LEventQueue(std::string_view InQueueName)
			: QueueName(InQueueName.data())
		{
			LK_CORE_VERIFY(!InQueueName.empty());
			LOBJECT_REGISTER();
		}
		LEventQueue(std::string_view InQueueName, const FEventHandler InHandler)
			: QueueName(InQueueName.data())
			, Handler(InHandler)
		{
			LK_CORE_VERIFY(!InQueueName.empty());
			LK_CORE_VERIFY(InHandler, "Passed invalid handler to event queue");
			LOBJECT_REGISTER();
		}
		LEventQueue() = delete;
		LEventQueue(const LEventQueue& Other) = delete;
		LEventQueue(LEventQueue&& Other) = delete;
		virtual ~LEventQueue() = default;

		LEventQueue& operator=(const LEventQueue& Other) = delete;
		LEventQueue& operator=(LEventQueue&& Other) = delete;

		void AttachHandler(const FEventHandler InHandler)
		{
			LK_CORE_DEBUG_TAG(std::format("{}", QueueName), "Attaching event handler");
			Handler = InHandler;
		}

		/**
		 * Process queued events.
		 *
		 *  Handles queued events up to the passed threshold 
		 *  or until empty if the threshold is set to 0 (default).
		 */
		FORCEINLINE void Process(const uint16_t MaxThreshold = 0)
		{
			std::scoped_lock<std::mutex> ScopedLock(Mutex);
			const std::size_t EventsToProcess = (MaxThreshold == 0) ? EventQueue.size() : MaxThreshold;
			for (int Idx = 0; Idx < EventsToProcess; Idx++)
			{
				auto Iter = EventQueue.begin();
				Iter->Event();
				EventQueue.erase(Iter);
			}
		}

		/**
		 * Process queued events of a certain type.
		 *
		 *  Handles events up to the passed threshold or until empty 
		 *  if the threshold is set to 0.
		 */
		template<typename... EventFilter>
		FORCEINLINE void ProcessFiltered(const uint16_t MaxThreshold = 0)
		{
			std::scoped_lock<std::mutex> ScopedLock(Mutex);
			constexpr int FilterCount = (sizeof...(EventFilter));
			static_assert(FilterCount > 0, "At least 1 event type is required");

			/* Filter for one single event type. */
			if constexpr (FilterCount == 1)
			{
				using TupleElementType = std::tuple_element_t<0, std::tuple<EventFilter...>>;
				constexpr bool IsArgEnum = std::is_enum_v<TupleElementType>;

				auto IsEntryCorrectType = [](const EventElement& Entry) -> bool
				{
					if constexpr (IsArgEnum)
					{
						LK_CORE_ASSERT(false, "Enum implementation not supported yet");
						return false;
					}
					else
					{
						return (Entry.TypeHash == typeid(TupleElementType).hash_code());
					}
				};

				EventContainer FilteredQueue;
				std::ranges::move(
					EventQueue | std::views::filter(IsEntryCorrectType), 
					std::back_inserter(FilteredQueue)
				);
				if (FilteredQueue.empty())
				{
					return;
				}

				const std::size_t MovedEvents = std::erase_if(EventQueue, IsEntryCorrectType);
				if (MovedEvents > 0)
				{
					const std::size_t EventsToProcess = (MaxThreshold == 0) ? FilteredQueue.size() : MaxThreshold;
					for (int Idx = 0; Idx < EventsToProcess; Idx++)
					{
						auto Iter = FilteredQueue.begin();
						LK_CORE_ASSERT(Iter->Name != nullptr, "Event indexed {} is invalid", Idx);
						LK_CORE_CONSOLE_DEBUG("[{}] Executing{}: {}", QueueName, Iter->Idx, (Iter->Name ? Iter->Name : "NULL"));
						Iter->Event();
						FilteredQueue.erase(Iter);
					}
				}
			}
			else
			{
				LK_MARK_FUNC_NOT_IMPLEMENTED();
				/* TODO: Invoke recursively. */
			}
		}

		/**
		 * Enqueue an event.
		 *
		 *  The passed arguments must match the LEvent constructor 
		 *  for the used EventType.
		 */
		template<typename EventType, typename... EventArgs>
		FORCEINLINE void Add(EventArgs&&... Args)
		{
			std::scoped_lock<std::mutex> ScopedLock(Mutex);
			std::shared_ptr<EventType> Event = std::make_shared<EventType>(std::forward<EventArgs>(Args)...);
			EventQueue.emplace_back(Event->GetName(), Event->GetType(), typeid(*Event), EventQueue.size(), [&, Event]()
			{ 
				LK_CORE_ASSERT(Handler, "No event handler registered");
				if (Handler)
				{
					LK_CORE_TRACE_TAG("EventQueue", "Executing: {}", Event->ToString());
					/* The handler sets the event result in LEvent::bHandled. */
					Handler(*Event);

					if (!Event->IsHandled())
					{
						LK_CORE_ERROR("{}: Failed", Event->GetName());
					}
				}
			});
		}

		/**
		 * Get the current count of queued events.
		 */
		std::size_t GetCount() const { return EventQueue.size(); }
		
		/**
		 * Get the name of the event queue instance.
		 */
		const char* GetName() const { return QueueName; }

		FORCEINLINE EventContainer::iterator begin() { return EventQueue.begin(); }
		FORCEINLINE EventContainer::iterator end() { return EventQueue.end(); }

	private:
		const char* QueueName = nullptr;
		EventContainer EventQueue{};
		FEventHandler Handler{};
		std::mutex Mutex;

		LCLASS(LEventQueue);
	};

}
