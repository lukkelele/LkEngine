#pragma once

#include "LkEngine/Core/Core.h"


namespace LkEngine {

	struct FBuffer
	{
		explicit FBuffer(const void* InData, const uint64_t InSize)
			: Data((void*)InData)
			, Size(InSize)
		{
		}

		FBuffer()
			: Data(nullptr)
			, Size(0)
		{
		}

		~FBuffer()
		{
			Release();
		}

		FBuffer(const FBuffer& Other)
			: Data(nullptr)
			, Size(Other.Size)
		{
			if (Other.Data && (Other.Size > 0))
			{
				/* Perform deep copy. */
				Allocate(Other.Size);
				if (Data)
				{
					memcpy(Data, Other.Data, Other.Size);
				}
			}
		}

		FBuffer(FBuffer&& Other) noexcept
			: Data(Other.Data)
			, Size(Other.Size)
		{
			/* Do NOT invoke Release here to prevent double call to 'free'. */
			Other.Data = nullptr;
			Other.Size = 0;
		}

		FBuffer& operator=(const FBuffer& Other) noexcept
		{
			if (this != &Other)
			{
				/* Release current buffer if it exists. */
				Release();

				if (Other.Data && Other.Size > 0)
				{
					/* Perform deep copy. */
					Allocate(Other.Size);
					memcpy(Data, Other.Data, Other.Size);
					Size = Other.Size;
				}
				else
				{
					Data = nullptr;
					Size = 0;
				}
			}

			return *this;
		}

		FBuffer& operator=(FBuffer&& Other) noexcept
		{
			if (this != &Other)
			{
				delete[](byte*)Data;

				/* Transfer ownership. */
				Allocate(Other.Size);
				memcpy(Data, Other.Data, Other.Size);

				Size = Other.Size;

				Other.Data = nullptr;
				Other.Size = 0;
			}

			return *this;
		}

		FORCEINLINE static FBuffer Copy(const FBuffer& Other)
		{
			FBuffer Buffer;
			Buffer.Allocate(Other.Size);
			memcpy(Buffer.Data, Other.Data, Other.Size);

			return Buffer;
		}

		FORCEINLINE static FBuffer Copy(const void* data, uint64_t size)
		{
			FBuffer buffer;
			buffer.Allocate(size);
			memcpy(buffer.Data, data, size);
			return buffer;
		}

		void Allocate(const uint64_t InSize)
		{
			LK_CORE_ASSERT(InSize > 0, "Allocate failed, invalid size: {}", InSize);

			delete[](byte*)Data;
			Data = nullptr;

			Data = new byte[InSize];
			Size = InSize;
		}

		void Release()
		{
			if (Data)
			{
				delete[](byte*)Data;
				Data = nullptr;
				Size = 0;
			}
		}

		void ZeroInitialize()
		{
			if (Size > 0)
			{
				memset(Data, 0, Size);
			}
		}

		template<typename T>
		T& Read(uint64_t offset = 0)
		{
			return *(T*)((byte*)Data + offset);
		}

		template<typename T>
		const T& Read(uint64_t offset = 0) const
		{
			return *(T*)((byte*)Data + offset);
		}

		byte* ReadBytes(uint64_t size, uint64_t offset) const
		{
			LK_CORE_ASSERT(offset + size <= Size, "FBuffer overflow!");
			byte* buffer = new byte[size];
			memcpy(buffer, (byte*)Data + offset, size);
			return buffer;
		}

		void Write(const void* data, uint64_t size, uint64_t offset = 0)
		{
			LK_CORE_ASSERT(offset + size <= Size, "FBuffer overflow!");
			memcpy((byte*)Data + offset, data, size);
		}

		operator bool() { return (Data && (Size > 0)); }
		operator bool() const { return (Data && (Size > 0)); }

		byte& operator[](int index)
		{
			return ((byte*)Data)[index];
		}

		byte operator[](int index) const
		{
			return ((byte*)Data)[index];
		}

		template<typename T>
		T* As() const
		{
			return (T*)Data;
		}

		inline uint64_t GetSize() const { return Size; }

	public:
		void* Data = nullptr;
		uint64_t Size = 0;
	};

	struct FBufferSafe : public FBuffer
	{
		~FBufferSafe()
		{
			Release();
		}

		static FBufferSafe Copy(const void* data, uint64_t size)
		{
			FBufferSafe buffer;
			buffer.Allocate(size);
			memcpy(buffer.Data, data, size);
			return buffer;
		}
	};
}
