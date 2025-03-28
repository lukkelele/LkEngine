/******************************************************************
 * LDelegate and LMulticastDelegate
 * 
 * Implementations for single and multicast delegates.
 *
 * The creation of a delegate should use the following macros:
 *  - LK_DECLARE_DELEGATE
 *  - LK_DECLARE_MULTICAST_DELEGATE
 *  - LK_DECLARE_DELEGATE_RET
 *  - LK_DECLARE_EVENT
 *
 * TODO:
 *  - Set static names for LMulticastDelegate on declaration.
 *
 *******************************************************************/
#pragma once

#include "DelegateFwd.h"

#include "LkEngine/Core/CoreMacros.h"
#include "LkEngine/Core/Template/TypeTrait.h"

#include "LkEngine/Core/LObject/ObjectPtr.h"

namespace LkEngine::Core {

	namespace DelegateCore
	{
		static constexpr std::size_t BUFSIZE_NAME = 100;

		/**
		 * Allocation size for the max bound objects on the stack.
		 * Larger sizes will result in heap allocation instead.
		 */
		static constexpr int INLINE_ALLOCATION_SIZE = 48;

		static void* (*Alloc)(std::size_t Size) = [](std::size_t Size) 
		{ 
			return std::malloc(Size); 
		};

		static void(*Free)(void* InHeapPointer) = [](void* InHeapPointer) 
		{ 
			std::free(InHeapPointer); 
		};
	}

	namespace DelegateMemory
	{
		using FAllocateCallback = void*(*)(std::size_t Size);
		using FFreeCallback = void(*)(void* HeapPointer);

		inline void SetAllocationCallbacks(const FAllocateCallback AllocateCallback, 
										   const FFreeCallback FreeCallback)
		{
			DelegateCore::Alloc = AllocateCallback;
			DelegateCore::Free = FreeCallback;
		}
	}

	/**
	 * IDelegateBase
	 * 
	 *  Base delegate interface.
	 */
	class IDelegateBase
	{
	public:
		IDelegateBase() = default;
		virtual ~IDelegateBase() noexcept = default;

		virtual const void* GetOwner() const { return nullptr; }
		virtual void Clone(void* Destination) = 0;
	};

	/** 
	 * IDelegate 
	 *
	 *  Delegate interface.
	 */
	template<typename TReturnValue, typename... TArgs>
	class IDelegate : public IDelegateBase
	{
	public:
		virtual TReturnValue Execute(TArgs&&... Args) = 0;
	};


	/** 
	 * LStaticDelegate
	 */
	template<typename TReturnValue, typename... Args2>
	class LStaticDelegate;

	/** 
	 * LStaticDelegate
	 * 
	 *  Template specilization.
	 */
	template<typename TReturnValue, typename... TArgs, typename... Args2>
	class LStaticDelegate<TReturnValue(TArgs...), Args2...> : public IDelegate<TReturnValue, TArgs...>
	{
	public:
		using DelegateFunction = TReturnValue(*)(TArgs..., Args2...);

		LStaticDelegate(DelegateFunction InFunction, Args2&&... InPayload)
			: Function(InFunction)
			, Payload(std::forward<Args2>(InPayload)...)
		{
		}

		LStaticDelegate(DelegateFunction InFunction, const std::tuple<Args2...>& InPayload)
			: Function(InFunction)
			, Payload(InPayload)
		{
		}

		FORCEINLINE virtual TReturnValue Execute(TArgs&&... Args) override
		{
			return Execute_Internal(std::forward<TArgs>(Args)..., std::index_sequence_for<Args2...>());
		}

		virtual void Clone(void* Destination) override
		{
			new (Destination) LStaticDelegate(Function, Payload);
		}

	private:
		template<std::size_t... Is>
		FORCEINLINE TReturnValue Execute_Internal(TArgs&&... Args, std::index_sequence<Is...>)
		{
			return Function(std::forward<TArgs>(Args)..., std::get<Is>(Payload)...);
		}

		DelegateFunction Function;
		std::tuple<Args2...> Payload;
	};


	/**
	 * LRawDelegate
	 */
	template<bool bIsConst, typename T, typename TReturnValue, typename... Args2>
	class LRawDelegate;

	/**
	 * LRawDelegate
	 *
	 *  Template specilization.
	 */
	template<bool bIsConst, typename T, typename TReturnValue, typename... TArgs, typename... Args2>
	class LRawDelegate<bIsConst, T, TReturnValue(TArgs...), Args2...> : public IDelegate<TReturnValue, TArgs...>
	{
	public:
		using DelegateFunction = typename Core::MemberFunction<bIsConst, T, TReturnValue, TArgs..., Args2...>::type;

