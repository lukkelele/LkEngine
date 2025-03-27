/**
 * @file
 * @brief Timer
 */
#pragma once

#include <chrono>

#include "LkEngine/Core/CoreMacros.h"
#include "LkEngine/Core/CoreTypes.h"

#include "LkEngine/Debug/Profiler.h"

namespace LkEngine {

	using namespace std::chrono_literals;

	/**
	 * @class LTimer
	 *
	 * Basic timer.
	 *
	 * @ingroup Time
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

		template<typename TDuration>
		TDuration GetElapsed() const
		{
			using namespace std::chrono;
			static_assert(std::disjunction_v<
							  std::is_same<TDuration, microseconds>,
							  std::is_same<TDuration, milliseconds>,
							  std::is_same<TDuration, seconds>>, "Timer format not supported");
			return duration_cast<TDuration>(high_resolution_clock::now() - StartTime);
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

	/**
	 * @class FScopedTimer
	 * 
	 * Scoped timer used for profiling.
	 */
	class FScopedTimer
	{
	public:
		FScopedTimer(const std::string& InName) 
			: Name(InName) 
		{
		}

		~FScopedTimer()
		{
			const float Time = Timer.GetElapsed<std::chrono::milliseconds>().count();
			LK_CORE_INFO_TAG("TIMER", "{0} - {1}ms", Name, Time);
		}

	private:
		std::string Name{};
		LTimer Timer{};
	};

	/**
	 * @class LPerformanceProfiler
	 *
	 * Performance profiler.
	 *
	 * @ingroup Profiling
	 */
	class LPerformanceProfiler
	{
	public:
		struct FFrameData
		{
			float Time = 0.0f;
			uint32_t Samples = 0;

			FFrameData() = default;
			FFrameData(const float InTime) : Time(InTime) {}

			operator float() const { return Time; }

			inline FFrameData& operator+=(const float InTime)
			{
				Time += InTime;
				return *this;
			}
		};
	public:
		FORCEINLINE void SetFrameTiming(const char* InName, const float InTime)
		{
			std::scoped_lock<std::mutex> ScopedLock(FrameDataMutex);
			if (FrameDataMap.find(InName) == FrameDataMap.end())
			{
				FrameDataMap[InName] = 0.0f;
			}

			FFrameData& FrameData = FrameDataMap[InName];
			FrameData.Time += InTime;
			FrameData.Samples++;
		}

		FORCEINLINE void Clear()
		{
			std::scoped_lock<std::mutex> ScopedLock(FrameDataMutex);
			FrameDataMap.clear();
		}

		const std::unordered_map<const char*, FFrameData>& GetFrameData() const 
		{ 
			return FrameDataMap; 
		}

	private:
		std::unordered_map<const char*, FFrameData> FrameDataMap{};
		inline static std::mutex FrameDataMutex;
	};

	/**
	 * @class FScopedPerformanceTimer
	 *
	 * Scoped performance profiler.
	 * @ingroup Profiling
	 */
	class FScopedPerformanceTimer
	{
	public:
		FScopedPerformanceTimer(const char* InName, LPerformanceProfiler* InPerformanceProfiler)
			: Name(InName)
			, Profiler(InPerformanceProfiler) 
		{
			LK_CORE_ASSERT(Profiler, "Profiler is nullptr");
		}

		~FScopedPerformanceTimer()
		{
			const float Time = Timer.GetElapsed<std::chrono::milliseconds>().count();
			Profiler->SetFrameTiming(Name, Time);
		}

	private:
		const char* Name;
		LPerformanceProfiler* Profiler{};

		LTimer Timer;
	};

	/** @} */

}

