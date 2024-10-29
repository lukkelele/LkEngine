/******************************************************************
 * LObjectBase
 * 
 *
 *******************************************************************/
#pragma once

#include <concepts>

#include "LkEngine/Core/CoreTypes.h"
#include "LkEngine/Core/String.h"

#include "Enum.h"


namespace LkEngine {
	
	class LClass;

	enum class EInitFlag
	{
		NoInit = 0,  /* Do not initialize object. */
		True,        /* Initialize object. */
	};

	/**
	 * LObjectFlag 
	 * 
	 *  Used for object initialization, loading and garbage collection.
	 */
	using LObjectFlag = uint32_t;

	namespace EObjectFlag
	{
		enum Flag : LObjectFlag
		{
			None = 0,              /* No flags. */
			NeedInitialization,    /* Need to be initialized. */
			NeedLoad,              /* Needs to get loaded to memory. */
			BeginDestroy,          /* Begin object destruction. */
			FinishDestroy,         /* Object destroyed. */
			Garbage,               /* Object deemed garbage and should get deleted. */
		};
	}

	/**
	 * LObjectBase
	 */
	class LObjectBase
	{
	public:
		/** 
		 * @brief Returns true if this object is of the specified type. 
		 */
		template <typename OtherClassType>
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

		template <typename ClassType>
		static FORCEINLINE bool IsChildOf(const ClassType* InObjectClass, const ClassType* InTestClass)
		{
			return InObjectClass->IsChildOf(InTestClass);
		}

	protected:
		/**
		 * @brief Set the class, should be run once in the LCLASS declaration for an LClass object.
		 */
		FORCEINLINE void SetClass(LClass* InClass)
		{
			LK_CORE_ASSERT(InClass, "Class object is nullptr");
			LK_VERIFY(ClassPrivate == nullptr, "SetClass called multiple times");
			ClassPrivate = InClass;
		}

		/**
		 * @brief Get the private class member.
		 */
		FORCEINLINE const LClass* GetClass() const
		{
			return ClassPrivate;
		}

		/**
		 * @brief Check if LObjectBase has a valid LClass object.
		 * To be used as a validity check during object construction.
		 */
		FORCEINLINE bool IsClassValid() const
		{
			return (ClassPrivate != nullptr);
		}

	private:
		LClass* ClassPrivate = nullptr;
	};


	/// TODO: Implement this
	template<typename TObject>
	concept HasCoreObjectFunctionality = requires(TObject Object)
	{
		Object.GetName();
	};

}
