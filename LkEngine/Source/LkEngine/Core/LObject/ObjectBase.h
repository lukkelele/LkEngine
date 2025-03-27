/**
 * @file 
 * @brief LObject base implementation.
 */
#pragma once

#include <concepts>

#include "LkEngine/Core/CoreTypes.h"
#include "LkEngine/Core/Log/Log.h"

#include "Enum.h"

namespace LkEngine {
	
	class LClass;

	enum class EInitFlag
	{
		NoInit = 0,  /* Do not initialize the object. */
		True,        /* Initialize the object. */
	};

	/**
	 * @enum LObjectFlag 
	 *
	 * @details Used for object initialization, loading and garbage collection.
	 */
	enum class EObjectFlag : uint32_t
	{
		None = 0,              /* No flags. */
		NeedInitialization,    /* Need to be initialized. */
		NeedLoad,              /* Needs to get loaded to memory. */
		BeginDestroy,          /* Begin object destruction. */
		FinishDestroy,         /* Object destroyed. */
		Garbage,               /* Object deemed garbage and should get deleted. */
	};
	LK_ENUM_CLASS(EObjectFlag);


	/** @todo: Implement this. */
	template<typename TObject>
	concept LObjectCore = requires(TObject Object)
	{
		{ Object.GetName() } -> std::same_as<std::string>;
	};

	/**
	 * @class LObjectBase
	 *
	 * @details Base class implemented by LObject.
	 *          Holds a LClass reference.
	 *
	 * @ingroup LObject
	 */
	class LObjectBase
	{
	public:
		/** 
		 * @brief Check if the object is the same type as the passed object's.
		 */
		template<typename OtherClassType>
		FORCEINLINE bool IsA(OtherClassType OtherObject) const
		{
			const LClass* OtherClass = OtherObject;
			LK_CORE_ASSERT(OtherClass, "IsA failed, other class object is nullptr");

			const LClass* ThisClass = GetClass();

			/* Stop the compiler doing unnecessary branching for nullptr checks. */
			LK_ASSUME(OtherClass);
			LK_ASSUME(ThisClass);

			return IsChildOf(ThisClass, OtherClass);
		}

		/**
		 * @brief Check if the object is a child of another class.
		 * @note Nullchecked.
		 */
		template<typename ClassType>
		static FORCEINLINE bool IsChildOf(const ClassType* InObjectClass, const ClassType* InOtherClass)
		{
			LK_CORE_ASSERT(InObjectClass);
			return InObjectClass->IsChildOf(InOtherClass);
		}

	protected:
		/**
		 * @brief Set the class.
		 * @details Invoked once in the LCLASS declaration for an LClass object. 
		 *          Throws error otherwise.
		 */
		void SetClass(LClass* InClass);

		/**
		 * @brief Get the private class member.
		 */
		FORCEINLINE const LClass* GetClass() const
		{
			return ClassPrivate;
		}

		/**
		 * @brief Check if LObjectBase has a valid LClass object.
		 * @details Used as a validity check during object construction.
		 */
		FORCEINLINE bool IsClassValid() const
		{
			return (ClassPrivate != nullptr);
		}

	private:
		/**
		 * @variable LClass reference.
		 */
		LClass* ClassPrivate = nullptr;
	};
	/** @} */

	/** 
	 * Helper to detect a member function. 
	 */
	template<typename, typename = void>
	struct HasGetClass : std::false_type {};

	template<typename T>
	struct HasGetClass<T, std::void_t<decltype(std::declval<T>().GetClass())>> : std::true_type {};

	/**
	 * @concept HasLClassMacro
	 */
	template<typename T>
	concept HasLClassMacro = requires 
	{
		{ T::StaticClassName() } -> std::convertible_to<std::string_view>;
		{ T::StaticClass() } -> std::same_as<const LClass*>;
		{ std::declval<T>().ClassName() } -> std::convertible_to<std::string>;
		{ std::declval<T>().ClassRegistration() } -> std::same_as<const LClass*>;
		typename T::BaseClass;
		typename T::ThisClass;
	};

}
