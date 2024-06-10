#include "LKpch.h"
#include "ThreadManager.h"



namespace LkEngine {

	LThreadManager& LThreadManager::Instance()
	{
		static LThreadManager ThreadManager;
		return ThreadManager;
	}

	void LThreadManager::UpdateThreads()
	{
		for (const TSharedPtr<TThread>& Thread : ThreadPool)
		{
			const bool bThreadIsRunning = Thread->IsRunning();
		}
	}

}