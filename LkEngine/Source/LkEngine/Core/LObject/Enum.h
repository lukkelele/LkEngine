/******************************************************************
 * Enum
 * 
 *******************************************************************/
#pragma once

/**  
 * TODO: 
 *  1) Need to make it possible to use macros like FORCEINLINE from anywhere. 
 *     Currently causes errors because 'Enum.h' is included in 'CoreMacros.h'.
 *  2) Tests for enum ranges, need to verify that flag values don't get stuck
 *     in infinite loops because of left shifts for unsigned integers. 
 *  3) Namespace nesting for enum range declarations.
 *  4) Use __VA_OPT__ to use the enum's COUNT value if no count value 
 *     is passed in LK_ENUM_RANGE_BY_COUNT.
 */
#ifndef FORCEINLINE
#define FORCEINLINE	__forceinline
#endif

#include <typeinfo>
#include <type_traits>

#define LK_INTERNAL_ENUM_RANGE_TYPE_CONTIGIOUS 0
#define LK_INTERNAL_ENUM_RANGE_TYPE_VALUEARRAY 1
#define LK_INTERNAL_ENUM_RANGE_TYPE_FLAG       2

namespace LkEngine {

	/** 
	 * LK_ENUM_CLASS
	 *
	 *  Adds operators to an enum class so it can be used as a regular enum.
	 */
	#define LK_ENUM_CLASS(EnumClass) \
		using LK_Enum_##EnumClass = std::underlying_type_t<EnumClass>; \
		inline           EnumClass& operator|=(EnumClass& Lhs, EnumClass Rhs) { return Lhs = (EnumClass)(static_cast<LK_Enum_##EnumClass>(Lhs) | static_cast<LK_Enum_##EnumClass>(Rhs)); } \
		inline           EnumClass& operator&=(EnumClass& Lhs, EnumClass Rhs) { return Lhs = (EnumClass)(static_cast<LK_Enum_##EnumClass>(Lhs) & static_cast<LK_Enum_##EnumClass>(Rhs)); } \
		inline           EnumClass& operator^=(EnumClass& Lhs, EnumClass Rhs) { return Lhs = (EnumClass)(static_cast<LK_Enum_##EnumClass>(Lhs) ^ static_cast<LK_Enum_##EnumClass>(Rhs)); } \
		inline constexpr EnumClass  operator| (EnumClass  Lhs, EnumClass Rhs) { return (EnumClass)(static_cast<LK_Enum_##EnumClass>(Lhs) | static_cast<LK_Enum_##EnumClass>(Rhs)); } \
		inline constexpr LK_Enum_##EnumClass operator& (EnumClass Lhs, EnumClass Rhs) { return (LK_Enum_##EnumClass)(static_cast<LK_Enum_##EnumClass>(Lhs) & static_cast<LK_Enum_##EnumClass>(Rhs)); } \
		inline constexpr EnumClass  operator^ (EnumClass  Lhs, EnumClass Rhs) { return (EnumClass)(static_cast<LK_Enum_##EnumClass>(Lhs) ^ static_cast<LK_Enum_##EnumClass>(Rhs)); } \
		inline constexpr bool       operator! (EnumClass  E)                  { return !static_cast<LK_Enum_##EnumClass>(E); } \
		inline constexpr EnumClass  operator~ (EnumClass  E)                  { return (EnumClass)~static_cast<LK_Enum_##EnumClass>(E); } \
		inline constexpr LK_Enum_##EnumClass operator&(LK_Enum_##EnumClass Lhs, EnumClass Rhs) { return static_cast<LK_Enum_##EnumClass>(Lhs & static_cast<LK_Enum_##EnumClass>(Rhs)); } \
		inline constexpr LK_Enum_##EnumClass operator&(EnumClass Lhs, LK_Enum_##EnumClass Rhs) { return static_cast<LK_Enum_##EnumClass>(static_cast<LK_Enum_##EnumClass>(Lhs) & Rhs); } \
		inline constexpr LK_Enum_##EnumClass operator|(LK_Enum_##EnumClass Lhs, EnumClass Rhs) { return static_cast<LK_Enum_##EnumClass>(Lhs | static_cast<LK_Enum_##EnumClass>(Rhs)); } \
		inline constexpr LK_Enum_##EnumClass operator|(EnumClass Lhs, LK_Enum_##EnumClass Rhs) { return static_cast<LK_Enum_##EnumClass>(static_cast<LK_Enum_##EnumClass>(Lhs) | Rhs); } \
		inline constexpr LK_Enum_##EnumClass operator^(LK_Enum_##EnumClass Lhs, EnumClass Rhs) { return static_cast<LK_Enum_##EnumClass>(Lhs ^ static_cast<LK_Enum_##EnumClass>(Rhs)); } \
		inline constexpr LK_Enum_##EnumClass operator^(EnumClass Lhs, LK_Enum_##EnumClass Rhs) { return static_cast<LK_Enum_##EnumClass>(static_cast<LK_Enum_##EnumClass>(Lhs) ^ Rhs); } \
		inline constexpr bool operator<(EnumClass Lhs, LK_Enum_##EnumClass Rhs)  { return static_cast<LK_Enum_##EnumClass>(static_cast<LK_Enum_##EnumClass>(Lhs) < Rhs); }  \
		inline constexpr bool operator<(LK_Enum_##EnumClass Lhs, EnumClass Rhs)  { return static_cast<LK_Enum_##EnumClass>(Lhs < static_cast<LK_Enum_##EnumClass>(Rhs)); }  \
		inline constexpr bool operator>(EnumClass Lhs, LK_Enum_##EnumClass Rhs)  { return static_cast<LK_Enum_##EnumClass>(static_cast<LK_Enum_##EnumClass>(Lhs) > Rhs); }  \
		inline constexpr bool operator>(LK_Enum_##EnumClass Lhs, EnumClass Rhs)  { return static_cast<LK_Enum_##EnumClass>(Lhs > static_cast<LK_Enum_##EnumClass>(Rhs)); }  \
		inline constexpr bool operator<=(EnumClass Lhs, LK_Enum_##EnumClass Rhs) { return static_cast<LK_Enum_##EnumClass>(static_cast<LK_Enum_##EnumClass>(Lhs) <= Rhs); } \
		inline constexpr bool operator<=(LK_Enum_##EnumClass Lhs, EnumClass Rhs) { return static_cast<LK_Enum_##EnumClass>(Lhs <= static_cast<LK_Enum_##EnumClass>(Rhs)); } \
		inline constexpr bool operator>=(EnumClass Lhs, LK_Enum_##EnumClass Rhs) { return static_cast<LK_Enum_##EnumClass>(static_cast<LK_Enum_##EnumClass>(Lhs) >= Rhs); } \
		inline constexpr bool operator>=(LK_Enum_##EnumClass Lhs, EnumClass Rhs) { return static_cast<LK_Enum_##EnumClass>(Lhs >= static_cast<LK_Enum_##EnumClass>(Rhs)); } \
		inline LK_Enum_##EnumClass& operator|=(LK_Enum_##EnumClass& Lhs, EnumClass Rhs) { return Lhs = (LK_Enum_##EnumClass)(Lhs | static_cast<LK_Enum_##EnumClass>(Rhs)); } \
		inline LK_Enum_##EnumClass& operator&=(LK_Enum_##EnumClass& Lhs, EnumClass Rhs) { return Lhs = (LK_Enum_##EnumClass)(Lhs & static_cast<LK_Enum_##EnumClass>(Rhs)); } \
		inline EnumClass& operator&=(EnumClass& Lhs, LK_Enum_##EnumClass Rhs) { return Lhs = (EnumClass)(static_cast<LK_Enum_##EnumClass>(Lhs) & Rhs); } \
		inline EnumClass& operator|=(EnumClass& Lhs, LK_Enum_##EnumClass Rhs) { return Lhs = (EnumClass)(static_cast<LK_Enum_##EnumClass>(Lhs) | Rhs); } 


	/**
	 * LK_ENUM_RANGE_BY_FIRST_AND_LAST
	 */
	#define LK_ENUM_RANGE_BY_FIRST_AND_LAST(EnumType, First, Last) \
		template<> \
		struct ::LkEngine::Enum::Internal::Range::Type<EnumType> \
		{ \
			enum { RangeType = 0 }; \
			static constexpr std::underlying_type_t<EnumType> Begin = (std::underlying_type_t<EnumType>)(First); \
			static constexpr std::underlying_type_t<EnumType> End   = (std::underlying_type_t<EnumType>)(Last) + 1; \
		};

	/**
	 * LK_ENUM_RANGE_BY_VALUES
	 */
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

	/**
	 * LK_ENUM_RANGE_BY_COUNT
	 * 
	 *  Requires the passed EnumType's initial value to be 0.
	 */
	#define LK_ENUM_RANGE_BY_COUNT(EnumType, Count) \
		LK_ENUM_RANGE_BY_FIRST_AND_LAST(EnumType, 0, (std::underlying_type_t<EnumType>)Count - 1)

	/**
	 * LK_ENUM_RANGE_FLAGS_BY_COUNT
	 */
	#define LK_ENUM_RANGE_FLAGS_BY_COUNT(EnumType) \
	template<> \
    struct ::LkEngine::Enum::Internal::Range::Type<EnumType> \
    { \
        enum { RangeType = 2 }; \
        static constexpr std::underlying_type_t<EnumType> ValidFlags() \
        { \
            using IntType = std::underlying_type_t<EnumType>; \
            IntType Flags = 0; \
            for (IntType Idx = 1; Idx < static_cast<IntType>(EnumType::COUNT); Idx <<= 1) \
            { \
                Flags |= Idx; \
            } \
            return Flags; \
        } \
    };

	/**
	 * LK_ENUM_RANGE_FLAGS_BY_FIRST_AND_LAST
	 */
	#define LK_ENUM_RANGE_FLAGS_BY_FIRST_AND_LAST(EnumType, First, Last) \
	template<> \
    struct ::LkEngine::Enum::Internal::Range::Type<EnumType> \
    { \
        enum { RangeType = 2 }; \
        static constexpr std::underlying_type_t<EnumType> ValidFlags() \
        { \
            using IntType = std::underlying_type_t<EnumType>; \
            IntType Flags = static_cast<IntType>(First); \
            for (IntType Idx = Flags; Idx <= static_cast<IntType>(Last); Idx <<= 1) \
            { \
                Flags |= Idx; \
            } \
            return Flags; \
        } \
    };

	namespace Enum::Internal::Range
	{
		/**
		 * Enum range traits.
		 *  0: Continous.
		 *  1: Value based.
		 *  2: Flag based.
		 */
		template<typename EnumType>
		struct Type { enum { RangeType = -1 }; };

		template<typename EnumType>
		struct TContiguousIterator
		{
			using IntType = std::underlying_type_t<EnumType>;

			FORCEINLINE explicit TContiguousIterator(IntType InValue) : Value(InValue) {}

			FORCEINLINE TContiguousIterator& operator++()
			{
				++Value;
				return *this;
			}

			FORCEINLINE EnumType operator*() const { return (EnumType)Value; }

		private:
			FORCEINLINE friend bool operator!=(const TContiguousIterator& Lhs, const TContiguousIterator& Rhs)
			{
				return (Lhs.Value != Rhs.Value);
			}

		private:
			IntType Value;
		};

		template<typename EnumType>
		struct TValueArrayIterator
		{
			FORCEINLINE explicit TValueArrayIterator(const EnumType* InPtr) : Ptr(InPtr) {}

			FORCEINLINE TValueArrayIterator& operator++()
			{
				++Ptr;
				return *this;
			}

			FORCEINLINE EnumType operator*() const { return *Ptr; }

		private:
			FORCEINLINE friend bool operator!=(const TValueArrayIterator& Lhs, const TValueArrayIterator& Rhs)
			{
				return (Lhs.Ptr != Rhs.Ptr);
			}

		private:
			const EnumType* Ptr;
		};

		/**
		 * 
		 */
		template<typename EnumType>
        struct TFlagIterator
        {
            using IntType = std::underlying_type_t<EnumType>;

            FORCEINLINE explicit TFlagIterator(IntType Flags) : RemainingFlags(Flags) {}

            FORCEINLINE TFlagIterator& operator++()
            {
				/* Clear the lowest set bit. */
                RemainingFlags &= (RemainingFlags - 1);
                return *this;
            }

            FORCEINLINE EnumType operator*() const
            {
				/* Extract the lowest set bit. */
                IntType Flag = RemainingFlags & -RemainingFlags;
                return static_cast<EnumType>(Flag);
            }

		private:
            friend bool operator!=(const TFlagIterator& Lhs, const TFlagIterator& Rhs)
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
		struct Impl<EnumType, LK_INTERNAL_ENUM_RANGE_TYPE_CONTIGIOUS>
		{
			TContiguousIterator<EnumType> begin() const { return TContiguousIterator<EnumType>(Type<EnumType>::Begin); }
			TContiguousIterator<EnumType> end()   const { return TContiguousIterator<EnumType>(Type<EnumType>::End); }
		};

		/**
		 * Enum range implementation, value based.
		 */
		template<typename EnumType>
		struct Impl<EnumType, LK_INTERNAL_ENUM_RANGE_TYPE_VALUEARRAY>
		{
			TValueArrayIterator<EnumType> begin() const { return TValueArrayIterator<EnumType>(Type<EnumType>::template GetPointer<void>(false)); }
			TValueArrayIterator<EnumType> end()   const { return TValueArrayIterator<EnumType>(Type<EnumType>::template GetPointer<void>(true)); }
		};

		/**
		 * TODO: NOT WORKING
		 * Enum range implementation, flag values.
		 */
        template<typename EnumType>
		struct Impl<EnumType, LK_INTERNAL_ENUM_RANGE_TYPE_FLAG>
        {
			TFlagIterator<EnumType> begin() const { return TFlagIterator<EnumType>(Type<EnumType>::ValidFlags()); }
			TFlagIterator<EnumType> end()   const { return TFlagIterator<EnumType>(0); }
        };
	}

	/**
	 * TEnumRange
	 * 
	 *  Range implementation for the enum.
	 *  The specified enum uses the Enum::Internal::Range::Type struct that is created
	 *  with the use of a LK_ENUM_RANGE macro.
	 */
	template<typename EnumType>
	struct TEnumRange : Enum::Internal::Range::Impl<EnumType, Enum::Internal::Range::Type<EnumType>::RangeType>
	{
	};

}
