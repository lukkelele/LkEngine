/******************************************************************
 * LObject
 *
 *
 *******************************************************************/
#pragma once

#include <set>
#include <typeindex>
#include <typeinfo>
#include <type_traits>

#include "LkEngine/Core/CoreMacros.h"
#include "LkEngine/Core/LObject/Class.h"
#include "LkEngine/Core/LObject/ObjectBase.h"
#include "LkEngine/Core/Delegate/Delegate.h"

#include "LkEngine/Core/MetadataRegistry.h"


namespace LkEngine {

	class LMetadataRegistry;

	using FObjectHandle = LUUID;

	template<typename T>
	class TObjectPtr;

	template<typename FunctionType>
	class TFunction;

	/**
	 * LObject
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

		/** @brief Get the object handle. */
		FORCEINLINE FObjectHandle GetObjectHandle() const
		{
			return ObjectHandle;
		}

		/**
		 * @brief Initialize object.
		 */
		virtual void Initialize();

		/**
		 * @brief Check if object is initialized.
		 */
		FORCEINLINE virtual bool IsInitialized() const
		{
			return bObjectInitialized;
		}

		/**
		 * Check if object is valid for use.
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
		 * Check if object has a specific flag.
		 */
		FORCEINLINE bool HasFlag(const EObjectFlag InFlag) const
		{
			return ((ObjectFlags & InFlag) == static_cast<std::underlying_type_t<EObjectFlag>>(InFlag));
		}

		/**
		 * Check if object has any of the passed flags.
		 */
		FORCEINLINE bool HasAnyFlags(const EObjectFlag InFlags) const
		{
			return static_cast<bool>(ObjectFlags & InFlags);
		}

		/**
		 * Get static class object. 
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
		 * Register object class.
		 * 
		 *  Implemented by the LOBJECT macro (LCLASS/LSTRUCT).
		 */
		virtual const LClass* ObjectRegistration() = 0;

		/**
		 * Static class name.
		 * 
		 *  Implemented by LCLASS macro.
		 */
		FORCEINLINE static std::string StaticClassName() { return "LObject"; }

		/**
		 * Get the class for this LObject.
		 *
		 *  No null-checks should be done inside this function since it is
		 *  used to determine the class registration at places, i.e the return value
		 *  is used to determine if the class is registered depending if nullptr or not.
		 *
		 *  Implemented by LCLASS macro.
		 */
		FORCEINLINE virtual const LClass* GetClass() const
		{
			return LObjectBase::GetClass();
		}

		/**
		 * Get name of the LObject class.
		 * 
		 *  Implemented by LCLASS macro.
		 */
		virtual std::string ClassName() const = 0;

		/**
		 * Mark object as garbage.
		 */
		void MarkAsGarbage()
		{
			ObjectFlags |= EObjectFlag::Garbage;
		}

		/**
		 * Cast object to type T.
		 */
		template<typename T>
		T& As()
		{
			static_assert(sizeof(T) > 0, "As<T> failed, incomplete type");
			return static_cast<T&>(*this);
		}

		/**
		 * Cast object to type T.
		 */
		template<typename T>
		const T& As() const
		{
			static_assert(sizeof(T) > 0, "As<T> failed, incomplete type");
			return static_cast<const T&>(*this);
		}

		/**
		 * Check if object is or is a derivation of T.
		 */
		template<typename T>
		bool IsA() const
		{
			static_assert(sizeof(T) > 0, "IsA<T> failed, incomplete type");
			return (GetClass()->GetName() == T::StaticClassName()); /// TODO: CHANGE THIS
		}

		/**
		 * Check if object is considered an asset.
		 */
		FORCEINLINE virtual bool IsAsset() const { return false; }

		/**
		 * Return current reference count from all object pointers.
		 */
		FORCEINLINE uint32_t GetReferenceCount() const { return Ptr_ReferenceCount.load(); }

		template <typename T>
		static void ValidateLObjectImplementation()
		{
			static_assert(HasLClassMacro<T>, "Class must include the LCLASS macro.");
		}

	private:
		/**
		 * Increment reference count.
		 *
		 *  Managed by TObjectPtr.
		 */
		FORCEINLINE void IncrementReferenceCount() const
		{
			Ptr_ReferenceCount++;
		}

		/**
		 * Decrement reference count.
		 *
		 *  Managed by TObjectPtr.
		 */
		FORCEINLINE void DecrementReferenceCount() const
		{
			LK_CORE_ASSERT(Ptr_ReferenceCount > 0, "LObject::DecrementReferenceCount failed");
			Ptr_ReferenceCount--;
		}

	protected:
		FObjectHandle ObjectHandle = 0; /* TODO: Rename to 'ObjectHandle' */

		bool bObjectInitialized = false;
		EObjectFlag ObjectFlags = EObjectFlag::None; /* TODO: Rename 'Flags' to 'ObjectFlags' */

		/** Reference count is managed by TObjectPtr. */
		mutable std::atomic<uint32_t> Ptr_ReferenceCount = 0;

		template<typename T>
		friend class TObjectPtr;
	};

	struct FInternalLObjectUtility
	{
		FORCEINLINE static bool CheckObjectValidBasedOnItsFlags(const LObject* Object)
		{
			return !(Object->HasAnyFlags(EObjectFlag::Garbage));
		}
	};

	FORCEINLINE bool IsValid(const LObject* Object)
	{
		return (Object && FInternalLObjectUtility::CheckObjectValidBasedOnItsFlags(Object));
	}

	FORCEINLINE bool IsValid(const LObject& Object)
	{
		return IsValid(&Object);
	}

	template<typename TObject>
	TObject* GetValid(TObject* Object)
	{
		static_assert(std::is_base_of<LObject, Object>::value, "GetValid only works with LObject-derived classes");
		return (IsValid(Object) ? Object : nullptr);
	}

	template<typename TObject>
	const TObject* GetValid(const TObject* Object)
	{
		static_assert(std::is_base_of<LObject, Object>::value, "GetValid only works with LObject-derived classes");
		return (IsValid(Object) ? Object : nullptr);
	}

	template<typename TObject>
	inline constexpr bool IsBaseOfObject = std::is_base_of<LObject, std::decay_t<TObject>>::value;

}


/** Allow usage of LObject's in maps and sets. */
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