		LRawDelegate(T* InObjectRef, DelegateFunction InFunction, Args2&&... InPayload)
			: ObjectRef(InObjectRef)
			, Function(InFunction)
			, Payload(std::forward<Args2>(InPayload)...)
		{
			LK_CORE_ASSERT(InObjectRef, "Passed object is nullptr");
			LK_CORE_ASSERT(InFunction, "Passed function is invalid");
		}

		LRawDelegate(T* InObjectRef, DelegateFunction InFunction, const std::tuple<Args2...>& InPayload)
			: ObjectRef(InObjectRef)
			, Function(InFunction)
			, Payload(InPayload)
		{
			LK_CORE_ASSERT(InObjectRef, "Passed object is nullptr");
			LK_CORE_ASSERT(InFunction, "Passed function is invalid");
		}

		FORCEINLINE virtual TReturnValue Execute(TArgs&&... Args) override
		{
			return Execute_Internal(std::forward<TArgs>(Args)..., std::index_sequence_for<Args2...>());
		}

		FORCEINLINE virtual const void* GetOwner() const override
		{
			return ObjectRef;
		}

		virtual void Clone(void* Destination) override
		{
			new (Destination) LRawDelegate(ObjectRef, Function, Payload);
		}

	private:
		template<std::size_t... Is>
		FORCEINLINE TReturnValue Execute_Internal(TArgs&&... Args, std::index_sequence<Is...>)
		{
			return (ObjectRef->*Function)(std::forward<TArgs>(Args)..., std::get<Is>(Payload)...);
		}

		T* ObjectRef = nullptr;
		DelegateFunction Function;
		std::tuple<Args2...> Payload;
	};


	/**
	 * LLambdaDelegate
	 */
	template<typename TLambda, typename TReturnValue, typename... TArgs>
	class LLambdaDelegate;

	/**
	 * LLambdaDelegate
	 * 
	 *  Template specialization.
	 */
	template<typename TLambda, typename TReturnValue, typename... TArgs, typename... Args2>
	class LLambdaDelegate<TLambda, TReturnValue(TArgs...), Args2...> : public IDelegate<TReturnValue, TArgs...>
	{
	public:
		explicit LLambdaDelegate(TLambda&& InLambda, Args2&&... InPayload)
			: Lambda(std::forward<TLambda>(InLambda))
			, Payload(std::forward<Args2>(InPayload)...)
		{
		}

		explicit LLambdaDelegate(const TLambda& InLambda, const std::tuple<Args2...>& InPayload)
			: Lambda(InLambda)
			, Payload(InPayload)
		{
		}

		FORCEINLINE TReturnValue Execute(TArgs&&... Args) override
		{
			return Execute_Internal(std::forward<TArgs>(Args)..., std::index_sequence_for<Args2...>());
		}

		FORCEINLINE virtual void Clone(void* Destination) override
		{
			new (Destination) LLambdaDelegate(Lambda, Payload);
		}

	private:
		template<std::size_t... Is>
		FORCEINLINE TReturnValue Execute_Internal(TArgs&&... Args, std::index_sequence<Is...>)
		{
			return (TReturnValue)((Lambda)(std::forward<TArgs>(Args)..., std::get<Is>(Payload)...));
		}

		TLambda Lambda;
		std::tuple<Args2...> Payload;
	};


	/**
	 * LSharedPtrDelegate
	 * 
	 *  Shared pointer implementation.
	 */
	template<bool bIsConst, typename T, typename TReturnValue, typename... TArgs>
	class LSharedPtrDelegate;

	template<bool bIsConst, typename TReturnValue, typename T, typename... TArgs, typename... Args2>
	class LSharedPtrDelegate<bIsConst, T, TReturnValue(TArgs...), Args2...> : public IDelegate<TReturnValue, TArgs...>
	{
	public:
		using DelegateFunction = typename Core::MemberFunction<bIsConst, T, TReturnValue, TArgs..., Args2...>::type;

		LSharedPtrDelegate(std::shared_ptr<T> InObjectRef, 
						   const DelegateFunction InFunction, 
						   Args2&&... InPayload)
			: ObjectRef(InObjectRef)
			, Function(InFunction)
			, Payload(std::forward<Args2>(InPayload)...)
		{
		}

