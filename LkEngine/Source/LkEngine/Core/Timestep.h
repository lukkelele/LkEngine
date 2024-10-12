/******************************************************************
 * Timestep
 * 
 *******************************************************************/
#pragma once

#include <stddef.h>
#include <stdint.h>


namespace LkEngine {

	class FTimestep
	{
	public:
		FTimestep(const float InTimestep = 0.0f)
			: Timestep(InTimestep) {}
		~FTimestep() = default;

		FORCEINLINE operator float() const { return Timestep; }
		FORCEINLINE FTimestep& operator=(const float InTimestep)
		{
			Timestep = InTimestep;
			return *this;
		}

	private:
		float Timestep = 0.0f;
	};

}