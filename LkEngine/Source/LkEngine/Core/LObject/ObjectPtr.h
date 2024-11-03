/******************************************************************
 * LObjectPtr
 * 
 *******************************************************************/
#pragma once

#include <unordered_set>

#include "LkEngine/Core/CoreMacros.h"
#include "LkEngine/Core/LObject/Object.h"

/** Log whenever a reference is added/removed. */
#define LK_DEBUG_LOG_LIVE_REFERENCES 0


namespace LkEngine {

	template<typename T>
	class TObjectPtr;

	/*-----------------------------------------------------------------------------
		Internal Memory Management.
	-----------------------------------------------------------------------------*/
	/**
	 * TObjectPtr_Internal
	 * 
	 *  Management of live references.
	 *  For internal use by TObjectPtr.
	 */
	namespace TObjectPtr_Internal
	{
		extern std::unordered_set<void*> LiveReferences;
		extern std::mutex LiveReferenceMutex;

		void AddToLiveReferences(void* InObject);
		void RemoveFromLiveReferences(void* InObject);
		bool IsLive(void* InObject);

		int GetLiveReferences(std::unordered_set<void*>& InSet);

		/**
		 * @brief Populare a vector with live objects.
		 * @return The count of objects returned.
		 */
		int GetLiveObjects(std::vector<TObjectPtr<LObject>>& ObjectArray, 
						   const bool FilterByStaticClass = true);
	}

	/**
	 * @class TObjectPtr
	 *
	 * @brief A smart pointer for managing LObject's with reference counting and tracking.
	 *
	 * `TObjectPtr` is a templated smart pointer class that provides automatic reference counting,
	 * basic type safety, and the ability to track live object references for memory management.
	 * It supports initialization from raw pointers, other `TObjectPtr` instances and conversion 
	 * between compatible types.
	 *
	 * @note TObjectPtr uses reference counting for lifetime management. 
	 *       When the reference count reaches zero, the object is deleted and its resources released.
	 */
	template<typename T>
	class TObjectPtr
	{
	public:
		TObjectPtr(T* Instance = nullptr)
			: ObjectPtr(Instance)
		{
			if (ObjectPtr)
			{
				TObjectPtr_IncrementReferenceCount();
			}
		}

		TObjectPtr(T& Object) 
			: ObjectPtr(&Object)
		{
			if (ObjectPtr)
			{
				TObjectPtr_IncrementReferenceCount();
			}
		}

		TObjectPtr(const T& Object)
			: ObjectPtr(const_cast<std::remove_const_t<T>*>(&Object))
		{
			if (ObjectPtr)
			{
				TObjectPtr_IncrementReferenceCount();
			}
		}

		TObjectPtr(const TObjectPtr<T>& Other)
			: ObjectPtr(Other.ObjectPtr)
		{
			if (ObjectPtr)
			{
				TObjectPtr_IncrementReferenceCount();
			}
		}

		TObjectPtr(TObjectPtr<T>&& Other) noexcept
			: ObjectPtr(Other.ObjectPtr)
		{
			Other.ObjectPtr = nullptr;
		}

		template<typename R>
		TObjectPtr(const TObjectPtr<R>& Other)
			: ObjectPtr((T*)Other.ObjectPtr)
		{
			if (ObjectPtr)
			{
				TObjectPtr_IncrementReferenceCount();
			}
		}

		template<typename R>
		TObjectPtr(TObjectPtr<R>&& Other)
			: ObjectPtr((T*)Other.ObjectPtr)
		{
			Other.ObjectPtr = nullptr;
		}

		~TObjectPtr()
		{
			TObjectPtr_DecrementReferenceCount();
		}

		TObjectPtr& operator=(std::nullptr_t)
		{
			TObjectPtr_DecrementReferenceCount();
			ObjectPtr = nullptr;

			return *this;
		}

		TObjectPtr& operator=(const TObjectPtr<T>& Other)
		{
			if (this != &Other)
			{
				TObjectPtr_DecrementReferenceCount();

				ObjectPtr = Other.ObjectPtr;
				if (ObjectPtr)
				{
					TObjectPtr_IncrementReferenceCount();
				}
			}

			return *this;
		}

		TObjectPtr& operator=(TObjectPtr<T>&& Other) noexcept
		{
			if (this != &Other)
			{
				TObjectPtr_DecrementReferenceCount();

				ObjectPtr = Other.ObjectPtr;
				Other.ObjectPtr = nullptr;
			}

			return *this;
		}

		template<typename R>
		TObjectPtr& operator=(const TObjectPtr<R>& Other)
		{
			if (ObjectPtr != Other.ObjectPtr)
			{
				TObjectPtr_DecrementReferenceCount();
				ObjectPtr = (T*)Other.ObjectPtr;
				if (ObjectPtr)
				{
					TObjectPtr_IncrementReferenceCount();
				}
			}

			return *this;
		}

		template<typename R>
		TObjectPtr& operator=(TObjectPtr<R>&& Other)
		{
			if (ObjectPtr != Other.ObjectPtr)
			{
				TObjectPtr_DecrementReferenceCount();
				ObjectPtr = (T*)Other.ObjectPtr;
				Other.ObjectPtr = nullptr;
			}

			return *this;
		}

		operator bool() { return ObjectPtr != nullptr; }
		operator bool() const { return ObjectPtr != nullptr; }

		/** Check against nullptr. */
		FORCEINLINE bool operator==(std::nullptr_t) const
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

		void Reset(T* ObjectRef = nullptr)
		{
			TObjectPtr_DecrementReferenceCount();
			ObjectPtr = ObjectRef;
		}

		void Release()
		{
			TObjectPtr_DecrementReferenceCount();
			ObjectPtr = nullptr;
		}

		template<typename ...TArgs>
		static TObjectPtr<T> Create(TArgs&&... Args);

		bool IsEqual(const TObjectPtr<T>& Other) const 
		{
			if ((ObjectPtr == nullptr) || (Other.ObjectPtr == nullptr))
			{
				return false;
			}
			
			return *ObjectPtr == *Other.ObjectPtr;
		}

	private:
		FORCEINLINE void TObjectPtr_IncrementReferenceCount() const
		{
			ObjectPtr->IncrementReferenceCount();
			TObjectPtr_Internal::AddToLiveReferences((void*)ObjectPtr);
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