		LSharedPtrDelegate(std::weak_ptr<T> InObjectRef, 
						   const DelegateFunction InFunction, 
						   const std::tuple<Args2...>& InPayload)
			: ObjectRef(InObjectRef)
			, Function(InFunction)
			, Payload(InPayload)
		{
		}

		FORCEINLINE virtual TReturnValue Execute(TArgs&&... Args) override
		{
			return Execute_Internal(std::forward<TArgs>(Args)..., std::index_sequence_for<Args2...>());
		}

		FORCEINLINE virtual const void* GetOwner() const override
		{
			return (ObjectRef.expired() ? nullptr : ObjectRef.lock().get());
		}

		FORCEINLINE virtual void Clone(void* Destination) override
		{
			new (Destination) LSharedPtrDelegate(ObjectRef, Function, Payload);
		}

	private:
		template<std::size_t... Is>
		TReturnValue Execute_Internal(TArgs&&... Args, std::index_sequence<Is...>)
		{
			if (ObjectRef.expired())
			{
				return TReturnValue();
			}
			else
			{
				std::shared_ptr<T> Object = ObjectRef.lock();
				return (Object.get()->*Function)(std::forward<TArgs>(Args)..., std::get<Is>(Payload)...);
			}
		}

		std::weak_ptr<T> ObjectRef;
		DelegateFunction Function;
		std::tuple<Args2...> Payload;
	};


	/**
	 * FDelegateHandle
	 *  
	 *  Unique identifier for delegate objects.
	 *  Used in event systems and to more easily register and manage callbacks.
	 */
	struct FDelegateHandle
	{
	public:
		constexpr FDelegateHandle() noexcept
			: ID(NullID)
		{
		}

		explicit FDelegateHandle(bool) noexcept
			: ID(GetNewID())
		{
		}

		~FDelegateHandle() noexcept = default;

		FDelegateHandle(const FDelegateHandle& Other) = default;
		FDelegateHandle& operator=(const FDelegateHandle& Other) = default;

		FDelegateHandle(FDelegateHandle&& Other) noexcept
			: ID(Other.ID)
		{
			Other.Reset();
		}

		FDelegateHandle& operator=(FDelegateHandle&& Other) noexcept
		{
			ID = Other.ID;
			Other.Reset();

			return *this;
		}

		FORCEINLINE operator bool() const noexcept
		{
			return IsValid();
		}

		FORCEINLINE bool operator==(const FDelegateHandle& Other) const noexcept
		{
			return (ID == Other.ID);
		}

		FORCEINLINE bool operator<(const FDelegateHandle& Other) const noexcept
		{
			return (ID < Other.ID);
		}

		FORCEINLINE bool IsValid() const noexcept
		{
			return (ID != NullID);
		}

		FORCEINLINE void Reset() noexcept
		{
			ID = NullID;
		}

		inline static constexpr unsigned int NullID = std::numeric_limits<unsigned int>::max();

	private:
		unsigned int ID = 0;

		/** Count of the created and assigned ID's. */
		inline static unsigned int IDCounter = 0;

		FORCEINLINE static int GetNewID()
		{
			const unsigned int Output = FDelegateHandle::IDCounter++;
			if (FDelegateHandle::IDCounter == NullID)
			{
				FDelegateHandle::IDCounter = 0;
			}

			return Output;
		}
	};

	/**
	 * LInlineAllocator
	 * 
	 *  Memory allocator that makes use of small memory allocations by 
	 *  using a fixed-size stack buffer whenever the requested memory is
	 *  small. Utilizes heap memory for larger allocations.
	 */
	template<size_t MaxStackSize>
	class LInlineAllocator
	{
	public:
		constexpr LInlineAllocator() noexcept
			: Size(0)
		{
			static_assert(MaxStackSize > sizeof(void*), "The stack size is too small");
		}

		~LInlineAllocator() noexcept
		{
			Free();
		}

		LInlineAllocator(const LInlineAllocator& Other)
			: Size(0)
		{
			if (Other.HasAllocation())
			{
				/* Deep copy. */
				memcpy(Allocate(Other.Size), Other.GetAllocation(), Other.Size);
			}

			Size = Other.Size;
		}

		LInlineAllocator(LInlineAllocator&& Other) noexcept
			: Size(Other.Size)
		{
			Other.Size = 0;

			if (Size > MaxStackSize)
			{
				std::swap(HeapPointer, Other.HeapPointer);
			}
			else
			{
				memcpy(Buffer, Other.Buffer, Size);
			}
		}

