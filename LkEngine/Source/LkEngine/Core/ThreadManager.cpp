#include "LKpch.h"
#include "ThreadManager.h"


namespace LkEngine {

	LThreadManager& LThreadManager::Get()
	{
		static LThreadManager ThreadManager;
		return ThreadManager;
	}

	void LThreadManager::UpdateThreads()
	{
		for (const std::shared_ptr<TThread>& Thread : ThreadPool)
		{
			const bool bThreadIsRunning = Thread->IsRunning();
		}
	}

}
