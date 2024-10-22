/******************************************************************
 * LObject
 *
 *
 *******************************************************************/
#pragma once

#include <typeindex>
#include <typeinfo>
#include <type_traits>

#include "LkEngine/Core/CoreMacros.h"
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

	class LObject
	{
		LK_DECLARE_MULTICAST_DELEGATE(FObjectDestructBegin, const FObjectHandle&);
		LK_DECLARE_MULTICAST_DELEGATE(FObjectDestructEnd, const FObjectHandle&);
		LK_DECLARE_MULTICAST_DELEGATE(FObjectDestroyed, const FObjectHandle&);
		
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

			Flags = Other.Flags;

			return *this;
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
		 * GetStaticClass, implemented by LCLASS macro.
		 */
		FORCEINLINE static std::string StaticClass() { return "LObject"; }

		/** 
		 * @brief Get name of object. 
		 */
		FORCEINLINE std::string_view GetName() const { return Name; }

		/**
		 * @brief Check to see if object is selected.
		 * Applies to editor and runtime game layer.
		 */
		bool IsSelected() const;

		/**
		 * @brief Serialize object. 
		 */
		virtual LString Serialize() const { return ""; }

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
			return (StaticClass() == T::StaticClass());
		}

		/**
		 * @brief Return current reference count from all smart pointers. 
		 */
		FORCEINLINE uint32_t GetReferenceCount() const 
		{ 
			return ReferenceCount.load(); 
		}

	protected:
		/**
		 * @brief End stage of object destruction, called last on destruction.
		 */
		virtual void FinalizeDestruction();

	private:
		FORCEINLINE void IncrementReferenceCount() const { ReferenceCount++; }
		FORCEINLINE void DecrementReferenceCount() const { ReferenceCount--; }
	protected:
		FObjectHandle Handle = 0;

		bool bInitialized = false;
		LObjectFlag Flags = EObjectFlag::None;

		/* Runtime object name. */
		std::string Name{};

		// Delegate
		FObjectDestructBegin OnDestructBegin;
		FObjectDestructEnd OnDestructEnd;
		FObjectDestroyed OnObjectDestroyed;
		// ~Delegate

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

}