		LInlineAllocator& operator=(const LInlineAllocator& Other)
		{
			if (Other.HasAllocation())
			{
				/* Deep copy. */
				memcpy(Allocate(Other.Size), Other.GetAllocation(), Other.Size);
			}

			Size = Other.Size;

			return *this;
		}

		LInlineAllocator& operator=(LInlineAllocator&& Other) noexcept
		{
			Free();

			Size = Other.Size;
			Other.Size = 0;

			if (Size > MaxStackSize)
			{
				std::swap(HeapPointer, Other.HeapPointer);
			}
			else
			{
				memcpy(Buffer, Other.Buffer, Size);
			}

			return *this;
		}

		/** 
		 * Allocate
		 * 
		 *  Allocate memory of given size, will create on the heap if the size
		 *  is larger than the max allowed stack size.
		 */
		FORCEINLINE void* Allocate(const size_t InSize)
		{
			if (Size != InSize)
			{
				Free();
				Size = InSize;

				if (InSize > MaxStackSize)
				{
					HeapPointer = DelegateCore::Alloc(InSize);
					return HeapPointer;
				}
			}

			return (void*)Buffer;
		}

		/** 
		 * Free
		 *
		 *  Release heap memory if any has been assigned.
		 */
		FORCEINLINE void Free()
		{
			if (Size > MaxStackSize)
			{
				DelegateCore::Free(HeapPointer);
			}

			Size = 0;
		}

		/** 
		 * GetAllocation
		 *
		 *  Return the allocated memory, regardless if on the heap or not. 
		 */
		FORCEINLINE void* GetAllocation() const
		{
			if (HasAllocation())
			{
				return (HasHeapAllocation() ? HeapPointer : (void*)Buffer);
			}
			else
			{
				return nullptr;
			}
		}

		FORCEINLINE size_t GetSize() const { return Size; }

		FORCEINLINE bool HasAllocation() const
		{
			return (Size > 0);
		}

		FORCEINLINE bool HasHeapAllocation() const
		{
			return (Size > MaxStackSize);
		}

	private:
		union
		{
			char Buffer[MaxStackSize] = {};
			void* HeapPointer;
		};

		size_t Size = 0;
	};


	/**
	 * LDelegateBase
	 * 
	 *  Base class for all delegate types.
	 */
	class LDelegateBase
	{
	public:
		constexpr LDelegateBase() noexcept
			: Allocator()
		{
		}

		virtual ~LDelegateBase() noexcept
		{
			Release();
		}

		LDelegateBase(const LDelegateBase& Other)
		{
			if (Other.Allocator.HasAllocation())
			{
				Allocator.Allocate(Other.Allocator.GetSize());
				Other.GetDelegate()->Clone(Allocator.GetAllocation());
			}
		}

		LDelegateBase(LDelegateBase&& Other) noexcept
			: Allocator(std::move(Other.Allocator))
		{
		}

		/**
		 * @brief Check if the delegate is bound.
		 */
		FORCEINLINE bool IsBound() const
		{
			return Allocator.HasAllocation();
		}

	protected:
		LDelegateBase& operator=(const LDelegateBase& Other)
		{
			Release();
			if (Other.Allocator.HasAllocation())
			{
				Allocator.Allocate(Other.Allocator.GetSize());
				Other.GetDelegate()->Clone(Allocator.GetAllocation());
			}

			return *this;
		}

		LDelegateBase& operator=(LDelegateBase&& Other) noexcept
		{
			Release();
			Allocator = std::move(Other.Allocator);

			return *this;
		}

		FORCEINLINE const void* GetOwner() const
		{
			return (Allocator.HasAllocation() ? GetDelegate()->GetOwner() : nullptr);
		}

		FORCEINLINE size_t GetSize() const
		{
			return Allocator.GetSize();
		}

		FORCEINLINE void ClearIfBoundTo(void* InObject)
		{
			if (InObject && IsBoundTo(InObject))
			{
				Release();
			}
		}

		FORCEINLINE void Clear()
		{
			Release();
		}

		FORCEINLINE bool IsBoundTo(void* InObject) const
		{
			if (!InObject || !Allocator.HasAllocation())
			{
				return false;
			}

			return (GetDelegate()->GetOwner() == InObject);
		}

	protected:
		FORCEINLINE void Release()
		{
			if (Allocator.HasAllocation())
			{
				GetDelegate()->~IDelegateBase();
				Allocator.Free();
			}
		}

		IDelegateBase* GetDelegate() const
		{
			return static_cast<IDelegateBase*>(Allocator.GetAllocation());
		}

