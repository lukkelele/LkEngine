/**
 * @file
 * @brief LObject implementation.
 */
#pragma once

/**
 * @ingroup Core
 * @{
 *
 * @defgroup LObject
 */

#include <set>
#include <typeindex>
#include <typeinfo>
#include <type_traits>

#include "LkEngine/Core/CoreMacros.h"
#include "LkEngine/Core/LObject/Class.h"
#include "LkEngine/Core/LObject/ObjectBase.h"
#include "LkEngine/Core/MetadataRegistry.h"

namespace LkEngine {

	/**
	 * @ingroup LObject
	 * @{
	 */

	using FObjectHandle = LUUID;

	class LMetadataRegistry;

	template<typename T>
	class TObjectPtr;

	/**
	 * @class LObject
	 *
	 * @details The base object class used in LkEngine.
	 *
	 * @ingroup LObject
	 */
	class LObject : public LObjectBase
	{
	public:
		LObject();

		LObject(const LObject& Other)
			: ObjectHandle(Other.ObjectHandle)
			, bObjectInitialized(Other.bObjectInitialized)
			, ObjectFlags(Other.ObjectFlags)
			, Ptr_ReferenceCount(Other.Ptr_ReferenceCount.load())
		{
		}

		virtual ~LObject() = default;

		/**
		 * @brief Initialize object.
		 */
		virtual void Initialize();

		/**
		 * @brief Destroy object, releasing all resources.
		 */
		virtual void Destroy() {};

		/**
		 * @brief Check if object is initialized.
		 */
		FORCEINLINE virtual bool IsInitialized() const { return bObjectInitialized; }

		/** 
		 * @brief Get the object handle. 
		 */
		FORCEINLINE FObjectHandle GetObjectHandle() const { return ObjectHandle; }

		/**
		 * @brief Check if object is valid for use.
		 */
		FORCEINLINE virtual bool IsObjectValid() const
		{
			if (HasAnyFlags(EObjectFlag::Garbage | EObjectFlag::BeginDestroy | EObjectFlag::FinishDestroy))
			{
				return false;
			}

			return true;
		}

		/**
		 * @brief Get static class object. 
		 * 
		 *  Implemented for every LClass.
		 */
		FORCEINLINE static const LClass* StaticClass()
		{
			static bool bClassRegistered = false;
			if (!bClassRegistered)
			{
				LClass* ObjectClass = LClass::Register<LObject>("LObject");
				bClassRegistered = true;
			}

			return LClass::Get(LType<LObject>::ID());
		}

		/**
		 * @brief Register object class.
		 * 
		 *  Implemented by the LOBJECT macro (LCLASS/LSTRUCT).
		 */
		virtual const LClass* ObjectRegistration() = 0;

		/**
		 * @brief Static class name.
		 * 
		 *  Implemented by the LOBJECT macro (LCLASS/LSTRUCT).
		 */
		FORCEINLINE static std::string StaticClassName() { return "LObject"; }

		/**
		 * @brief Get the class for this LObject.
		 *
		 *  No null-checks should be done inside this function since it is
		 *  used to determine the class registration at places, i.e the return value
		 *  is used to determine if the class is registered depending if nullptr or not.
		 *
		 *  Implemented by the LCLASS macro.
		 */
		FORCEINLINE virtual const LClass* GetClass() const
		{
			return LObjectBase::GetClass();
		}

		/**
		 * @brief Get name of the LObject class.
		 * 
		 *  Implemented by the LCLASS macro.
		 */
		virtual std::string ClassName() const = 0;

		LObject& operator=(const LObject& Other)
		{
			if (this == &Other)
			{
				return *this;
			}

			ObjectHandle = Other.ObjectHandle;
			bObjectInitialized = Other.bObjectInitialized;
			ObjectFlags = Other.ObjectFlags;
			Ptr_ReferenceCount = Other.Ptr_ReferenceCount.load();

			return *this;
		}

		/**
		 * @brief Mark object as garbage.
		 */
		void MarkAsGarbage()
		{
			ObjectFlags |= EObjectFlag::Garbage;
		}

