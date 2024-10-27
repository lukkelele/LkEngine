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
#include "LkEngine/Core/LObject/LClass.h"
#include "LkEngine/Core/LObject/LObjectBase.h"
#include "LkEngine/Core/String.h"
#include "LkEngine/Core/Delegate/Delegate.h"

#include "LkEngine/Core/MetadataRegistry.h"


namespace LkEngine {

	class LMetadataRegistry;

	using FObjectHandle = UUID;

	template<typename T>
	class TObjectPtr;

	template<typename FunctionType>
	class TFunction;

	/**
	 * LObject
	 */
	class LObject
	{
		LK_DECLARE_MULTICAST_DELEGATE(FObjectDestructBegin, const FObjectHandle&);
		LK_DECLARE_MULTICAST_DELEGATE(FObjectDestructEnd, const FObjectHandle&);
		
	public:
		LObject();

		LObject(const LObject& Other)
			: Handle(Other.Handle)
			, bInitialized(Other.bInitialized)
			, Flags(Other.Flags)
			, Name(Other.Name)
		{
		}

		virtual ~LObject() = default;

		LObject& operator=(const LObject& Other)
		{
			if (this == &Other)
			{
				return *this;
			}

			Handle = Other.Handle;
			bInitialized = Other.bInitialized;
			Flags = Other.Flags;
			Name = Other.Name;
			ReferenceCount = Other.ReferenceCount.load();

			return *this;
		}

		/** @brief Get the object handle. */
		FORCEINLINE FObjectHandle GetHandle() const 
		{ 
			return Handle; 
		}

		/**
		 * @brief Initialize object.
		 */
		virtual void Initialize();

		/**
		 * @brief Destroy object and release from memory.
		 */
		virtual void BeginDestroy();

		/**
		 * @brief Check if object is initialized.
		 */
		FORCEINLINE virtual bool IsInitialized() const 
		{ 
			return bInitialized; 
		}

		/**
		 * @brief Check if object is valid for use.
		 */
		FORCEINLINE virtual bool IsValid() const
		{
			if (HasAnyFlags(EObjectFlag::Garbage | EObjectFlag::BeginDestroy | EObjectFlag::FinishDestroy))
			{
				return false;
			}

			return true;
		}

		/**
		 * @brief Check if object has a specific flag.
		 */
		FORCEINLINE bool HasFlag(const LObjectFlag InFlag) const
		{
			return ((Flags & InFlag) == InFlag);
		}

		/**
		 * @brief Check if object has any of the passed flags.
		 */
		FORCEINLINE bool HasAnyFlags(const LObjectFlag InFlags) const
		{
			return (Flags & InFlags);
		}

		/**
		 * @brief Get static class.
		 */
		FORCEINLINE static const LClass* StaticClass() 
		{ 
			static const bool bClassRegistered = (LClass::Register<LObject>("LObject"), true);
			return LClass::Get(LType<LObject>::ID());
		}

		/**
		 * @brief Static class name, implemented by LCLASS macro.
		 */
		FORCEINLINE static std::string StaticClassName() { return "LObject"; }

		/** 
		 * @brief Return LClass object for this class.
		 */
		virtual const LClass* GetClass() const = 0;

		/** 
		 * @brief Name of class, implemented by LCLASS macro.
		 */
		virtual std::string ClassName() const = 0;

		/** 
		 * @brief Get name of object. 
		 */
		FORCEINLINE std::string_view GetName() const { return Name; }

		/**
		 * @brief Check to see if object is selected.
		 */
		bool IsSelected() const;

		/**
		 * @brief Serialize object. 
		 */
		virtual LString Serialize() const { return ""; }

		/**
		 * @brief Mark object as garbage.
		 */
		FORCEINLINE void MarkAsGarbage()
		{
			//Flags |= static_cast<decltype(Flags)>(EObjectFlag::Garbage);
			Flags |= EObjectFlag::Garbage;
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
		 * @brief Check if object is or is a derivation of T.
		 */
		template<typename T>
		bool IsA() const
		{
			static_assert(sizeof(T) > 0, "IsA<T> failed, incomplete type");
			//return (StaticClassName() == T::StaticClassName());
			return (StaticClass() == T::StaticClass());
		}

		/**
		 * @brief Check if object is considered an asset.
		 */
		FORCEINLINE virtual bool IsAsset() const { return false; }

		/**
		 * @brief Return current reference count from all smart pointers. 
		 */
		FORCEINLINE uint32_t GetReferenceCount() const { return ReferenceCount.load(); }

	protected:
		/**
		 * @brief End stage of object destruction.
		 */
		virtual void FinalizeDestruction();

	private:
		/**
		 * @brief Increment reference count.
		 * Managed by TObjectPtr.
		 */
		FORCEINLINE void IncrementReferenceCount() const 
		{ 
			ReferenceCount++; 
		}

		/**
		 * @brief Decrement reference count.
		 * Managed by TObjectPtr.
		 */
		FORCEINLINE void DecrementReferenceCount() const 
		{ 
			ReferenceCount--; 
		}

	protected:
		FObjectHandle Handle = 0;

		bool bInitialized = false;
		LObjectFlag Flags = EObjectFlag::None;

		/* Runtime name. */
		std::string Name{};

		FObjectDestructBegin OnDestructBegin{};
		FObjectDestructEnd OnDestructEnd{};

		/** Reference count is managed by TObjectPtr. */
		mutable std::atomic<uint32_t> ReferenceCount = 0;

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

	/**
	 * GIsObjectSelectedInEditor
	 *
	 *  Check to see if an object is selected.
	 */
	extern TFunction<bool(const LObject*)> GIsObjectSelectedInEditor;

	FORCEINLINE bool IsValid(const LObject* Object)
	{
		return (Object && FInternalLObjectUtility::CheckObjectValidBasedOnItsFlags(Object));
	}

	FORCEINLINE bool IsValid(const LObject& Object)
	{
		return IsValid(&Object);
	}

	template <typename TObject>
	TObject* GetValid(TObject* Object)
	{
		static_assert(std::is_base_of<LObject, Object>::value, "GetValid only works with LObject-derived classes");
		return (IsValid(Object) ? Object: nullptr);
	}

	template <typename TObject>
	const TObject* GetValid(const TObject* Object)
	{
		static_assert(std::is_base_of<LObject, Object>::value, "GetValid only works with LObject-derived classes");
		return (IsValid(Object) ? Object : nullptr);
	}

	template<typename TObject>
	inline constexpr bool IsBaseOfObject = std::is_base_of<LObject, std::decay_t<TObject>>::value;

	/// TODO: Move to garbage collector or some other registry structure.
	LK_DECLARE_MULTICAST_DELEGATE(FOnObjectCreated, const LObject*);
	extern FOnObjectCreated GOnObjectCreated;

}