		LInlineAllocator<DelegateCore::INLINE_ALLOCATION_SIZE> Allocator;
	};


	/**
	 * LDelegate
	 *
	 *  Supports binding to all types of functions.
	 */
	template<TStringLiteral DelegateName, typename TReturnValue, typename... TArgs>
	class LDelegate : public LDelegateBase
	{
	public:
		LDelegate() = default;
		~LDelegate() = default;

	private:
		template<typename T, typename... Args2>
		using ConstMemberFunction = typename Core::MemberFunction<true, T, TReturnValue, TArgs..., Args2...>::type;

		template<typename T, typename... Args2>
		using NonConstMemberFunction = typename Core::MemberFunction<false, T, TReturnValue, TArgs..., Args2...>::type;

		using TDelegateInterface = IDelegate<TReturnValue, TArgs...>;

	public:
		/**
		 * Execute
		 *
		 *  Direct execution of the delegate, will cause crash if not properly bound.
		 */
		[[nodiscard]] FORCEINLINE TReturnValue Execute(TArgs... Args) const
		{
			LK_CORE_VERIFY(Allocator.HasAllocation(), "Delegate '{}' is not bound", typeid(this).name());
			return ((TDelegateInterface*)GetDelegate())->Execute(std::forward<TArgs>(Args)...);
		}

		/**
		 * ExecuteIfBound
		 *
		 *  Attempt to execute the delegate if it bound, otherwise ignore.
		 *  A safe way to execute.
		 */
		[[nodiscard]] FORCEINLINE TReturnValue ExecuteIfBound(TArgs... Args) const
		{
			if (IsBound())
			{
				return ((TDelegateInterface*)GetDelegate())->Execute(std::forward<TArgs>(Args)...);
			}

			return TReturnValue();
		}

	private:
		template<typename T, typename... TArgs2>
		[[nodiscard]] static LDelegate CreateRaw(T* InObject, NonConstMemberFunction<T, TArgs2...> InFunction, TArgs2... Args)
		{
			LDelegate Handler;
			Handler.Bind_Internal<LRawDelegate<false, T, TReturnValue(TArgs...), TArgs2...>>(
				InObject, 
				InFunction, 
				std::forward<TArgs2>(Args)...
			);
			
			return Handler;
		}

		template<typename T, typename... TArgs2>
		[[nodiscard]] static LDelegate CreateRaw(T* InObject, ConstMemberFunction<T, TArgs2...> InFunction, TArgs2... Args)
		{
			LDelegate Handler;
			Handler.Bind_Internal<LRawDelegate<true, T, TReturnValue(TArgs...), TArgs2...>>(InObject, InFunction, std::forward<TArgs2>(Args)...);
			return Handler;
		}

		template<typename... TArgs2>
		[[nodiscard]] static LDelegate CreateStatic(TReturnValue(*InFunction)(TArgs..., TArgs2...), TArgs2... Args)
		{
			LDelegate Handler;
			Handler.Bind_Internal<LStaticDelegate<TReturnValue(TArgs...), TArgs2...>>(InFunction, std::forward<TArgs2>(Args)...);
			return Handler;
		}

		template<typename T, typename... TArgs2>
		[[nodiscard]] static LDelegate CreateShared(const std::shared_ptr<T>& ObjectRef, 
													 NonConstMemberFunction<T, TArgs2...> InFunction, 
													 TArgs2... Args)
		{
			LDelegate Handler;
			Handler.Bind_Internal<LSharedPtrDelegate<false, T, TReturnValue(TArgs...), TArgs2...>>(
				ObjectRef, 
				InFunction, 
				std::forward<TArgs2>(Args)...
			);

			return Handler;
		}

		template<typename T, typename... TArgs2>
		[[nodiscard]] static LDelegate CreateShared(const std::shared_ptr<T>& ObjectRef, 
													 ConstMemberFunction<T, TArgs2...> InFunction, 
													 TArgs2... Args)
		{
			LDelegate Handler;
			Handler.Bind_Internal<LSharedPtrDelegate<true, T, TReturnValue(TArgs...), TArgs2...>>(
				ObjectRef, 
				InFunction, 
				std::forward<TArgs2>(Args)...
			);

			return Handler;
		}

