/******************************************************************
 * LObjectPtr
 *
 *******************************************************************/
#pragma once

#include <unordered_set>

#include "LkEngine/Core/CoreMacros.h"
#include "LkEngine/Core/LObject/Object.h"

#define LK_OBJECTPTR_CUSTOM_EQUALITY_OP 1

#define LK_DEBUG_LOG_LIVE_REFERENCES 0  /* Log whenever a reference is added/removed. */
#define LK_DEBUG_LOG_OBJECTPTR       0  /* Log TObjectPtr constructors and destructor calls. */
#define LK_DEBUG_OBJECTPTR_PARANOID  0  /* Validate the LObject pointer on all TObjectPtr access calls. */

#if LK_DEBUG_OBJECTPTR_PARANOID
#	define LK_PTR_ASSERT(...) LK_CORE_ASSERT(__VA_ARGS__)
#else
#	define LK_PTR_ASSERT(...)
#endif

#if LK_DEBUG_LOG_OBJECTPTR
#	define LK_DEBUG_PTR_LOG(...) \
			if (ObjectPtr) \
			{ \
				if (const LClass* ClassInfo = ObjectPtr->GetClass(); ClassInfo != nullptr) \
				{ \
					LK_PRINTLN("[TObjectPtr::{}] {}", ClassInfo->GetName(), __VA_ARGS__); \
				} \
				else \
				{ \
					LK_PRINTLN("[TObjectPtr::LObject] {}", __VA_ARGS__); \
				} \
			} \
			else \
			{ \
				LK_PRINTLN("[TObjectPtr] {}", __VA_ARGS__); \
			} 
#else
#	define LK_DEBUG_PTR_LOG(...) 
#endif

namespace LkEngine {

	template<typename T>
	class TObjectPtr;

