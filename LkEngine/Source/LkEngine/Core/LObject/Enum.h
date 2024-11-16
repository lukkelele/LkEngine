/******************************************************************
 * Enum
 * 
 *******************************************************************/
#pragma once

#include <type_traits>
#include <typeinfo>

/** 
 * @brief Defines all bitwise operators for enum classes so they can 
 *        be used as a regular enum.
 */
#define LK_ENUM_CLASS_FLAGS(Enum) \
	inline           Enum& operator|=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs | (__underlying_type(Enum))Rhs); } \
	inline           Enum& operator&=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs & (__underlying_type(Enum))Rhs); } \
	inline           Enum& operator^=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs ^ (__underlying_type(Enum))Rhs); } \
	inline constexpr Enum  operator| (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs | (__underlying_type(Enum))Rhs); } \
	inline constexpr Enum  operator& (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs & (__underlying_type(Enum))Rhs); } \
	inline constexpr Enum  operator^ (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs ^ (__underlying_type(Enum))Rhs); } \
	inline constexpr bool  operator! (Enum  E)             { return !(__underlying_type(Enum))E; } \
	inline constexpr Enum  operator~ (Enum  E)             { return (Enum)~(__underlying_type(Enum))E; }


/**
 * @brief Friends all bitwise operators for enum classes so the 
 *        definition can be kept private/protected.
 */
#define LK_FRIEND_ENUM_CLASS_FLAGS(Enum) \
	friend           Enum& operator|=(Enum& Lhs, Enum Rhs); \
	friend           Enum& operator&=(Enum& Lhs, Enum Rhs); \
	friend           Enum& operator^=(Enum& Lhs, Enum Rhs); \
	friend constexpr Enum  operator| (Enum  Lhs, Enum Rhs); \
	friend constexpr Enum  operator& (Enum  Lhs, Enum Rhs); \
	friend constexpr Enum  operator^ (Enum  Lhs, Enum Rhs); \
	friend constexpr bool  operator! (Enum  E); \
	friend constexpr Enum  operator~ (Enum  E);


namespace LkEngine::Enum {

	template<typename Enum>
	constexpr bool HasAllFlags(Enum Flags, Enum Contains)
	{
		using UnderlyingType = __underlying_type(Enum);
		return ((UnderlyingType)Flags & (UnderlyingType)Contains) == (UnderlyingType)Contains;
	}

	template<typename Enum>
	constexpr bool HasAnyFlags(Enum Flags, Enum Contains)
	{
		using UnderlyingType = __underlying_type(Enum);
		return ((UnderlyingType)Flags & (UnderlyingType)Contains) != 0;
	}

	template<typename Enum>
	void AddFlags(Enum& Flags, Enum FlagsToAdd)
	{
		using UnderlyingType = __underlying_type(Enum);
		Flags = (Enum)((UnderlyingType)Flags | (UnderlyingType)FlagsToAdd);
	}

	template<typename Enum>
	void RemoveFlags(Enum& Flags, Enum FlagsToRemove)
	{
		using UnderlyingType = __underlying_type(Enum);
		Flags = (Enum)((UnderlyingType)Flags & ~(UnderlyingType)FlagsToRemove);
	}

}
