///|//////////////////////////////////////////////////////////////
/// LObjectPtr
///
/// Object Pointer
///
///|//////////////////////////////////////////////////////////////
#pragma once

#include <unordered_set>

#include "LkEngine/Core/CoreMacros.h"
#include "LkEngine/Core/LObject/Object.h"


namespace LkEngine {

	/**
	 * TObjectPtr_Internal
	 * 
	 *  Management of live references.
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
	struct TObjectPtr
	{
	public:
		TObjectPtr(T* Instance = nullptr)
			: ObjectPtr(Instance)
		{
			Pointer_IncrementReferenceCount();
		}

		TObjectPtr(const TObjectPtr<T>& Other)
			: ObjectPtr(Other.ObjectPtr)
		{
			Pointer_IncrementReferenceCount();
		}

		TObjectPtr(TObjectPtr<T>&& Other) noexcept
			: ObjectPtr(Other.ObjectPtr)
		{
			Other.ObjectPtr = nullptr;

			Pointer_IncrementReferenceCount();
		}

		/// Disabled for now.
		/// Prob not used.
	#if 0
		/* Object reference. */
		TObjectPtr(T& Object) 
			: ObjectPtr(&Object)
		{
			Pointer_IncrementReferenceCount();
		}

		/* Const object reference. */
		TObjectPtr(const T& Object)
			: ObjectPtr(const_cast<std::remove_const_t<T>*>(&Object))
		{
			Pointer_IncrementReferenceCount();
		}
	#endif

		template<typename R>
		TObjectPtr(const TObjectPtr<R>& Other)
		{
			ObjectPtr = (T*)Other.ObjectPtr;
			Pointer_IncrementReferenceCount();
		}

		template<typename R>
		TObjectPtr(TObjectPtr<R>&& Other)
		{
			ObjectPtr = (T*)Other.ObjectPtr;
			Other.ObjectPtr = nullptr;
		}

		~TObjectPtr()
		{
			Pointer_DecrementReferenceCount();
		}

		TObjectPtr& operator=(Type_Nullptr)
		{
			Pointer_DecrementReferenceCount();
			ObjectPtr = nullptr;

			return *this;
		}

		TObjectPtr& operator=(const TObjectPtr<T>& Other)
		{
			if (this == &Other)
			{
				return *this;
			}

			Other.Pointer_IncrementReferenceCount();
			Pointer_DecrementReferenceCount();

			ObjectPtr = Other.ObjectPtr;

			return *this;
		}

		template<typename R>
		TObjectPtr& operator=(const TObjectPtr<R>& Other)
		{
			Other.Pointer_IncrementReferenceCount();
			Pointer_DecrementReferenceCount();

			ObjectPtr = Other.ObjectPtr;

			return *this;
		}

		template<typename R>
		TObjectPtr& operator=(TObjectPtr<R>&& Other)
		{
			Pointer_DecrementReferenceCount();

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
		TObjectPtr<R> As() const
		{
			return TObjectPtr<R>(*this);
		}

		void Reset(T* ObjectRef = nullptr)
		{
			Pointer_DecrementReferenceCount();

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

		bool EqualsObject(const TObjectPtr<T>& Other) const 
		{
			/* Validity check so no dereferencing of nullptr happends. */
			if (ObjectPtr == nullptr || Other.ObjectPtr == nullptr)
			{
				return false;
			}
			
			return *ObjectPtr == *Other.ObjectPtr;
		}

	private:
		FORCEINLINE void Pointer_IncrementReferenceCount() const
		{
			if (ObjectPtr)
			{
				ObjectPtr->IncrementReferenceCount();
				TObjectPtr_Internal::AddToLiveReferences((void*)ObjectPtr);
			}
		}

		FORCEINLINE void Pointer_DecrementReferenceCount() const
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
		friend struct TObjectPtr;
	};

#if 0
	static std::unordered_set<void*> LiveReferences;
	static std::mutex LiveReferenceMutex;

	template<typename T>
	FORCEINLINE void TObjectPtr<T>::AddToLiveReferences(void* ObjectInstance)
	{
		std::scoped_lock<std::mutex> ScopedLock(LiveReferenceMutex);
		LK_CORE_ASSERT(ObjectInstance != nullptr);
		LiveReferences.insert(ObjectInstance);
	}

	template<typename T>
	FORCEINLINE void TObjectPtr<T>::RemoveFromLiveReferences(void* ObjectInstance)
	{
		std::scoped_lock<std::mutex> ScopedLock(LiveReferenceMutex);
		LK_CORE_ASSERT(ObjectInstance != nullptr);
		LK_CORE_ASSERT(LiveReferences.find(ObjectInstance) != LiveReferences.end());

		LiveReferences.erase(ObjectInstance);
	}

	template<typename T>
	FORCEINLINE bool TObjectPtr<T>::IsLive(void* ObjectInstance)
	{
		LK_CORE_ASSERT(ObjectInstance != nullptr);

		return LiveReferences.find(ObjectInstance) != LiveReferences.end();
	}
#endif

}

#include "LObjectPtr.tpp"
