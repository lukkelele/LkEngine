#pragma once

#include <optional>

#include "LkEngine/Core/Core.h"

namespace LkEngine {

	using FThreadHandle = uint64_t;

	/**
	 * @class IThread
	 * 
	 *  Thread interface.
	 */
	class IThread
	{
	public:
		virtual ~IThread() = default;

		virtual void Run() = 0;
		virtual FThreadHandle GetHandle() const = 0;
		virtual bool IsRunning() const = 0;
	};

	/**
	 * @class LThread
	 *
	 *  Thread implementation, platform-specific.
	 */
	template<typename TPlatformType>
	class LThread : public IThread
	{
	public:
		LThread() = default;
		template<typename TCallable, typename ...TArgs>
		LThread(TCallable&& Func, TArgs&&... Args)
		{
			Task = [Func = std::forward<TCallable>(Func), ...Args = std::forward<TArgs>(Args)]() mutable
			{
				std::invoke(Func, std::move(Args)...);
			};
		}

		virtual ~LThread() = default;

		/**
		 * @brief Setup thread.
		 * Used whenever the thread function hasn't been passed in the constructor.
		 */
		template<typename TCallable, typename ...TArgs>
		void Setup(TCallable&& Func, TArgs&&... Args)
		{
			Task = [Func = std::forward<TCallable>(Func), ...Args = std::forward<TArgs>(Args)]() mutable
			{
				std::invoke(Func, std::move(Args)...);
			};
		}

		FORCEINLINE void Run()
		{
			LK_CORE_DEBUG_TAG("LThread", "Invoking Run");
			if (WorkerThread.joinable())
			{
				LK_CORE_ERROR_TAG("LThread", "Thread is already running");
				assert(false);
			}

			if (Task && Task.has_value())
			{
				WorkerThread = std::thread([this] 
				{ 
					bIsRunning = true;

					if (Task)
					{
						(*Task)(); 
					}

					bIsRunning = false;
				});
			}
		}

		/**
		 * @brief Get the thread handle.
		 */
		FORCEINLINE FThreadHandle GetHandle() const { return Handle; }

		/**
		 * @brief Check if the thread is running.
		 */
		FORCEINLINE bool IsRunning() const { return bIsRunning; }

		/**
		 * @brief Sleep for a duration, inhibiting thread execution.
		 * Uses std::chrono.
		 */
		template <typename TRep, typename TPeriod>
		static void Sleep(const std::chrono::duration<TRep, TPeriod> Duration)
		{
			std::this_thread::sleep_for(Duration);
		}

	protected:
		FThreadHandle Handle = 0;

		std::thread WorkerThread;
		std::optional<std::function<void()>> Task;

		bool bIsRunning = false;
	};

	struct FThreadData
	{
		bool bIsRunning = true;
		std::queue<std::function<void()>> CommandQueue{};
	};

}
