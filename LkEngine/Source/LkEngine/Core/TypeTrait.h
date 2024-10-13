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
#include "LkEngine/Core/Memory/MemoryPtr.h"


namespace LkEngine::Meta {

	/**
	 * TString
	 * 
	 *  String literal applicable for templates.
	 */
	template<size_t N>
	struct TString
	{
		constexpr TString(const char(&InString)[N])
		{
			std::copy_n(InString, N, value);
		}

		operator std::string_view() const 
		{
			return std::string_view(value);
		}

		char value[N];
	};

	template <typename T>
	static std::string TypeName()
	{
		return typeid(T).name();
	};

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

	template <typename T>
	struct IsSmartPointer : std::false_type {};

	template <typename T>
	struct IsSmartPointer<TUniquePtr<T>> : std::true_type {};

	template <typename T>
	struct IsSmartPointer<TSharedPtr<T>> : std::true_type {};

	template <typename T>
	struct RemovePointerReferenceSmart 
	{
		using type = std::remove_pointer_t<std::remove_reference_t<std::remove_const_t<T>>>;
	};

	template <typename T>
	struct RemovePointerReferenceSmart<TUniquePtr<T>> 
	{
		using type = T;
	};

	template <typename T>
	struct RemovePointerReferenceSmart<TSharedPtr<T>> 
	{
		using type = T;
	};

	template <typename T>
	using RemovePointerReferenceSmart_t = typename RemovePointerReferenceSmart<T>::type;

	/** 
	 * IsBaseOf 
	 * 
	 *  Determine if type T is a derivation of TBase.
	 */
	template<typename TBase, typename T>
	struct IsBaseOf 
	{
		static constexpr bool value = std::is_base_of<TBase, RemovePointerReferenceSmart_t<T>>::value;
	};

	template <typename TBase, typename T>
	inline constexpr bool IsBaseOf_v = IsBaseOf<TBase, T>::value;
	
	/**
	 * MemberFunction
	 */
	template<bool bIsConst, typename TObject, typename TReturnValue, typename ...Args>
	struct MemberFunction;

	template<typename TObject, typename TReturnValue, typename... TArgs>
	struct MemberFunction<true, TObject, TReturnValue, TArgs...>
	{
		using Type = TReturnValue(TObject::*)(TArgs...) const;
	};

	template<typename TObject, typename TReturnValue, typename... TArgs>
	struct MemberFunction<false, TObject, TReturnValue, TArgs...>
	{
		using Type = TReturnValue(TObject::*)(TArgs...);
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

	struct FClassData
	{
		std::vector<FMemberData> Members;

		/** */
		FORCEINLINE void AddMember(std::string_view InName, std::string_view InType)
		{
			Members.push_back({ InName, InType });
		}

		/** */
		FORCEINLINE const std::vector<FMemberData>& GetMembers() const
		{
			return Members;
		}

	};

	/** 
	 * Register a member to a class metadata container. 
	 */
	template<typename TClass, typename TMemberPtr>
	constexpr void RegisterClassMember(FClassData& ClassData, 
									   std::string_view MemberName, 
									   TMemberPtr TClass::*MemberPtr)
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

}

