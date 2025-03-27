/**
 * @file
 * @brief Timer manager.
 */
#pragma once

#include <array>

#include "LkEngine/Core/CoreMacros.h"
#include "LkEngine/Core/CoreTypes.h"
#include "LkEngine/Core/Assert.h"
#include "LkEngine/Core/Log/Log.h"
#include "LkEngine/Core/Delegate/Delegate.h"

#include "LkEngine/Core/Time/Time.h"
#include "LkEngine/Core/Time/Timer.h"
#include "LkEngine/Core/Time/TimerHandle.h"

namespace LkEngine {

	/**
	 * @ingroup Time
	 * @{
	 */

	/**
	 * @var FTimerDelegate
	 * Timer delegate.
	 *
	 * @note Used internally by LTimer.
	 */
	LK_DECLARE_DELEGATE(FTimerDelegate);

	/**
	 * @class LTimerManager
	 *
	 * @ingroup Time
	 */
	class LTimerManager
	{
	public:
		/** 
		 * @brief Set a timer with a delegate, delay and optional looping. 
		 * @return the handle of the created timer.
		 */
		FTimerHandle SetTimer(const FTimerDelegate& TimerDelegate, 
							  const float DelayInSeconds, 
							  const bool Loop = false)
		{
			using namespace std::chrono;
			FTimerHandle Handle = FTimerHandle::CreateHandle();
			Internal_FTimer Timer;
			Timer.Delegate = TimerDelegate; /* Copy the delegate. */
			Timer.EndTime = high_resolution_clock::now() + milliseconds(static_cast<int>(DelayInSeconds * 1000));
			Timer.bShouldLoop = Loop;
			Timer.Delay = DelayInSeconds;

			Timers[Handle] = std::move(Timer);

			return Handle;
		}

		/** 
		 * @brief Clear a timer by its handle. 
		 *
		 * @details Does nothing if the handle doesn't belong to an active timer.
		 */
		FORCEINLINE void ClearTimer(const FTimerHandle Handle)
		{
			if (Timers.contains(Handle))
			{
				Timers.erase(Handle);
			}
			else
			{
				LK_CORE_ERROR_TAG("TimerManager", "Timer with handle {} is not in timer collection", Handle.GetID());
			}
		}

		/** 
		 * @brief Check if a timer is active. 
		 * @return true if active, else false.
		 */
		FORCEINLINE bool IsTimerActive(const FTimerHandle Handle) const
		{
			return Timers.find(Handle) != Timers.end();
		}

		/** 
		 * @brief Update timers, execute callbacks as they expire. 
		 */
		FORCEINLINE void Update()
		{
			using namespace std::chrono;
			const high_resolution_clock::time_point Now = high_resolution_clock::now();

			uint8_t ExpiredTimers = 0;
			std::array<FTimerHandle, MaxExpiredTimerStackSize> ExpiredTimersArray;

			for (auto& [Handle, Timer] : Timers)
			{
				if (Now >= Timer.EndTime)
				{
					Timer.Delegate.ExecuteIfBound();

					if (Timer.bShouldLoop)
					{
						/* Reset the end time for the next interval. */
						Timer.EndTime = Now + milliseconds(static_cast<int>(Timer.Delay * 1000));
					}
					else
					{
						/* Mark for removal if it's a one-time timer. */
						LK_CORE_ASSERT((ExpiredTimers + 1) < MaxExpiredTimerStackSize, "Expired timer index exceed max stack count");
						ExpiredTimersArray[ExpiredTimers++] = Handle;
					}
				}
			}

			/* Remove non-looping expired timers. */
			for (uint8_t TimerIndex = 0; TimerIndex < ExpiredTimers; TimerIndex++)
			{
				Timers.erase(ExpiredTimersArray[TimerIndex]);
			}
		}

	private:
		/**
		 * @struct Internal_FTimer
		 *
		 * Internal delegate timer.
		 */
		struct Internal_FTimer
		{
			FTimerDelegate Delegate;
			std::chrono::high_resolution_clock::time_point EndTime{};
			float Delay = 0.0f;
			bool bShouldLoop = false;
		};

	private:
		/** 
		 * @var Timers
		 * Map collection of timers. 
		 */
		std::unordered_map<FTimerHandle, Internal_FTimer> Timers{};

		/** 
		 * @var MaxExpiredTimerStackSize
		 * Max allowed count of expired timers on the stack. 
		 */
		static constexpr uint8_t MaxExpiredTimerStackSize = 40;
	};

	/** @} */

}
