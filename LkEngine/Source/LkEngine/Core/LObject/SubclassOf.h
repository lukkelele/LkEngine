/******************************************************************
 * SubclassOf
 *
 *******************************************************************/
#pragma once

#include "LkEngine/Core/Template/LkTemplate.h"
#include "LkEngine/Core/LObject/LClass.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"


namespace LkEngine {

	template <typename T>
	class TSubclassOf;

	template <typename T>
	struct TIsTSubclassOf
	{
		enum { Value = false };
	};

	template <typename T> struct TIsTSubclassOf<               TSubclassOf<T>> { enum { Value = true }; };
	template <typename T> struct TIsTSubclassOf<const          TSubclassOf<T>> { enum { Value = true }; };
	template <typename T> struct TIsTSubclassOf<      volatile TSubclassOf<T>> { enum { Value = true }; };
	template <typename T> struct TIsTSubclassOf<const volatile TSubclassOf<T>> { enum { Value = true }; };

	template <typename T>
	class TSubclassOf
	{
	public:
		TSubclassOf() = default;
		TSubclassOf(TSubclassOf&&) = default;
		TSubclassOf(const TSubclassOf&) = default;
		~TSubclassOf() = default;

		TSubclassOf& operator=(TSubclassOf&&) = default;
		TSubclassOf& operator=(const TSubclassOf&) = default;

		FORCEINLINE TSubclassOf(LClass* From)
			: Class(From)
		{
		}

		/**
		 * @brief Create TSubclassOf instance from any type that can be implicitly converted to LClass*.
		 * 
		 * This constructor enables implicit conversion of an input object to the `LClass*` base pointer type,
		 * provided the input type is not already a `TSubclassOf`. 
		 * Doing this allows the safe initialization of `TSubclassOf` with objects derived from `LClass`.
		 * It restricts unsafe conversions, such as downcasting or conversions from unrelated types.
		 * 
		 * @tparam R   The type of the object used to initialize the `TSubclassOf` instance.
		 * @param From An object of type R that is implicitly convertible to `LClass*`.
		 *             Commonly used for initializing `TSubclassOf` with derived types of `LClass`.
		 */
		template<typename R, 
				std::enable_if_t<
					!TIsTSubclassOf<std::decay_t<R>>::Value, decltype(ImplicitConv<LClass*>(std::declval<R>())) 
				>* = nullptr>
			FORCEINLINE TSubclassOf(R&& From)
			: Class(From)
		{
		}

		/** @brief Assign from a LClass pointer. */
		FORCEINLINE TSubclassOf& operator=(LClass* From)
		{
			Class = From;
			return *this;
		}

		/**
		 * @brief Assigns a compatible object to the TSubclassOf instance, enabling implicit conversion to LClass*.
		 *
		 * Allows an object of type `R` to be assigned to `TSubclassOf`.
		 * Provided that `R` can be implicitly converted to `LClass*` and is not already a `TSubclassOf` type. 
		 * This enables safe assignment of derived `LClass` objects or other compatible types to `TSubclassOf`, 
		 * preventing unsafe downcasts or conversions from unrelated types.
		 *
		 * @tparam R   The type of the object being assigned to this `TSubclassOf` instance.
		 * @param From An object of type `R` that is implicitly convertible to `LClass*`.
		 *             Commonly used to assign a derived class of `LClass` or another compatible object.
		 * @return     A reference to this `TSubclassOf` instance.
		 */
		template<typename R,
				std::enable_if_t<
					!TIsTSubclassOf<std::decay_t<R>>::Value, decltype(ImplicitConv<LClass*>(std::declval<R>()))
				>* = nullptr
		>
		FORCEINLINE TSubclassOf& operator=(R&& From)
		{
			Class = From;
			return *this;
		}

		/** @brief Dereference back into a LClass pointer. */
		FORCEINLINE LClass* operator*() const
		{
			if (!Class || !Class->IsChildOf(T::StaticClass()))
			{
				return nullptr;
			}

			return Class;
		}

		/** @brief Dereference back into a LClass pointer. */
		FORCEINLINE LClass* Get() const
		{
			return **this;
		}

		/** @brief Dereference back into a LClass pointer. */
		FORCEINLINE LClass* operator->() const
		{
			return **this;
		}

		/** @brief Implicit conversion to LClass pointer. */
		FORCEINLINE operator LClass* () const
		{
			return **this;
		}

	private:
		/** @brief Class object reference. */
		TObjectPtr<LClass> Class = nullptr;

		template <typename R>
		friend class TSubclassOf;
	};

}
