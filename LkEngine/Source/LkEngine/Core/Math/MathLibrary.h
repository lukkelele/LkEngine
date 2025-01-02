#pragma once

#include <random>
#include <limits>
#include <type_traits>

/**
 * TODO:
 */

#include "Math.h" /* FIXME: Temporary include until refactor is done. */

namespace LkEngine::Math {

	template <typename T>
	T GenerateRandomNumber()
	{
		static_assert(std::is_integral<T>::value, "T must be an integral type");
		constexpr T Min = std::numeric_limits<T>::min();
		constexpr T Max = std::numeric_limits<T>::max();
		LK_CORE_ASSERT(Min <= Max, "Minimum value has to be less than or equal to maximum, min: {}, max: {}", Min, Max);

		thread_local static std::mt19937 Generator(std::random_device{}());
		thread_local static std::uniform_int_distribution<typename std::make_unsigned<T>::type> Distribution;

		/* Update the distribution range before generating a number. */
		return static_cast<T>(Distribution(Generator, 
			typename std::uniform_int_distribution<typename std::make_unsigned<T>::type>::param_type{
				static_cast<typename std::make_unsigned<T>::type>(Min), 
				static_cast<typename std::make_unsigned<T>::type>(Max)
			}));
	}

}

