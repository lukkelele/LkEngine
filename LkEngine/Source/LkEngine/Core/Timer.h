#pragma once

#include <chrono>


namespace LkEngine {

	class Timer
	{
	public:
		Timer() { Reset(); }

		void Reset() { m_Start = std::chrono::high_resolution_clock::now(); }

		// Get elapsed time since m_Start in nanoseconds
		float Elapsed()
		{
			return std::chrono::duration_cast<std::chrono::nanoseconds>
				(std::chrono::high_resolution_clock::now() - m_Start).count() * 0.001f * 0.001f * 0.001f;
		}

		float ElapsedMs()
		{
			return Elapsed() * 1000.0f;
		}

		float GetDeltaTime()
		{
			float now = ElapsedMs();
			float deltaTime = now - m_LastTime;
			m_LastTime = now;
			return deltaTime;
		}

		static llong GetTimeSinceEpoch()
		{
			return std::chrono::high_resolution_clock::now().time_since_epoch().count();
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
		float m_LastTime = 0.0f;
	};

}
