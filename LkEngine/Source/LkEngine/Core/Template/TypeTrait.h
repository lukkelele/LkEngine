/******************************************************************
 * TypeTrait
 * 
 * Helpers for meta programming with the use of type traits.
 *******************************************************************/
#pragma once

#include <typeindex>
#include <typeinfo>
#include <type_traits>

#include <tuple>
#include <vector>
#include <string_view>

#include "LkEngine/Core/CoreMacros.h"
#include "LkEngine/Core/CoreTypes.h"


namespace LkEngine {

	/**
	 * LType
	 *
	 *  Type ID wrapper, is equal to std::type_index(typeid(T))
	 */
	template<typename T>
	struct LType
	{
		[[nodiscard]] static const std::type_index& ID()
		{
			static const std::type_index TypeIndex = std::type_index(typeid(T));
			return TypeIndex;
		}

		operator const std::type_index&() const { return ID(); }
	};

	namespace Core 
	{
		/**
		 * TStringLiteral
		 * 
		 *  Constexpr string literal.
		 */
		template<std::size_t N>
		struct TStringLiteral
		{
			constexpr TStringLiteral(const char(&InString)[N]) { std::copy_n(InString, N, value); }

			constexpr std::size_t Size() const { return N; }
			constexpr std::string_view View() const { return std::string_view(value); }

			operator std::string_view() const { return std::string_view(value); }

		public:
			char value[N];
		};

		template <typename T>
		static std::string TypeName() { return typeid(T).name(); };

		/**
		 * RemovePointerAndReference
		 * 
		 *  Remove raw pointer types.
		 */
		template<typename T>
		struct RemovePointerAndReference
		{
			using type = std::remove_pointer_t<std::remove_reference_t<T>>;
		};

		template<typename T>
		using RemovePointerAndReference_t = typename RemovePointerAndReference<T>::type;

		template<typename T>
		struct IsSmartPointer : std::false_type {};

		template<typename T>
		struct IsSmartPointer<std::unique_ptr<T>> : std::true_type {};

		template<typename T>
		struct IsSmartPointer<std::shared_ptr<T>> : std::true_type {};

		template<typename T>
		struct RemovePointerReferenceSmart { using type = std::remove_pointer_t<std::remove_reference_t<std::remove_const_t<T>>>; };

		template<typename T>
		struct RemovePointerReferenceSmart<std::unique_ptr<T>> { using type = T; };

		template<typename T>
		struct RemovePointerReferenceSmart<std::shared_ptr<T>> { using type = T; };

		template<typename T>
		using RemovePointerReferenceSmart_t = typename RemovePointerReferenceSmart<T>::type;

		/** 
		 * IsBaseOf 
		 * 
		 *  Determine if type Target is a derivation of Base.
		 */
		template<typename Base, typename Target>
		struct IsBaseOf 
		{
			static constexpr bool value = std::is_base_of<Base, RemovePointerReferenceSmart_t<Target>>::value;
		};

		template<typename TBase, typename T>
		inline constexpr bool IsBaseOf_v = IsBaseOf<TBase, T>::value;
		
		template<bool bIsConst, typename TObject, typename TReturnValue, typename... TArgs>
		struct MemberFunction;

		/**
		 * Const member function.
		 */
		template<typename TObject, typename TReturnValue, typename... TArgs>
		struct MemberFunction<true, TObject, TReturnValue, TArgs...>
		{
			using type = TReturnValue(TObject::*)(TArgs...) const;
		};

		/**
		 * Member function.
		 */
		template<typename TObject, typename TReturnValue, typename... TArgs>
		struct MemberFunction<false, TObject, TReturnValue, TArgs...>
		{
			using type = TReturnValue(TObject::*)(TArgs...);
		};

		template<typename T>
		struct MemberData;

		template<typename TClass, typename TMember>
		struct MemberData<TMember TClass::*>
		{
			using Class = TClass;
			using Type  = TMember;
		};

		struct FMemberData
		{
			std::string_view Name;
			std::string_view Type;
		};

	#if 0
		struct FClassData
		{
			std::vector<FMemberData> Members;

			void AddMember(std::string_view InName, std::string_view InType)
			{
				Members.push_back({ InName, InType });
			}

			const std::vector<FMemberData>& GetMembers() const
			{
				return Members;
			}
		};

		/** 
		 * Register a member to a class metadata container. 
		 */
		template<typename TClass, typename TMemberPtr>
		constexpr void RegisterClassMember(FClassData& ClassData, std::string_view MemberName, TMemberPtr TClass::*MemberPtr)
		{
			/* Deduce the type of the pointer-to-member. */
			using MemberType = decltype(MemberPtr); 
			ClassData.AddMember(MemberName, typeid(MemberType).name());
		}

		/** 
		 * Register multiple class members to class metadata container. 
		 */
		template<typename TClass, typename... TMemberPtrs>
		constexpr void RegisterClassMembers(FClassData& ClassData, 
											const std::tuple<TMemberPtrs...>& Members, 
											const std::array<std::string_view, 
											sizeof...(TMemberPtrs)>& MemberNames)
		{
			std::apply([&ClassData](auto&&... Member)
			{
				 ((RegisterClassMember<TClass>(ClassData, MemberNames[&Member - &std::get<0>(std::make_tuple(Member...))], Member)), ...);
			}, Members);
		}

		/** @brief Deduce argument type of a function argument. */
		template <typename TClass, typename ReturnType, typename ArgumentType>
		ArgumentType DeduceArgumentType(ReturnType(TClass::*Function)(ArgumentType))
		{
			return ArgumentType{};
		}
	#endif

		/**
		 * Check if T is a std::pair.
		 */
		template<typename T>
		constexpr bool IsPair = false;

		template<typename T1, typename T2>
		constexpr bool IsPair<std::pair<T1, T2>> = true;

		template<typename T>
		struct TPairTraits;

		template<typename T1, typename T2>
		struct TPairTraits<std::pair<T1, T2>>
		{
			using FirstType  = T1;
			using SecondType = T2;
		};

		template<typename T>
		constexpr bool IsPairWithFirstArgConstChar = false;

		template<typename T1, typename T2>
		constexpr bool IsPairWithFirstArgConstChar<std::pair<T1, T2>> = std::is_same_v<T1, const char*>;
	}

}

