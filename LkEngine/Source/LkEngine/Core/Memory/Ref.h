#pragma once

#include <memory>
#include <cstdint>
#include <atomic>


namespace LkEngine {

	class RefCounted
	{
	public:
		virtual ~RefCounted() = default;

		void IncRefCount() const { ++m_RefCount; }
		void DecRefCount() const { --m_RefCount; }

		uint32_t GetRefCount() const { return m_RefCount.load(); }

	private:
		mutable std::atomic<uint32_t> m_RefCount = 0;
	};

	namespace _RefInternal 
	{
		void AddToLiveReferences(void* instance);
		void RemoveFromLiveReferences(void* instance);
		bool IsLive(void* instance);
	}

	/* Pointer Type. */
	template<typename T>
	class Ref
	{
	public:
		Ref()
			: m_Instance(nullptr)
		{
		}
		
		Ref(std::nullptr_t n)
			: m_Instance(nullptr)
		{
		}

		Ref(T* instance)
			: m_Instance(instance)
		{
			IncRef();
		}

		template<typename T2>
		Ref(const Ref<T2>& Other)
		{
			m_Instance = (T*)Other.m_Instance;
			IncRef();
		}

		template<typename T2>
		Ref(Ref<T2>&& Other)
		{
			m_Instance = (T*)Other.m_Instance;
			Other.m_Instance = nullptr;
		}

		static Ref<T> CopyWithoutIncrement(const Ref<T>& Other)
		{
			Ref<T> result = nullptr;
			result->m_Instance = Other.m_Instance;
			return result;
		}

		~Ref()
		{
			DecRef();
		}

		Ref(const Ref<T>& Other)
			: m_Instance(Other.m_Instance)
		{
			IncRef();
		}

		Ref& operator=(std::nullptr_t)
		{
			DecRef();
			m_Instance = nullptr;
			return *this;
		}

		Ref& operator=(const Ref<T>& Other)
		{
			if (this == &Other)
			{
				return *this;
			}

			Other.IncRef();
			DecRef();

			m_Instance = Other.m_Instance;
			return *this;
		}

		template<typename T2>
		Ref& operator=(const Ref<T2>& Other)
		{
			Other.IncRef();
			DecRef();

			m_Instance = Other.m_Instance;
			return *this;
		}

		template<typename T2>
		Ref& operator=(Ref<T2>&& Other)
		{
			DecRef();

			m_Instance = Other.m_Instance;
			Other.m_Instance = nullptr;
			return *this;
		}

		operator bool() { return m_Instance != nullptr; }
		operator bool() const { return m_Instance != nullptr; }

		T* operator->() { return m_Instance; }
		const T* operator->() const { return m_Instance; }

		T& operator*() { return *m_Instance; }
		const T& operator*() const { return *m_Instance; }

		T* Raw() { return  m_Instance; }
		const T* Raw() const { return  m_Instance; }

		void Reset(T* instance = nullptr)
		{
			DecRef();
			m_Instance = instance;
		}

		template<typename T2>
		Ref<T2> As() const
		{
			return Ref<T2>(*this);
		}

		template<typename... Args>
		static Ref<T> Create(Args&&... args)
		{
			return Ref<T>(new T(std::forward<Args>(args)...));
		}

		bool operator==(const Ref<T>& Other) const { return m_Instance == Other.m_Instance; }
		bool operator!=(const Ref<T>& Other) const { return !(*this == Other); }

		bool operator==(T& Other) const { return (*this) == Other; }
		bool operator==(const T& Other) const { return (*this) == Other; }

		bool IsNullptr() const
		{
			return m_Instance == nullptr;
		}

		bool EqualsObject(const Ref<T>& Other) const
		{
			if (m_Instance != nullptr || Other.m_Instance != nullptr)
			{
				return false;
			}
			
			return *m_Instance == *Other.m_Instance;
		}

	private:
		void IncRef() const
		{
			if (m_Instance != nullptr)
			{
				m_Instance->IncRefCount();
				_RefInternal::AddToLiveReferences((void*)m_Instance);
			}
		}

		void DecRef() const
		{
			if (m_Instance != nullptr)
			{
				m_Instance->DecRefCount();
				
				if (m_Instance->GetRefCount() == 0)
				{
					delete m_Instance;
					_RefInternal::RemoveFromLiveReferences((void*)m_Instance);
					m_Instance = nullptr;
				}
			}
		}

	private:
		template<class T2>
		friend class Ref;

		mutable T* m_Instance;
	};
	
}
