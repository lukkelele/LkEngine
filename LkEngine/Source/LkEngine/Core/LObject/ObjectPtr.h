/******************************************************************
 * LObjectPtr
 * 
 *******************************************************************/
#pragma once

#include <unordered_set>

#include "LkEngine/Core/CoreMacros.h"
#include "LkEngine/Core/LObject/Object.h"


namespace LkEngine {

	/**
	 * TObjectPtr_Internal
	 * 
	 *  Management of live references.
	 *  For internal use by TObjectPtr.
	 */
	namespace TObjectPtr_Internal
	{
		inline static std::mutex LiveReferenceMutex;
		inline static std::unordered_set<void*> LiveReferences{};

		static void AddToLiveReferences(void* ObjectInstance);
		static void RemoveFromLiveReferences(void* ObjectInstance);
		static bool IsLive(void* ObjectInstance);
	}

	/**
	 * TObjectPtr
	 * 
	 *  Smart pointer implementation for LObject derived classes.
	 */
	template<typename T>
	class TObjectPtr
	{
	public:
		TObjectPtr(T* Instance = nullptr)
			: ObjectPtr(Instance)
		{
			TObjectPtr_IncrementReferenceCount();
		}

		TObjectPtr(T& Object) 
			: ObjectPtr(&Object)
		{
			TObjectPtr_IncrementReferenceCount();
		}

		TObjectPtr(const T& Object)
			: ObjectPtr(const_cast<std::remove_const_t<T>*>(&Object))
		{
			TObjectPtr_IncrementReferenceCount();
		}

		TObjectPtr(const TObjectPtr<T>& Other)
			: ObjectPtr(Other.ObjectPtr)
		{
			TObjectPtr_IncrementReferenceCount();
		}

		TObjectPtr(TObjectPtr<T>&& Other) noexcept
			: ObjectPtr(Other.ObjectPtr)
		{
			Other.ObjectPtr = nullptr;

			TObjectPtr_IncrementReferenceCount();
		}

		template<typename R>
		TObjectPtr(const TObjectPtr<R>& Other)
		{
			ObjectPtr = (T*)Other.ObjectPtr;
			TObjectPtr_IncrementReferenceCount();
		}

		template<typename R>
		TObjectPtr(TObjectPtr<R>&& Other)
		{
			ObjectPtr = (T*)Other.ObjectPtr;
			Other.ObjectPtr = nullptr;
		}

		~TObjectPtr()
		{
			TObjectPtr_DecrementReferenceCount();
		}

		TObjectPtr& operator=(Type_Nullptr)
		{
			TObjectPtr_DecrementReferenceCount();
			ObjectPtr = nullptr;

			return *this;
		}

		TObjectPtr& operator=(const TObjectPtr<T>& Other)
		{
			if (this == &Other)
			{
				return *this;
			}

			Other.TObjectPtr_IncrementReferenceCount();
			TObjectPtr_DecrementReferenceCount();

			ObjectPtr = Other.ObjectPtr;

			return *this;
		}

		template<typename R>
		TObjectPtr& operator=(const TObjectPtr<R>& Other)
		{
			Other.TObjectPtr_IncrementReferenceCount();
			TObjectPtr_DecrementReferenceCount();

			ObjectPtr = Other.ObjectPtr;

			return *this;
		}

		template<typename R>
		TObjectPtr& operator=(TObjectPtr<R>&& Other)
		{
			TObjectPtr_DecrementReferenceCount();

			ObjectPtr = Other.ObjectPtr;
			Other.ObjectPtr = nullptr;

			return *this;
		}

		operator bool() { return ObjectPtr != nullptr; }
		operator bool() const { return ObjectPtr != nullptr; }

		/** Check against nullptr. */
		FORCEINLINE bool operator==(Type_Nullptr) const
		{
			return (ObjectPtr == nullptr);
		}

		/** Equality comparison. */
		template<
			typename U, 
			typename Base = std::common_type_t<T*, U*>
		>
		FORCEINLINE bool operator==(const TObjectPtr<U>& Other) const
		{
			return (ObjectPtr == nullptr ? !Other : ObjectPtr == Other.ObjectPtr);
		}

		T* operator->() { return ObjectPtr; }
		const T* operator->() const { return ObjectPtr; }

		T& operator*() { return *ObjectPtr; }
		const T& operator*() const { return *ObjectPtr; }

		T* Get() { return static_cast<T*>(ObjectPtr); }
		const T* Get() const { return static_cast<T*>(ObjectPtr); }

		template<typename R>
		FORCEINLINE TObjectPtr<R> As() const
		{
			return TObjectPtr<R>(*this);
		}

		/** Wrapper for LObject::IsA. */
		template<typename T>
		FORCEINLINE bool IsA() const
		{
			if (ObjectPtr)
			{
				return (ObjectPtr->IsA<T>());
			}

			return false;
		}

		void Reset(T* ObjectRef = nullptr)
		{
			TObjectPtr_DecrementReferenceCount();

			ObjectPtr = ObjectRef;
		}

		void Release()
		{
			if (ObjectPtr)
			{
				delete ObjectPtr;
			}

			ObjectPtr = nullptr;
		}

		template<typename ...TArgs>
		static TObjectPtr<T> Create(TArgs&&... Args);

		bool IsEqual(const TObjectPtr<T>& Other) const 
		{
			/* Validity check so no dereferencing of nullptr happends. */
			if (ObjectPtr == nullptr || Other.ObjectPtr == nullptr)
			{
				return false;
			}
			
			return *ObjectPtr == *Other.ObjectPtr;
		}

	private:
		FORCEINLINE void TObjectPtr_IncrementReferenceCount() const
		{
			if (ObjectPtr)
			{
				ObjectPtr->IncrementReferenceCount();
				TObjectPtr_Internal::AddToLiveReferences((void*)ObjectPtr);
			}
		}

		FORCEINLINE void TObjectPtr_DecrementReferenceCount() const
		{
			if (ObjectPtr)
			{
				ObjectPtr->DecrementReferenceCount();
				
				if (ObjectPtr->GetReferenceCount() == 0)
				{
					TObjectPtr_Internal::RemoveFromLiveReferences((void*)ObjectPtr);

					delete ObjectPtr;
					ObjectPtr = nullptr;
				}
			}
		}

	private:
		mutable T* ObjectPtr;

		template<class R>
		friend class TObjectPtr;
	};

}

#include "ObjectPtr.tpp"
