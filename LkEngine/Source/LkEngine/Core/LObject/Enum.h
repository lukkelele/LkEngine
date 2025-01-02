/******************************************************************
 * Enum
 * 
 *******************************************************************/
#pragma once

#include <type_traits>
#include <typeinfo>


namespace LkEngine {

	/** 
	 * @brief Defines all bitwise operators for an enum class so it can 
	 *        be used as a regular enum.
	 */
	#define LK_ENUM_CLASS_FLAGS(EnumClass) \
		using LK_Enum_Type_##EnumClass = std::underlying_type_t<EnumClass>; \
		inline           EnumClass& operator|=(EnumClass& Lhs, EnumClass Rhs) { return Lhs = (EnumClass)(static_cast<LK_Enum_Type_##EnumClass>(Lhs) | static_cast<LK_Enum_Type_##EnumClass>(Rhs)); } \
		inline           EnumClass& operator&=(EnumClass& Lhs, EnumClass Rhs) { return Lhs = (EnumClass)(static_cast<LK_Enum_Type_##EnumClass>(Lhs) & static_cast<LK_Enum_Type_##EnumClass>(Rhs)); } \
		inline           EnumClass& operator^=(EnumClass& Lhs, EnumClass Rhs) { return Lhs = (EnumClass)(static_cast<LK_Enum_Type_##EnumClass>(Lhs) ^ static_cast<LK_Enum_Type_##EnumClass>(Rhs)); } \
		inline constexpr EnumClass  operator| (EnumClass  Lhs, EnumClass Rhs) { return (EnumClass)(static_cast<LK_Enum_Type_##EnumClass>(Lhs) | static_cast<LK_Enum_Type_##EnumClass>(Rhs)); } \
		inline constexpr EnumClass  operator& (EnumClass  Lhs, EnumClass Rhs) { return (EnumClass)(static_cast<LK_Enum_Type_##EnumClass>(Lhs) & static_cast<LK_Enum_Type_##EnumClass>(Rhs)); } \
		inline constexpr EnumClass  operator^ (EnumClass  Lhs, EnumClass Rhs) { return (EnumClass)(static_cast<LK_Enum_Type_##EnumClass>(Lhs) ^ static_cast<LK_Enum_Type_##EnumClass>(Rhs)); } \
		inline constexpr bool       operator! (EnumClass  E)                  { return !static_cast<LK_Enum_Type_##EnumClass>(E); } \
		inline constexpr EnumClass  operator~ (EnumClass  E)                  { return (EnumClass)~static_cast<LK_Enum_Type_##EnumClass>(E); } \
		inline constexpr LK_Enum_Type_##EnumClass operator&(LK_Enum_Type_##EnumClass Lhs, EnumClass Rhs) { return static_cast<LK_Enum_Type_##EnumClass>(Lhs & static_cast<LK_Enum_Type_##EnumClass>(Rhs)); } \
		inline constexpr LK_Enum_Type_##EnumClass operator&(EnumClass Lhs, LK_Enum_Type_##EnumClass Rhs) { return static_cast<LK_Enum_Type_##EnumClass>(static_cast<LK_Enum_Type_##EnumClass>(Lhs) & Rhs); } \
		inline constexpr LK_Enum_Type_##EnumClass operator|(LK_Enum_Type_##EnumClass Lhs, EnumClass Rhs) { return static_cast<LK_Enum_Type_##EnumClass>(Lhs | static_cast<LK_Enum_Type_##EnumClass>(Rhs)); } \
		inline constexpr LK_Enum_Type_##EnumClass operator|(EnumClass Lhs, LK_Enum_Type_##EnumClass Rhs) { return static_cast<LK_Enum_Type_##EnumClass>(static_cast<LK_Enum_Type_##EnumClass>(Lhs) | Rhs); } \
		inline constexpr bool operator<(EnumClass Lhs, LK_Enum_Type_##EnumClass Rhs)  { return static_cast<LK_Enum_Type_##EnumClass>(static_cast<LK_Enum_Type_##EnumClass>(Lhs) < Rhs); }  \
		inline constexpr bool operator<(LK_Enum_Type_##EnumClass Lhs, EnumClass Rhs)  { return static_cast<LK_Enum_Type_##EnumClass>(Lhs < static_cast<LK_Enum_Type_##EnumClass>(Rhs)); }  \
		inline constexpr bool operator>(EnumClass Lhs, LK_Enum_Type_##EnumClass Rhs)  { return static_cast<LK_Enum_Type_##EnumClass>(static_cast<LK_Enum_Type_##EnumClass>(Lhs) > Rhs); }  \
		inline constexpr bool operator>(LK_Enum_Type_##EnumClass Lhs, EnumClass Rhs)  { return static_cast<LK_Enum_Type_##EnumClass>(Lhs > static_cast<LK_Enum_Type_##EnumClass>(Rhs)); }  \
		inline constexpr bool operator<=(EnumClass Lhs, LK_Enum_Type_##EnumClass Rhs) { return static_cast<LK_Enum_Type_##EnumClass>(static_cast<LK_Enum_Type_##EnumClass>(Lhs) <= Rhs); } \
		inline constexpr bool operator<=(LK_Enum_Type_##EnumClass Lhs, EnumClass Rhs) { return static_cast<LK_Enum_Type_##EnumClass>(Lhs <= static_cast<LK_Enum_Type_##EnumClass>(Rhs)); } \
		inline constexpr bool operator>=(EnumClass Lhs, LK_Enum_Type_##EnumClass Rhs) { return static_cast<LK_Enum_Type_##EnumClass>(static_cast<LK_Enum_Type_##EnumClass>(Lhs) >= Rhs); } \
		inline constexpr bool operator>=(LK_Enum_Type_##EnumClass Lhs, EnumClass Rhs) { return static_cast<LK_Enum_Type_##EnumClass>(Lhs >= static_cast<LK_Enum_Type_##EnumClass>(Rhs)); }


	/**
	 * @brief Friends all bitwise operators for enum classes so the 
	 *        definition can be kept private/protected.
	 */
	#define LK_FRIEND_ENUM_CLASS_FLAGS(EnumClass) \
		friend           EnumClass& operator|=(EnumClass& Lhs, EnumClass Rhs); \
		friend           EnumClass& operator&=(EnumClass& Lhs, EnumClass Rhs); \
		friend           EnumClass& operator^=(EnumClass& Lhs, EnumClass Rhs); \
		friend constexpr EnumClass  operator| (EnumClass  Lhs, EnumClass Rhs); \
		friend constexpr EnumClass  operator& (EnumClass  Lhs, EnumClass Rhs); \
		friend constexpr EnumClass  operator^ (EnumClass  Lhs, EnumClass Rhs); \
		friend constexpr bool       operator! (EnumClass  E); \
		friend constexpr EnumClass  operator~ (EnumClass  E);

}

namespace LkEngine::Enum {

	template<typename Enum>
	constexpr bool HasAllFlags(Enum Flags, Enum Contains)
	{
		using UnderlyingType = std::underlying_type_t<Enum>;
		return ((UnderlyingType)Flags & (UnderlyingType)Contains) == (UnderlyingType)Contains;
	}

	template<typename Enum>
	constexpr bool HasAnyFlags(Enum Flags, Enum Contains)
	{
		using UnderlyingType = std::underlying_type_t<Enum>;
		return ((UnderlyingType)Flags & (UnderlyingType)Contains) != 0;
	}

	template<typename Enum>
	void AddFlags(Enum& Flags, Enum FlagsToAdd)
	{
		using UnderlyingType = std::underlying_type_t<Enum>;
		Flags = (Enum)((UnderlyingType)Flags | (UnderlyingType)FlagsToAdd);
	}

	template<typename Enum>
	void RemoveFlags(Enum& Flags, Enum FlagsToRemove)
	{
		using UnderlyingType = std::underlying_type_t<Enum>;
		Flags = (Enum)((UnderlyingType)Flags & ~(UnderlyingType)FlagsToRemove);
	}

}
