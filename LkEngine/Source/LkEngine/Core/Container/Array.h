/******************************************************************
 * TArray
 *
 *
 *******************************************************************/
#pragma once

#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <algorithm>

#include "LkEngine/Core/CoreMacros.h"
#include "LkEngine/Core/Assert.h"
#include "LkEngine/Core/Memory/MemoryPtr.h"


namespace LkEngine {

	/**
	 * LDefaultAllocator
	 */
	template<typename ElementType>
	class LDefaultAllocator
	{
	public:
		using SizeType = ElementType;
		static constexpr bool NeedsElementType = true;

		/**
		 * @brief Allocates memory for a given number of elements without constructing them.
		 *
		 * @param count The number of elements to allocate memory for.
		 * @return A pointer to the allocated memory.
		 */
		FORCEINLINE SizeType* Allocate(const SizeType Count)
		{
			if (Count <= 0)
			{
				return nullptr;
			}

			return static_cast<ElementType*>(::operator new(Count * sizeof(ElementType)));
		}

		/**
		 * @brief Deallocates memory for a given pointer.
		 * 
		 * @param ptr Pointer to the memory to be deallocated.
		 * @param count The number of elements that were allocated at the memory location.
		 */
		FORCEINLINE void Deallocate(ElementType* Destination, const SizeType Count)
		{
			if (Destination)
			{
				::operator delete(Destination, Count * sizeof(ElementType));
			}
		}
	};


	/**
	 * TArray
	 */
	template<typename InElementType, 
			 typename InAllocatorType = LDefaultAllocator<InElementType>>
	class TArray
	{
	public:
		using ElementType = InElementType;
		using AllocatorType = InAllocatorType;
		//using SizeType = typename InAllocatorType::SizeType; // Disabled until allocators are working.
		using SizeType = std::conditional_t<std::is_integral_v<ElementType>, ElementType, std::size_t>;
	private:

		
		template <typename OtherInElementType, typename OtherAllocator>
		friend class TArray;

	public:
		TArray() noexcept = default;

		explicit TArray(const SizeType InitialSize)
			: m_Size(InitialSize)
			, m_Capacity(InitialSize)
			, m_Data(std::make_unique<ElementType[]>(InitialSize))
		{
		}

		TArray(std::initializer_list<InElementType> InitList)
			: TArray(InitList.size())
		{
			std::copy(InitList.begin(), InitList.end(), m_Data.get());
		}

		virtual ~TArray() = default;

		TArray(const TArray& Other)
			: m_Size(Other.m_Size)
			, m_Capacity(Other.m_Capacity)
			, m_Data(std::make_unique<ElementType[]>(Other.m_Capacity))
		{
			std::copy(Other.m_Data.get(), Other.m_Data.get() + m_Size, m_Data.get());
		}

		TArray(TArray&& Other) noexcept
			: m_Size(Other.m_Size)
			, m_Capacity(Other.m_Capacity)
			, m_Data(std::move(Other.m_Data))
		{
			Other.m_Size = 0;
			Other.m_Capacity = 0;
		}

		/** 
		 * @brief Copy assignment. 
		 */
		TArray& operator=(const TArray& Other)
		{
			if (this != &Other)
			{
				m_Size = Other.m_Size;
				m_Capacity = Other.m_Capacity;
				m_Data = std::make_unique<ElementType[]>(Other.m_Capacity);
				std::copy(Other.m_Data.get(), Other.m_Data.get() + m_Size, m_Data.get());
			}

			return *this;
		}

		/** 
		 * @brief Move assignment. 
		 */
		TArray& operator=(TArray&& Other) noexcept
		{
			if (this != &Other)
			{
				m_Size = Other.m_Size;
				m_Capacity = Other.m_Capacity;
				m_Data = std::move(Other.m_Data);

				Other.m_Size = 0;
				Other.m_Capacity = 0;
			}

			return *this;
		}

		/**
		 * @brief Initializer list assignment operator. 
		 * First deletes all currently contained elements and then copies from initializer list.
		 */
		TArray& operator=(std::initializer_list<InElementType> InitList)
		{
			Clear();

			/* Ensure the array has enough capacity for the new elements. */
			Reserve(static_cast<SizeType>(InitList.size()));

			/* Copy elements from the initializer list into the array. */
			std::uninitialized_copy(InitList.begin(), InitList.end(), m_Data.get());
			m_Size = static_cast<SizeType>(InitList.size());

			return *this;
		}


		/*----------------------------------------------------------------------
			Accessors.
		----------------------------------------------------------------------*/
		FORCEINLINE ElementType& operator[](const SizeType Index)
		{
			LK_CORE_ASSERT(Index < m_Size, "Index {} is out of range,  Size={}  Capacity={}", Index, m_Size, m_Capacity);
			return m_Data[Index];
		}

