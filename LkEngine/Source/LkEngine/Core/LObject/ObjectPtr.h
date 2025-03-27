/******************************************************************
 * TObjectPtr
 *
 *******************************************************************/
#pragma once

/**
 * @ingroup Core
 * @{
 */

#include <unordered_set>

#include "LkEngine/Core/CoreMacros.h"
#include "LkEngine/Core/LObject/Object.h"

#include "ObjectPtrHelpers.h"

namespace LkEngine {

	/** 
	 * @ingroup LObject
	 * @{
	 *
	 * @defgroup TObjectPtr
	 * @{
	 *
	 * @defgroup TObjectPtr_Internal
	 */

	template<typename T>
	class TObjectPtr;

	/**
	 * @brief Internal functions used by TObjectPtr for managing live LObject references.
	 * @note For internal use by TObjectPtr.
	 *
	 * @ingroup TObjectPtr_Internal
	 * @{
	 */
	namespace TObjectPtr_Internal 
	{
		/**
		 * @var LiveReferences 
		 * Set of live references.
		 *
		 * @note Defined in ObjectPtr.tpp
		 */
		extern std::unordered_set<void*> LiveReferences;

		/**
		 * @var LiveReferenceMutex
		 * Mutex used on modifications of the live object's set.
		 *
		 * @note Defined in ObjectPtr.tpp
		 */
		extern std::mutex LiveReferenceMutex;

		/**
		 * @brief Add a LObject instance to the live references.
		 * @details Does nothing if the instance already has been added.
		 */
		void AddToLiveReferences(void* InObject);

		/**
		 * @brief Remove a LObject instance from the live references.
		 * @details Does nothing if the instance does not exist in the live set.
		 */
		void RemoveFromLiveReferences(void* InObject);

		/**
		 * @brief Check if a LObject instance is alive.
		 * @returns true if the reference count for the LObject instance is larger than 0, else false.
		 */
		bool IsLive(void* InObject);

		/**
		 * @brief Populate a set with all live references.
		 * @return The count of live objects that was added onto the set.
		 */
		int GetLiveReferences(std::unordered_set<void*>& InSet);

		/**
		 * @brief Populare a vector with live objects.
		 * @return The count of live objects.
		 */
		int GetLiveObjects(std::vector<TObjectPtr<LObject>>& ObjectArray, const bool FilterByStaticClass = true);
	}
	/** @} */

	/**
	 * @class TObjectPtr
	 *
	 * @details Smart pointer for managing LObject's with reference counting and tracking.
	 *          Provides automatic reference counting, basic type safety and the ability 
	 *          to track live object references for memory management.
	 *          Supports initialization from raw pointers, other TObjectPtr instances and 
	 *          conversion between compatible types.
	 */
	template<typename T>
	class TObjectPtr
	{
	public:
		TObjectPtr(T* Instance = nullptr)
			: ObjectPtr(Instance)
		{
			LK_DEBUG_PTR_LOG("Copy Constructor (T* Instance = nullptr)");
			static_assert(std::is_base_of<LObject, T>::value, "TObject is only usable on objects that derive from LObject");
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

		/**
		 * @brief Create new pointer instance of type T.
		 * @return pointer to the new object.
		 */
		template<typename ...TArgs>
		static TObjectPtr<T> Create(TArgs&&... Args);

		FORCEINLINE bool IsEqual(const TObjectPtr<T>& Other) const
		{
			if ((ObjectPtr == nullptr) || (Other.ObjectPtr == nullptr))
			{
				return false;
			}

			return (*ObjectPtr == *Other.ObjectPtr);
		}

	private:
		/**
		 * @brief Increment the reference count by one.
		 */
		FORCEINLINE void TObjectPtr_IncrementReferenceCount() const
		{
			LK_PTR_ASSERT(ObjectPtr, "IncrementReferenceCount failed, invalid ObjectPtr");
			((LObject*)ObjectPtr)->IncrementReferenceCount();
			TObjectPtr_Internal::AddToLiveReferences((void*)ObjectPtr);
		}

		/**
		 * @brief Decrement the reference count by one.
		 */
		FORCEINLINE void TObjectPtr_DecrementReferenceCount() const
		{
			((LObject*)ObjectPtr)->DecrementReferenceCount();

			if (((LObject*)ObjectPtr)->GetReferenceCount() == 0)
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
	 * @class TWeakPtr
	 *
	 * @details Weak pointer implementation.
	 *
	 * @note Not used as of now, needs to be fully implemented.
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

		/**
		 * @brief Check if the pointer holds a valid reference.
		 */
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

	/** @} */ /* TObjectPtr */
	/** @} */ /* LObject */

}

/** @} */ /* Core */

#include "ObjectPtr.tpp"
