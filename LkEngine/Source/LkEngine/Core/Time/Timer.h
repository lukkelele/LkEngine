#pragma once

#include <chrono>

#include "LkEngine/Core/CoreMacros.h"
#include "LkEngine/Core/CoreTypes.h"


namespace LkEngine {

	using namespace std::chrono_literals;

	/**
	 * LTimer
	 */
	class LTimer
	{
	public:
		LTimer() 
		{ 
			Reset(); 
		}

		virtual ~LTimer() = default;

		FORCEINLINE void Reset() 
		{ 
			using namespace std::chrono;
			StartTime = high_resolution_clock::now(); 
		}

		template<typename Rep, typename Period>
		std::chrono::duration<Rep, Period> GetElapsed() const
		{
			using namespace std::chrono;
			return (high_resolution_clock::now() - StartTime);
		}

		FORCEINLINE float GetDeltaTime()
		{
			using namespace std::chrono;
			const time_point<high_resolution_clock> Now = high_resolution_clock::now();
			const duration<float, std::milli> DeltaTime = Now - LastTime;
			LastTime = Now;

			return DeltaTime.count();
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> StartTime{};
		std::chrono::time_point<std::chrono::high_resolution_clock> LastTime{};
	};

}
