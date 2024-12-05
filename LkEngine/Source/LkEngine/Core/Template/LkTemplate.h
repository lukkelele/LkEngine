/******************************************************************
 * CoreTypes
 * 
 *******************************************************************/
#pragma once

#include "TypeTrait.h"

#include "LkEngine/Core/Log/Log.h"
#include "LkEngine/Core/Assert.h"


namespace LkEngine {

	/**
	 * @brief Uses implicit conversion to create an instance of a specific type.
	 * Useful to make things clearer or circumvent unintended type deduction in templates.
     * Safer than C casts and static_casts, i.e does not allow down-casts which could
	 * become problematic in templates.
	 */
	template <typename T>
	FORCEINLINE T ImplicitConv(typename std::type_identity_t<T> Object)
	{
		return Object;
	}

	/**
	 * @brief Get number of items in an initializer list.
	 */
	template <typename T>
	constexpr int32_t GetNum(std::initializer_list<T> List)
	{
		return static_cast<int32_t>(List.size());
	}

	/**
	 * @brief Returns a non-const reference type as const.
	 */
	template <typename T>
	constexpr FORCEINLINE const T& AsConst(T& Ref)
	{
		return Ref;
	}

	/**
	 * Not allowed for rvalue references because their lifetime cannot be extended.
	 */
	template <typename T>
	void AsConst(const T&& Ref) = delete;

	/** 
	 * Bitmask. 
	 */
	template <typename T>
	FORCEINLINE T BitMask(const uint32_t Count);

	template <>
	FORCEINLINE uint64_t BitMask<uint64_t>(const uint32_t Count)
	{
		LK_CORE_ASSERT(Count <= 64);
		return (uint64_t(Count < 64) << Count) - 1;
	}

	template <>
	FORCEINLINE uint32_t BitMask<uint32_t>(const uint32_t Count)
	{
		LK_CORE_ASSERT(Count <= 32);
		return uint32_t(uint64_t(1) << Count) - 1;
	}

	template <>
	FORCEINLINE uint16_t BitMask<uint16_t>(const uint32_t Count)
	{
		LK_CORE_ASSERT(Count <= 16);
		return uint16_t((uint32_t(1) << Count) - 1);
	}

	template <>
	FORCEINLINE uint8_t BitMask<uint8_t>(const uint32_t Count)
	{
		LK_CORE_ASSERT(Count <= 8);
		return uint8_t((uint32_t(1) << Count) - 1);
	}

}
