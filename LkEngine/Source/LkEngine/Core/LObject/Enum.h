/******************************************************************
 * Enum
 * 
 *******************************************************************/
#pragma once

#include <type_traits>
#include <typeinfo>

/**  
 * TODO: Need to fix so macros like FORCEINLINE are allowed to be included from anywhere. 
 * Currently causes errors because 'Enum.h' is included in 'CoreMacros.h'.
 */
#ifndef FORCEINLINE
#define FORCEINLINE	__forceinline
#endif


namespace LkEngine {

	/** 
	 * @brief Defines all bitwise operators for an enum class so it can 
	 *        be used as a regular enum.
	 */
	#define LK_ENUM_CLASS_FLAGS(EnumClass) \
		template<> struct ::LkEngine::Enum::Internal::IsFlagEnum<EnumClass> : std::true_type { }; \
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
		inline constexpr LK_Enum_Type_##EnumClass operator^(LK_Enum_Type_##EnumClass Lhs, EnumClass Rhs) { return static_cast<LK_Enum_Type_##EnumClass>(Lhs ^ static_cast<LK_Enum_Type_##EnumClass>(Rhs)); } \
		inline constexpr LK_Enum_Type_##EnumClass operator^(EnumClass Lhs, LK_Enum_Type_##EnumClass Rhs) { return static_cast<LK_Enum_Type_##EnumClass>(static_cast<LK_Enum_Type_##EnumClass>(Lhs) ^ Rhs); } \
		inline constexpr bool operator<(EnumClass Lhs, LK_Enum_Type_##EnumClass Rhs)  { return static_cast<LK_Enum_Type_##EnumClass>(static_cast<LK_Enum_Type_##EnumClass>(Lhs) < Rhs); }  \
		inline constexpr bool operator<(LK_Enum_Type_##EnumClass Lhs, EnumClass Rhs)  { return static_cast<LK_Enum_Type_##EnumClass>(Lhs < static_cast<LK_Enum_Type_##EnumClass>(Rhs)); }  \
		inline constexpr bool operator>(EnumClass Lhs, LK_Enum_Type_##EnumClass Rhs)  { return static_cast<LK_Enum_Type_##EnumClass>(static_cast<LK_Enum_Type_##EnumClass>(Lhs) > Rhs); }  \
		inline constexpr bool operator>(LK_Enum_Type_##EnumClass Lhs, EnumClass Rhs)  { return static_cast<LK_Enum_Type_##EnumClass>(Lhs > static_cast<LK_Enum_Type_##EnumClass>(Rhs)); }  \
		inline constexpr bool operator<=(EnumClass Lhs, LK_Enum_Type_##EnumClass Rhs) { return static_cast<LK_Enum_Type_##EnumClass>(static_cast<LK_Enum_Type_##EnumClass>(Lhs) <= Rhs); } \
		inline constexpr bool operator<=(LK_Enum_Type_##EnumClass Lhs, EnumClass Rhs) { return static_cast<LK_Enum_Type_##EnumClass>(Lhs <= static_cast<LK_Enum_Type_##EnumClass>(Rhs)); } \
		inline constexpr bool operator>=(EnumClass Lhs, LK_Enum_Type_##EnumClass Rhs) { return static_cast<LK_Enum_Type_##EnumClass>(static_cast<LK_Enum_Type_##EnumClass>(Lhs) >= Rhs); } \
		inline constexpr bool operator>=(LK_Enum_Type_##EnumClass Lhs, EnumClass Rhs) { return static_cast<LK_Enum_Type_##EnumClass>(Lhs >= static_cast<LK_Enum_Type_##EnumClass>(Rhs)); } \
		inline LK_Enum_Type_##EnumClass& operator|=(LK_Enum_Type_##EnumClass& Lhs, EnumClass Rhs) { return Lhs = (LK_Enum_Type_##EnumClass)(Lhs | static_cast<LK_Enum_Type_##EnumClass>(Rhs)); } \
		inline LK_Enum_Type_##EnumClass& operator&=(LK_Enum_Type_##EnumClass& Lhs, EnumClass Rhs) { return Lhs = (LK_Enum_Type_##EnumClass)(Lhs & static_cast<LK_Enum_Type_##EnumClass>(Rhs)); } \
		inline EnumClass& operator&=(EnumClass& Lhs, LK_Enum_Type_##EnumClass Rhs) { return Lhs = (EnumClass)(static_cast<LK_Enum_Type_##EnumClass>(Lhs) & Rhs); } \
		inline EnumClass& operator|=(EnumClass& Lhs, LK_Enum_Type_##EnumClass Rhs) { return Lhs = (EnumClass)(static_cast<LK_Enum_Type_##EnumClass>(Lhs) | Rhs); } 


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


	#define LK_ENUM_RANGE_BY_FIRST_AND_LAST(EnumType, First, Last) \
		template<> \
		struct ::LkEngine::Enum::Internal::Range::Type<EnumType> \
		{ \
			enum { RangeType = 0 }; \
			static constexpr std::underlying_type_t<EnumType> Begin = (std::underlying_type_t<EnumType>)(First); \
			static constexpr std::underlying_type_t<EnumType> End   = (std::underlying_type_t<EnumType>)(Last) + 1; \
		};

	#define LK_ENUM_RANGE_BY_VALUES(EnumType, ...) \
		template<> \
		struct ::LkEngine::Enum::Internal::Range::Type<EnumType> \
		{ \
			enum { RangeType = 1 }; \
			template<typename DummyType> \
			static const EnumType* GetPointer(const bool IsLast) \
			{ \
				static constexpr EnumType Values[] = { __VA_ARGS__ }; \
				return IsLast ? Values + sizeof(Values) / sizeof(EnumType) : Values; \
			} \
		};

	#define LK_ENUM_RANGE_BY_COUNT(EnumType, Count) \
				LK_ENUM_RANGE_BY_FIRST_AND_LAST(EnumType, 0, (std::underlying_type_t<EnumType>)(Count) - 1)


	namespace Enum::Internal 
	{
		/** Declared true when LK_ENUM_CLASS_FLAGS macro is used on an enum class. */
		template<typename EnumType>
		struct IsFlagEnum : std::false_type { };

		template<typename EnumType>
		inline constexpr bool IsFlagEnum_v = IsFlagEnum<EnumType>::value;
	}

	namespace Enum::Internal::Range
	{
		/**
		 * Enum range traits.
		 *  0: Continous.
		 *  1: Value based.
		 *  2: Flag based. (IMPL NOT DONE)
		 */
		template<typename EnumType>
		struct Type { enum { RangeType = -1 }; };

		template<typename EnumType>
		struct ContiguousIterator
		{
			using IntType = std::underlying_type_t<EnumType>;

			FORCEINLINE explicit ContiguousIterator(IntType InValue)
				: Value(InValue)
			{
			}

			FORCEINLINE ContiguousIterator& operator++()
			{
				++Value;
				return *this;
			}

			FORCEINLINE EnumType operator*() const { return (EnumType)Value; }

		private:
			FORCEINLINE friend bool operator!=(const ContiguousIterator& Lhs, const ContiguousIterator& Rhs)
			{
				return (Lhs.Value != Rhs.Value);
			}

		private:
			IntType Value;
		};

		template<typename EnumType>
		struct ValueArrayIterator
		{
			FORCEINLINE explicit ValueArrayIterator(const EnumType* InPtr)
				: Ptr(InPtr)
			{
			}

			FORCEINLINE ValueArrayIterator& operator++()
			{
				++Ptr;
				return *this;
			}

			FORCEINLINE EnumType operator*() const { return *Ptr; }

		private:
			FORCEINLINE friend bool operator!=(const ValueArrayIterator& Lhs, const ValueArrayIterator& Rhs)
			{
				return (Lhs.Ptr != Rhs.Ptr);
			}

		private:
			const EnumType* Ptr;
		};

		template<typename EnumType>
        struct FlagIterator
        {
            using IntType = std::underlying_type_t<EnumType>;

            explicit FlagIterator(IntType Flags) 
				: RemainingFlags(Flags)
            {
            }

            FlagIterator& operator++()
            {
				/* Clear the lowest set bit. */
                RemainingFlags &= (RemainingFlags - 1);
                return *this;
            }

            EnumType operator*() const
            {
				/* Extract the lowest set bit. */
                IntType Flag = RemainingFlags & -RemainingFlags;
                return static_cast<EnumType>(Flag);
            }

		private:
            friend bool operator!=(const FlagIterator& Lhs, const FlagIterator& Rhs)
            {
                return (Lhs.RemainingFlags != Rhs.RemainingFlags);
            }

        private:
            IntType RemainingFlags;
        };

		template<typename EnumType>
		struct TIterator
		{
			using IntType = std::underlying_type_t<EnumType>;

			FORCEINLINE explicit TIterator(EnumType InFlags)
				: Flags(IntType(InFlags))
			{
			}

			FORCEINLINE TIterator& operator++()
			{
				const IntType PoppedBit = Flags & (~Flags + 1);
				Flags ^= PoppedBit;
				return *this;
			}

			FORCEINLINE EnumType operator*() const
			{
				const IntType Result = Flags & (~Flags + 1); 
				return (EnumType)Result;
			}

		private:
			FORCEINLINE friend bool operator!=(const TIterator& Lhs, const TIterator& Rhs)
			{
				return (Lhs.Flags != Rhs.Flags);
			}

		private:
			IntType Flags;
		};

		template<typename EnumType, int32_t RangeType>
		struct Impl
		{
			static_assert(sizeof(EnumType) == 0, "Unknown enum type");
		};

		/**
		 * Enum range implementation, continous values.
		 */
		template<typename EnumType>
		struct Impl<EnumType, 0>
		{
			ContiguousIterator<EnumType> begin() const { return ContiguousIterator<EnumType>(Type<EnumType>::Begin); }
			ContiguousIterator<EnumType> end() const { return ContiguousIterator<EnumType>(Type<EnumType>::End); }
		};

		/**
		 * Enum range implementation, value based.
		 */
		template<typename EnumType>
		struct Impl<EnumType, 1>
		{
			ValueArrayIterator<EnumType> begin() const 
			{ 
				return ValueArrayIterator<EnumType>(Type<EnumType>::template GetPointer<void>(false)); 
			}

			ValueArrayIterator<EnumType> end() const 
			{ 
				return ValueArrayIterator<EnumType>(Type<EnumType>::template GetPointer<void>(true)); 
			}
		};

		/**
		 * TODO: NOT WORKING
		 * Enum range implementation, flag values.
		 */
        template<typename EnumType>
		struct Impl<EnumType, 2>
        {
            FlagIterator<EnumType> begin() const { return FlagIterator<EnumType>(Flags); }
            FlagIterator<EnumType> end() const { return FlagIterator<EnumType>(0); }

        private:
            std::underlying_type_t<EnumType> Flags;
        };
	}


	/**
	 * TEnumRange
	 * 
	 *  Creates the range for the enum.
	 */
	template<typename EnumType>
	struct TEnumRange : Enum::Internal::Range::Impl<EnumType, Enum::Internal::Range::Type<EnumType>::RangeType>
	{
	};

}
