///|//////////////////////////////////////////////////////
/// LWindowsThread
///
///
///|//////////////////////////////////////////////////////
#pragma once

#include "LkEngine/Core/Thread.h"

namespace LkEngine {

	class LWindowsThread : public LThread<LWindowsThread>
	{
	public:
		LWindowsThread();
		~LWindowsThread();

	private:
		/**
		 * Internal sleep implementation using CRTP design pattern.
		 */
		template <typename TRep, typename TPeriod>
		void Internal_SleepImpl(const std::chrono::duration<TRep, TPeriod> Duration)
		{
			std::this_thread::sleep_for(Duration);
		}

	private:

		friend class LThread;
	};

}