		/**
		 * @brief Cast object to type T.
		 */
		template<typename T>
		T& As()
		{
			static_assert(sizeof(T) > 0, "As<T> failed, incomplete type");
			return static_cast<T&>(*this);
		}

		/**
		 * @brief Cast object to type T.
		 */
		template<typename T>
		const T& As() const
		{
			static_assert(sizeof(T) > 0, "As<T> failed, incomplete type");
			return static_cast<const T&>(*this);
		}

		/**
		 * @brief Check if the object is or is a derivation of type T.
		 */
		template<typename T>
		bool IsA() const
		{
			static_assert(sizeof(T) > 0, "IsA<T> failed, incomplete type");
			return (GetClass()->GetName() == T::StaticClassName()); /// TODO: CHANGE THIS
		}

		/**
		 * @brief Check if object is an asset.
		 */
		FORCEINLINE virtual bool IsAsset() const { return false; }

		/**
		 * @brief Return current reference count from all object pointers.
		 */
		FORCEINLINE uint32_t GetReferenceCount() const { return Ptr_ReferenceCount.load(); }

		/**
		 * @brief Validate the implementation of a LObject.
		 */
		template <typename T>
		static void ValidateLObjectImplementation()
		{
			static_assert(HasLClassMacro<T>, "Class must include the LCLASS macro");
		}

	private:
		/**
		 * @brief Check if object has a specific flag.
		 */
		FORCEINLINE bool HasFlag(const EObjectFlag InFlag) const
		{
			return ((ObjectFlags & InFlag) == static_cast<std::underlying_type_t<EObjectFlag>>(InFlag));
		}

		/**
		 * @brief Check if object has any of the passed flags.
		 */
		FORCEINLINE bool HasAnyFlags(const EObjectFlag InFlags) const
		{
			return static_cast<bool>(ObjectFlags & InFlags);
		}

	private:
		/**
		 * @brief Increment reference count.
		 *
		 *  Managed by TObjectPtr.
		 */
		FORCEINLINE void IncrementReferenceCount() const
		{
			Ptr_ReferenceCount++;
		}

		/**
		 * @brief Decrement reference count.
		 *
		 *  Managed by TObjectPtr.
		 */
		FORCEINLINE void DecrementReferenceCount() const
		{
			LK_CORE_ASSERT(Ptr_ReferenceCount > 0, "LObject::DecrementReferenceCount failed");
			Ptr_ReferenceCount--;
		}

	protected:
		FObjectHandle ObjectHandle = 0;

		bool bObjectInitialized = false;
		EObjectFlag ObjectFlags = EObjectFlag::None;

		/** @variable Reference count is managed by TObjectPtr. */
		mutable std::atomic<uint32_t> Ptr_ReferenceCount = 0;

		template<typename T>
		friend class TObjectPtr;

		friend struct FInternalLObjectValidator;
	};

	/**
	 * FInternalObjectValidator
	 *
	 *  Helper for validating a LObject based on the object's flags.
	 */
	struct FInternalLObjectValidator
	{
		FORCEINLINE static bool CheckObjectValidBasedOnFlags(const LObject* Object)
		{
			return !(Object->HasAnyFlags(EObjectFlag::Garbage));
		}
	};

	FORCEINLINE bool IsValid(const LObject* Object)
	{
		return (Object && FInternalLObjectValidator::CheckObjectValidBasedOnFlags(Object));
	}

	FORCEINLINE bool IsValid(const LObject& Object)
	{
		return IsValid(&Object);
	}

	template<typename TObject>
	inline constexpr bool IsBaseOfObject = std::is_base_of<LObject, std::decay_t<TObject>>::value;

	/** @} */
}

/** @} */

/** 
 * Make LObject a usable type in maps and sets.
 */
namespace std 
{
	template<>
	struct hash<LkEngine::LObject>
	{
		std::size_t operator()(const LkEngine::LObject& Object) const noexcept
		{
			return std::hash<uint64_t>()(Object.GetObjectHandle());
		}
	};
}