	/*----------------------------------------------------------------------------
		Internal Memory Management.
	 ----------------------------------------------------------------------------*/
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
		int GetLiveObjects(std::vector<TObjectPtr<LObject>>& ObjectArray, const bool FilterByStaticClass = true);
	}

	/**
	 * TObjectPtr
	 *
	 *  Smart pointer for managing LObject's with reference counting and tracking.
	 *  Provides automatic reference counting, basic type safety and the ability 
	 *  to track live object references for memory management.
	 *  It supports initialization from raw pointers, other TObjectPtr instances 
	 *  and conversion between compatible types.
	 */
	template<typename T>
	class TObjectPtr
	{
	public:
		TObjectPtr(T* Instance = nullptr)
			: ObjectPtr(Instance)
		{
			LK_DEBUG_PTR_LOG("Copy Constructor (T* Instance = nullptr)");
			static_assert(std::is_base_of_v<LObject, T>, "TObject is only usable on objects that derive from LObject");
			if (ObjectPtr)
			{
				TObjectPtr_IncrementReferenceCount();
			}
		}

		TObjectPtr(T& Object)
			: ObjectPtr(&Object)
		{
			LK_DEBUG_PTR_LOG("Copy(T&)");
			if (ObjectPtr)
			{
				TObjectPtr_IncrementReferenceCount();
			}
		}

		TObjectPtr(const T& Object)
			: ObjectPtr(const_cast<std::remove_const_t<T>*>(&Object))
		{
			LK_DEBUG_PTR_LOG("Copy(const T&)");
			if (ObjectPtr)
			{
				TObjectPtr_IncrementReferenceCount();
			}
		}

		TObjectPtr(const TObjectPtr<T>& Other)
			: ObjectPtr(Other.ObjectPtr)
		{
			LK_DEBUG_PTR_LOG("Copy(TObjectPtr<T>&)");
			if (ObjectPtr)
			{
				TObjectPtr_IncrementReferenceCount();
			}
		}

		template<typename R>
		TObjectPtr(const TObjectPtr<R>& Other)
			: ObjectPtr((T*)Other.ObjectPtr)
		{
			LK_DEBUG_PTR_LOG("Copy(const TObjectPtr<R>&)");
			if (ObjectPtr)
			{
				TObjectPtr_IncrementReferenceCount();
			}
		}

		TObjectPtr(TObjectPtr<T>&& Other) noexcept
			: ObjectPtr(Other.ObjectPtr)
		{
			LK_DEBUG_PTR_LOG("Move(TObjectPtr<T>&&)");
			Other.ObjectPtr = nullptr;
		}

		template<typename R>
		TObjectPtr(TObjectPtr<R>&& Other)
			: ObjectPtr((T*)Other.ObjectPtr)
		{
			LK_DEBUG_PTR_LOG("Move(TObjectPtr<R>&&)");
			Other.ObjectPtr = nullptr;
		}

		~TObjectPtr()
		{
			LK_DEBUG_PTR_LOG("Destructor");
			if (ObjectPtr)
			{
				TObjectPtr_DecrementReferenceCount();
			}
		}

		TObjectPtr& operator=(std::nullptr_t)
		{
			LK_DEBUG_PTR_LOG("&operator=(std::nullptr_t)");
			if (ObjectPtr)
			{
				TObjectPtr_DecrementReferenceCount();
			}
			ObjectPtr = nullptr;

			return *this;
		}

		TObjectPtr& operator=(const TObjectPtr<T>& Other)
		{
			LK_DEBUG_PTR_LOG("&operator=(const TObjectPtr<T>&)");
			if (this == &Other)
			{
				return *this;
			}

			if (Other.ObjectPtr)
			{
				Other.TObjectPtr_IncrementReferenceCount();
			}
			if (ObjectPtr)
			{
				TObjectPtr_DecrementReferenceCount();
			}

			ObjectPtr = Other.ObjectPtr;

			return *this;
		}

		template<typename R>
		FORCEINLINE TObjectPtr& operator=(const TObjectPtr<R>& Other)
		{
			LK_DEBUG_PTR_LOG("&operator=(const TObjectPtr<R>&)");
			if (Other.ObjectPtr)
			{
				Other.TObjectPtr_IncrementReferenceCount();
			}
			if (ObjectPtr)
			{
				TObjectPtr_DecrementReferenceCount();
			}

			ObjectPtr = Other.ObjectPtr;

			return *this;
		}

		template<typename R>
		FORCEINLINE TObjectPtr& operator=(TObjectPtr<R>&& Other)
		{
			LK_DEBUG_PTR_LOG("&operator=(TObjectPtr<R>&&)");
			if (ObjectPtr)
			{
				TObjectPtr_DecrementReferenceCount();
			}

			ObjectPtr = Other.ObjectPtr;
			Other.ObjectPtr = nullptr;

			return *this;
		}

		FORCEINLINE operator bool() { return ObjectPtr != nullptr; }
		FORCEINLINE operator bool() const { return ObjectPtr != nullptr; }

		FORCEINLINE bool operator==(std::nullptr_t) const { return (ObjectPtr == nullptr); }
		FORCEINLINE bool operator==(const T* Other) const { return (Get() == Other); }

		FORCEINLINE T* operator->() 
		{ 
			LK_PTR_ASSERT(ObjectPtr, "TObjectPtr::operator-> failed, ObjectPtr is nullptr");
			return ObjectPtr; 
		}

		FORCEINLINE const T* operator->() const 
		{
			LK_PTR_ASSERT(ObjectPtr, "TObjectPtr::operator-> failed, ObjectPtr is nullptr");
			return ObjectPtr; 
		}

		FORCEINLINE T& operator*() 
		{ 
			LK_PTR_ASSERT(ObjectPtr, "TObjectPtr::operator* failed, ObjectPtr is nullptr");
			return *ObjectPtr; 
		}

		FORCEINLINE const T& operator*() const 
		{ 
			LK_PTR_ASSERT(ObjectPtr, "TObjectPtr::operator* failed, ObjectPtr is nullptr");
			return *ObjectPtr; 
		}

		FORCEINLINE T* Get() 
		{ 
			LK_PTR_ASSERT(ObjectPtr, "TObjectPtr::Get failed, ObjectPtr is nullptr");
			return static_cast<T*>(ObjectPtr); 
		}

		FORCEINLINE const T* Get() const 
		{ 
			LK_PTR_ASSERT(ObjectPtr, "TObjectPtr::Get failed, ObjectPtr is nullptr");
			return static_cast<T*>(ObjectPtr); 
		}

		template<typename R>
		FORCEINLINE TObjectPtr<R> As() const
		{
			static_assert(std::disjunction_v<std::is_base_of<T, R>, std::is_base_of<R, T>>, "Cannot cast TObjectPtr to that type");
			return TObjectPtr<R>(*this);
		}

		FORCEINLINE void Reset(T* ObjectRef = nullptr)
		{
			TObjectPtr_DecrementReferenceCount();
			ObjectPtr = ObjectRef;
		}

		FORCEINLINE void Release()
		{
			TObjectPtr_DecrementReferenceCount();
			ObjectPtr = nullptr;
		}

		template<typename... TArgs>
		static TObjectPtr<T> Create(TArgs&&... Args);

		FORCEINLINE bool IsEqual(const TObjectPtr<T>& Other) const
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
			LK_PTR_ASSERT(ObjectPtr, "IncrementReferenceCount failed, invalid ObjectPtr");
			ObjectPtr->IncrementReferenceCount();
			TObjectPtr_Internal::AddToLiveReferences((void*)ObjectPtr);
		}

		FORCEINLINE void TObjectPtr_DecrementReferenceCount() const
		{
			ObjectPtr->DecrementReferenceCount();

			if (ObjectPtr->GetReferenceCount() == 0)
			{
				TObjectPtr_Internal::RemoveFromLiveReferences((void*)ObjectPtr);

				delete ObjectPtr;
				ObjectPtr = nullptr;
			}
		}

	private:
		mutable T* ObjectPtr;

		template<class R>
		friend class TObjectPtr;
	};


	/**
	 * TWeakPtr
	 *
	 *  Weak pointer.
	 */
	template<typename T>
	class TWeakPtr
	{
	public:
		TWeakPtr(T* InObject = nullptr)
		{
			ObjectPtr = InObject;
		}

		TWeakPtr(TObjectPtr<T> InObjectPtr)
		{
			ObjectPtr = InObjectPtr.Get();
		}

		FORCEINLINE T* operator->() { return ObjectPtr; }
		FORCEINLINE const T* operator->() const { return ObjectPtr; }

		FORCEINLINE T& operator*() { return *ObjectPtr; }
		FORCEINLINE const T& operator*() const { return *ObjectPtr; }

		FORCEINLINE bool IsValid() const 
		{ 
			return ObjectPtr ? TObjectPtr_Internal::IsLive(ObjectPtr) : false;
		}

		FORCEINLINE operator bool() const { return IsValid(); }

		template<typename R>
		FORCEINLINE TWeakPtr<R> As() const
		{
			return TWeakPtr<R>(dynamic_cast<R*>(ObjectPtr));
		}

	private:
		T* ObjectPtr = nullptr;
	};

}


#include "ObjectPtr.tpp"
