///|//////////////////////////////////////////////////////
/// LThreadManager
///
///
///|//////////////////////////////////////////////////////
#pragma once

#include "LkEngine/Core/Thread.h"


/* Windows Platform */
#if defined(LK_PLATFORM_WINDOWS)
	#include "LkEngine/Platform/Windows/WindowsThread.h"
	namespace LkEngine 
	{
		using TThread = LThread<LWindowsThread>;
	}
#endif


namespace LkEngine {

	struct FThreadStartArgs
	{
		bool bRunAfterCreation = false;
	};
	
	class LThreadManager
	{
	public:
		~LThreadManager() = default;

		template<typename TCallable, typename ...TArgs>
		void CreateThread(const FThreadStartArgs& ThreadStartArgs, TCallable&& Func, TArgs&&... Args);

		void UpdateThreads();

		FORCEINLINE uint8_t GetThreadPoolSize() const { return ThreadPool.size(); }

		FORCEINLINE void StartThread(const uint8_t ThreadIndex)
		{
			ThreadPool.at(ThreadIndex)->Run();
		}

		FORCEINLINE static void SubmitFunctionToThread(FThreadData& ThreadData, const std::function<void()> Function)
		{
			LK_CORE_DEBUG_TAG("ThreadManager", "Adding function to queue, new queue size: {}", ThreadData.CommandQueue.size() + 1);
			ThreadData.CommandQueue.push(Function);
		}

		static LThreadManager& Instance();

	private:
		std::vector<TSharedPtr<TThread>> ThreadPool{};
	};


	template<typename TCallable, typename ...TArgs>
	inline void LThreadManager::CreateThread(const FThreadStartArgs& ThreadStartArgs, TCallable&& Function, TArgs&& ...Args)
	{
		LK_CORE_DEBUG_TAG("ThreadManager", "Creating new thread, indexed={}", ThreadPool.size());

		TSharedPtr<TThread> Thread = MakeShared<TThread>(
			std::forward<TCallable>(Function), std::forward<TArgs>(Args)...
		);
		ThreadPool.push_back(Thread);

		if (ThreadStartArgs.bRunAfterCreation)
		{
			Thread->Run();
		}
	}


	static void Thread_SubmitCommand(FThreadData& ThreadData, const std::function<void()> Function)
	{
		ThreadData.CommandQueue.push(Function);
		LK_CORE_DEBUG_TAG("ThreadManager", "Added command to command queue, new queue size: {}", ThreadData.CommandQueue.size());
	}

	static void ThreadFunction_TestCommandQueue(FThreadData& ThreadData)
	{
		std::queue<std::function<void()>>& CommandQueue = ThreadData.CommandQueue;

		while (ThreadData.bIsRunning)
		{
			/* Handle commands if there are any. */
			while (CommandQueue.size() > 0)
			{
				//LK_CORE_DEBUG_TAG("ThreadFunction_1", "Executing function in queue, indexed={}", CommandQueue.size());
				std::function<void()> Function = CommandQueue.front();
				Function();
				CommandQueue.pop();
			}
		}
	}

}