		template<typename TLambda, typename... TArgs2>
		[[nodiscard]] static LDelegate CreateLambda(TLambda&& InLambda, TArgs2... Args)
		{
			using LambdaType = std::decay_t<TLambda>;
			LDelegate Handler;
			Handler.Bind_Internal<LLambdaDelegate<LambdaType, TReturnValue(TArgs...), TArgs2...>>(
				std::forward<LambdaType>(InLambda), 
				std::forward<TArgs2>(Args)...
			);

			return Handler;
		}

	public:
		/* Bind: Raw, non-const member function. */
		template<typename T, typename... TArgs2>
		void Bind(T* ObjectRef, NonConstMemberFunction<T, TArgs2...> InFunction, TArgs2&&... Args)
		{
			static_assert(!std::is_const_v<T>, "Non-const function cannot be bound on a const object");
			*this = CreateRaw<T, TArgs2...>(ObjectRef, InFunction, std::forward<TArgs2>(Args)...);
		}

		/* Bind: Raw, const member function. */
		template<typename T, typename... TArgs2>
		void Bind(T* ObjectRef, ConstMemberFunction<T, TArgs2...> InFunction, TArgs2&&... Args)
		{
			*this = CreateRaw<T, TArgs2...>(ObjectRef, InFunction, std::forward<TArgs2>(Args)...);
		}

		/* Bind: Static. */
		template<typename... TArgs2>
		void Bind(TReturnValue(*InFunction)(TArgs..., TArgs2...), TArgs2&&... Args)
		{
			*this = CreateStatic<TArgs2...>(InFunction, std::forward<TArgs2>(Args)...);
		}

		/* Bind: Lambda. */
		template<typename LambdaType, typename... Args2>
		void Bind(LambdaType&& InLambda, Args2&&... args)
		{
			*this = CreateLambda<LambdaType, Args2...>(std::forward<LambdaType>(InLambda), std::forward<Args2>(args)...);
		}

		/* Bind: Shared Pointer, non-const member function. */
		template<typename T, typename... Args2>
		void Bind(std::shared_ptr<T> ObjectRef, NonConstMemberFunction<T, Args2...> InFunction, Args2&&... args)
		{
			static_assert(!std::is_const_v<T>, "Attempted to bind a non-const member function on a const object reference");
			*this = CreateShared<T, Args2...>(ObjectRef, InFunction, std::forward<Args2>(args)...);
		}

		/* Bind: Shared Pointer, const member function. */
		template<typename T, typename... Args2>
		void Bind(std::shared_ptr<T> ObjectRef, ConstMemberFunction<T, Args2...> InFunction, Args2&&... args)
		{
			*this = CreateShared<T, Args2...>(ObjectRef, InFunction, std::forward<Args2>(args)...);
		}

	private:
		template<typename T, typename... TBindArgs>
		FORCEINLINE void Bind_Internal(TBindArgs&&... Args)
		{
			Release();
			void* AllocPointer = Allocator.Allocate(sizeof(T));
			new (AllocPointer) T(std::forward<TBindArgs>(Args)...);
		}

		static std::string_view GetStaticName() { return StaticName; }

	private:
		/** Allow access to the Create<TFunction> functions. */
	#if defined(LK_ENGINE_MSVC)
		template<typename... TArgs>
	#elif defined(LK_ENGINE_GCC) || defined(LK_ENGINE_CLANG)
		template<typename... TArgs2>
	#endif
		friend class LMulticastDelegate;

		inline static constexpr TStringLiteral StaticName = DelegateName;
	};


	/**
	 * LMulticastDelegate
	 *
	 *  Supports most function types.
	 */
	template<typename... TArgs>
	class LMulticastDelegate : public LDelegateBase
	{
	private:
		template<typename T, typename... TArgs2>
		using ConstMemberFunction = typename Core::MemberFunction<true, T, void, TArgs..., TArgs2...>::type;

		template<typename T, typename... TArgs2>
		using NonConstMemberFunction = typename Core::MemberFunction<false, T, void, TArgs..., TArgs2...>::type;

	public:
		using TDelegate = LDelegate<"MulticastComponent", void, TArgs...>;

		constexpr LMulticastDelegate() : Locks(0) {}
		~LMulticastDelegate() noexcept = default;

		LMulticastDelegate(const LMulticastDelegate& Other) = default;
		LMulticastDelegate(LMulticastDelegate&& Other) noexcept
			: Dispatchers(std::move(Other.Dispatchers))
			, Locks(std::move(Other.Locks))
		{
		}

		LMulticastDelegate& operator=(const LMulticastDelegate& Other) = default;

