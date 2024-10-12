/******************************************************************
 * LObject
 *
 *
 *******************************************************************/
#pragma once

#include <typeindex>
#include <typeinfo>
#include <type_traits>

#include "LkEngine/Core/String.h"
#include "LkEngine/Core/LObject/LObjectBase.h"
#include "LkEngine/Core/CoreMacros.h"

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

		virtual void Initialize();

		/* TODO */
		virtual LString Serialize() const 
		{ 
			return ""; 
		}

		bool IsSelected() const;

		FORCEINLINE bool HasAnyFlags(const LObjectFlag InFlags) const
		{
			return (Flags & InFlags);
		}

		FORCEINLINE bool IsInitialized() const { return bInitialized; }

		/**
		 * GetStaticClass
		 *
		 *  Implemented by LCLASS macro.
		 */
		virtual std::string GetStaticClass() const { return "LObject"; }

		FORCEINLINE std::string_view GetName() const { return Name; }

		template<typename T>
		T& As()
		{
			/* TODO: static assert, validate T is derived type */
			return static_cast<T>(*this);
		}

		template<typename T>
		const T& As() const
		{
			/* TODO: static assert, validate T is derived type */
			return static_cast<T>(*this);
		}

		LObject& operator=(const LObject& Other)
		{
			if (this == &Other)
			{
				return *this;
			}

			Flags = Other.Flags;

			return *this;
		}

		FORCEINLINE uint32_t GetReferenceCount() const { return ReferenceCount.load(); }

	private:
		/* FIXME: Make these private and only accessable through TObjectPtr. */
		FORCEINLINE void IncrementReferenceCount() const { ReferenceCount++; }
		FORCEINLINE void DecrementReferenceCount() const { ReferenceCount--; }

	protected:
		FObjectHandle Handle = 0;

		bool bInitialized = false;
		LObjectFlag Flags = ObjectFlag::None;

		/** Reference count is managed by TObjectPtr. */
		mutable std::atomic<uint32_t> ReferenceCount = 0;

		/* Runtime name. */
		std::string Name{};

		template<typename T>
		friend class TObjectPtr;
	};

	struct FInternalLObjectUtility
	{
		FORCEINLINE static bool CheckObjectValidBasedOnItsFlags(const LObject* Object)
		{
			return !(Object->HasAnyFlags(ObjectFlag::Garbage));
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