		FORCEINLINE const ElementType& operator[](const SizeType Index) const
		{
			LK_CORE_ASSERT(Index < m_Size, "Index {} is out of range,  Size={}  Capacity={}", Index, m_Size, m_Capacity);
			return m_Data[Index];
		}

		FORCEINLINE ElementType& At(const SizeType Index)
		{
			return (*this)[Index];
		}

		FORCEINLINE const ElementType& At(const SizeType Index) const
		{
			return (*this)[Index];
		}

		/*----------------------------------------------------------------------
			Capacity.
		----------------------------------------------------------------------*/
		FORCEINLINE bool IsEmpty() const noexcept { return (m_Size == 0); }

		/**
		 * @brief Get array size.
		 */
		FORCEINLINE SizeType Size() const noexcept { return m_Size; }

		/**
		 * @brief Get array capacity.
		 */
		FORCEINLINE SizeType Capacity() const noexcept { return m_Capacity; }

		/**
		 * @brief Reserve
		 */
		FORCEINLINE void Reserve(const SizeType NewCapacity)
		{
			if (NewCapacity > m_Capacity)
			{
				Reallocate(NewCapacity);
			}
		}

		/**
		 * @brief Resize array.
		 */
		FORCEINLINE void Resize(const SizeType NewSize)
		{
			if (NewSize > m_Capacity)
			{
				Reserve(NewSize);
			}

			m_Size = NewSize;
		}

		/**
		 * @brief Shrink capacity to current size.
		 */
		FORCEINLINE void Shrink()
		{
			if (m_Size < m_Capacity)
			{
				/* Reduce capacity to the current size. */
				Reallocate(m_Size);
			}
		}

		/*---------------------------------------------------------------------
			Modifiers.
		----------------------------------------------------------------------*/
		FORCEINLINE void PushBack(const ElementType& Value)
		{
			EnsureCapacity(m_Size + 1);
			m_Data[m_Size++] = Value;
		}

		FORCEINLINE void PushBack(ElementType&& Value)
		{
			EnsureCapacity(m_Size + 1);
			m_Data[m_Size++] = std::move(Value);
		}

		FORCEINLINE void PopBack()
		{
			if (m_Data && (m_Size > 0))
			{
		#ifdef LK_ARRAY_PARANOID
				const uintptr_t StartAddress = reinterpret_cast<uintptr_t>(Data);
				const uintptr_t EndAddress = StartAddress + Capacity * sizeof(ElementType);
				const uintptr_t ElementAddress = reinterpret_cast<uintptr_t>(Data + Size - 1);
				LK_CORE_ASSERT((ElementAddress < StartAddress) || (ElementAddress >= EndAddress)), 
					"PopBack error, memory access out of bounds");
		#endif
				std::destroy_at(m_Data + m_Size - 1);
				m_Size--;
			}
		}

		/**
		 * @brief Clear data from array.
		 */
		FORCEINLINE void Clear() noexcept
		{
			if (m_Data)
			{
				std::destroy(m_Data, m_Data + m_Size);
				m_Size = 0;
				m_Capacity = 0;

				operator delete(m_Data);
				m_Data = nullptr;
			}
		}

		/*---------------------------------------------------------------------
			Iterator support.
		----------------------------------------------------------------------*/
		ElementType* begin() noexcept { return m_Data.get(); }
		const ElementType* begin() const noexcept { return m_Data.get(); }
		const ElementType* cbegin() const noexcept { return m_Data.get(); }

		ElementType* end() noexcept { return (m_Data.get() + m_Size); }
		const ElementType* end() const noexcept { return (m_Data.get() + m_Size); }
		const ElementType* cend() const noexcept { return (m_Data.get() + m_Size); }

	protected:
		/** Internal utility functions. */
		FORCEINLINE void EnsureCapacity(const SizeType RequiredCapacity)
		{
			LK_MARK_FUNC_NOT_IMPLEMENTED();
			if (RequiredCapacity > m_Capacity)
			{
				Reallocate(std::max(m_Capacity * 2, RequiredCapacity));
			}
		}

		FORCEINLINE void Reallocate(const SizeType NewCapacity)
		{
			LK_MARK_FUNC_NOT_IMPLEMENTED();
		#if 0
			TUniquePtr<ElementType[]> NewData = std::make_unique<ElementType[]>(NewCapacity);
			std::move(m_Data.get(), m_Data.get() + m_Size, NewData.get());

			m_Data = std::move(NewData);
			m_Capacity = NewCapacity;
		#endif
		}

	protected:
		SizeType m_Size = 0;
		SizeType m_Capacity = 0;
		TUniquePtr<ElementType[]> m_Data{};
	};

}