		LMulticastDelegate& operator=(LMulticastDelegate&& Other) noexcept
		{
			Dispatchers = std::move(Other.Dispatchers);
			Locks = std::move(Other.Locks);
			return *this;
		}

		/**
		 * Broadcast
		 *
		 *  Invoke all bound callbacks.
		 */
		FORCEINLINE void Broadcast(TArgs... Args)
		{
			Lock();
			{
				for (size_t i = 0; i < Dispatchers.size(); ++i)
				{
					if (Dispatchers[i].Handle.IsValid())
					{
						Dispatchers[i].Callback.Execute(Args...);
					}
				}
			}
			Unlock();
		}

		/**
		 * Remove
		 *
		 *  Remove a bound delegate.
		 */
		FORCEINLINE bool Remove(FDelegateHandle& Handle)
		{
			if (Handle.IsValid())
			{
				for (size_t i = 0; i < Dispatchers.size(); ++i)
				{
					if (Dispatchers[i].Handle == Handle)
					{
						if (IsLocked())
						{
							Dispatchers[i].Callback.Clear();
						}
						else
						{
							std::swap(Dispatchers[i], Dispatchers[Dispatchers.size() - 1]);
							Dispatchers.pop_back();
						}

						Handle.Reset();
						return true;
					}
				}
			}

			return false;
		}

		/** 
		 * RemoveAll
		 *
		 *  Clear all bound callbacks.
		 */
		FORCEINLINE void RemoveAll()
		{
			if (IsLocked())
			{
				for (FDelegateHandlerPair& Handler : Dispatchers)
				{
					Handler.Callback.Clear();
				}
			}
			else
			{
				Dispatchers.clear();
			}
		}

		/**
		 * GetSize
		 * 
		 *  Return the count of bound callbacks.
		 */
		FORCEINLINE size_t GetSize() const { return Dispatchers.size(); }

		FORCEINLINE std::string_view GetName() const { return typeid(decltype(this)).name(); }

		FORCEINLINE std::string ToString() const
		{
			#if 0
			return LK_FMT_LIB::format("\n[Multicast Delegate]\n" 
							   " - Name:     {}\n"
							   " - Raw Name: {}\n", 
							   TypeName, typeid(*this).raw_name());
			#endif
			return "MulticastDelegate::FIXME";
		}

		/** Raw Pointer, non-const function. */
		template<typename T, typename... TArgs2>
		FDelegateHandle Add(T* ObjectRef, NonConstMemberFunction<T, TArgs2...> InFunction, TArgs2&&... Args)
		{
			return AddHandler(TDelegate::CreateRaw(ObjectRef, InFunction, std::forward<TArgs2>(Args)...));
		}

		/** Raw Pointer, const function. */
		template<typename T, typename... TArgs2>
		FDelegateHandle Add(T* ObjectRef, ConstMemberFunction<T, TArgs2...> InFunction, TArgs2&&... Args)
		{
			return AddHandler(TDelegate::CreateRaw(ObjectRef, InFunction, std::forward<TArgs2>(Args)...));
		}

		/** Lambda. */
		template<typename LambdaType, typename... TArgs2>
		FDelegateHandle Add(LambdaType&& InLambda, TArgs2&&... LambdaArgs)
		{
			return AddHandler(TDelegate::CreateLambda(std::forward<LambdaType>(InLambda), std::forward<TArgs2>(LambdaArgs)...));
		}

		/** Shared Pointer, non-const function. */
		template<typename T, typename... TArgs2>
		FDelegateHandle Add(std::shared_ptr<T> ObjectRef, NonConstMemberFunction<T, TArgs2...> InFunction, TArgs2&&... Args)
		{
			return AddHandler(TDelegate::CreateShared(ObjectRef, InFunction, std::forward<TArgs2>(Args)...));
		}

		/** Shared Pointer, const function. */
		template<typename T, typename... TArgs2>
		FDelegateHandle Add(std::shared_ptr<T> ObjectRef, ConstMemberFunction<T, TArgs2...> InFunction, TArgs2&&... Args)
		{
			return AddHandler(TDelegate::CreateShared(ObjectRef, InFunction, std::forward<TArgs2>(Args)...));
		}

		/** TObjectPtr, non-const function. */
		template<typename T, typename... TArgs2>
		FDelegateHandle Add(TObjectPtr<T> ObjectRef, NonConstMemberFunction<T, TArgs2...> InFunction, TArgs2&&... Args)
		{
			return AddHandler(TDelegate::CreateRaw(ObjectRef.Get(), InFunction, std::forward<TArgs2>(Args)...));
		}

		/** TObjectPtr, const function. */
		template<typename T, typename... TArgs2>
		FDelegateHandle Add(TObjectPtr<T> ObjectRef, ConstMemberFunction<T, TArgs2...> InFunction, TArgs2&&... Args)
		{
			return AddHandler(TDelegate::CreateRaw(ObjectRef.Get(), InFunction, std::forward<TArgs2>(Args)...));
		}

		/** Static Function. */
		template<typename... TArgs2>
		FDelegateHandle Add(void(*InFunction)(TArgs..., TArgs2...), TArgs2&&... Args)
		{
			return AddHandler(TDelegate::CreateStatic(InFunction, std::forward<TArgs2>(Args)...));
		}

	private:
		template<typename T>
		FDelegateHandle operator+=(T&& LHS)
		{
			return AddHandler(TDelegate::CreateLambda(std::move(LHS)));
		}

		FDelegateHandle operator+=(TDelegate&& InHandler) noexcept
		{
			return AddHandler(std::forward<TDelegate>(InHandler));
		}

		bool operator-=(FDelegateHandle& InHandle)
		{
			return Remove(InHandle);
		}

		FORCEINLINE FDelegateHandle AddHandler(TDelegate&& Handler) noexcept
		{
			for (size_t i = 0; i < Dispatchers.size(); ++i)
			{
				if (Dispatchers[i].Handle.IsValid() == false)
				{
					Dispatchers[i] = FDelegateHandlerPair(FDelegateHandle(true), std::move(Handler));

					return Dispatchers[i].Handle;
				}
			}

			Dispatchers.emplace_back(FDelegateHandle(true), std::move(Handler));

			return Dispatchers.back().Handle;
		}

		/**
		 * RemoveObject.
		 *
		 *  Removes all handles that are bound on an object
		 */
		FORCEINLINE void RemoveObject(void* InObjectRef)
		{
			if (InObjectRef)
			{
				for (size_t i = 0; i < Dispatchers.size(); ++i)
				{
					if (Dispatchers[i].Callback.GetOwner() == InObjectRef)
					{
						if (IsLocked())
						{
							Dispatchers[i].Callback.Clear();
						}
						else
						{
							std::swap(Dispatchers[i], Dispatchers[Dispatchers.size() - 1]);
							Dispatchers.pop_back();
						}
					}
				}
			}
		}

		FORCEINLINE bool IsBoundTo(const FDelegateHandle& Handle) const
		{
			if (Handle.IsValid())
			{
				for (size_t i = 0; i < Dispatchers.size(); ++i)
				{
					if (Dispatchers[i].Handle == Handle)
					{
						return true;
					}
				}
			}

			return false;
		}

		FORCEINLINE void Compress(const size_t MaxSpace = 0)
		{
			if (IsLocked() == false)
			{
				size_t ToDelete = 0;
				for (size_t i = 0; i < Dispatchers.size() - ToDelete; i++)
				{
					if (!Dispatchers[i].Handle.IsValid())
					{
						std::swap(Dispatchers[i], Dispatchers[ToDelete]);
						ToDelete++;
					}
				}

				if (ToDelete > MaxSpace)
				{
					Dispatchers.resize(Dispatchers.size() - ToDelete);
				}
			}
		}

	private:
		FORCEINLINE void Lock()
		{
			Locks++;
		}

		FORCEINLINE void Unlock()
		{
			LK_CORE_ASSERT(Locks > 0, "Cannot unlock, locks is {}", Locks);
			Locks--;
		}

		/**
		 * IsLocked
		 *
		 *  Is true whenever we are broadcasting.
		 */
		FORCEINLINE bool IsLocked() const { return (Locks > 0); }

		/**
		 * FDelegateHandlerPair
		 *
		 *  Container for storing an UUID to a delegate object.
		 */
		struct FDelegateHandlerPair
		{
			FDelegateHandle Handle;
			TDelegate Callback;

			FDelegateHandlerPair() 
				: Handle(false) 
			{
			}

			FDelegateHandlerPair(const FDelegateHandle& InHandle, const TDelegate& InCallback) 
				: Handle(InHandle)
				, Callback(InCallback) 
			{
			}

			FDelegateHandlerPair(const FDelegateHandle& InHandle, TDelegate&& InCallback) 
				: Handle(InHandle)
				, Callback(std::move(InCallback)) 
			{
			}
		};

	private:
		std::vector<FDelegateHandlerPair> Dispatchers{};
		uint32_t Locks = 0;
	};
}
