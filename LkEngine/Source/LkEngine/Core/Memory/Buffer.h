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
			std::memcpy(Buffer.Data, Other.Data, Other.Size);

			return Buffer;
		}

		FORCEINLINE static FBuffer Copy(const void* Data, const uint64_t Size)
		{
			FBuffer Buffer;
			Buffer.Allocate(Size);
			std::memcpy(Buffer.Data, Data, Size);

			return Buffer;
		}

		FORCEINLINE void Allocate(const uint64_t InSize)
		{
			LK_CORE_ASSERT(InSize > 0, "Allocate failed, invalid size: {}", InSize);

			delete[](byte*)Data;
			Data = nullptr;

			Data = new byte[InSize];
			Size = InSize;
		}

		FORCEINLINE void Release()
		{
			if (Data)
			{
				delete[](byte*)Data;
				Data = nullptr;
				Size = 0;
			}
		}

		FORCEINLINE void ZeroInitialize()
		{
			if (Size > 0)
			{
				std::memset(Data, 0, Size);
			}
		}

		template<typename T>
		FORCEINLINE T& Read(const uint64_t Offset = 0)
		{
			return *(T*)(static_cast<byte*>(Data) + Offset);
		}

		template<typename T>
		FORCEINLINE const T& Read(uint64_t Offset = 0) const
		{
			return *(T*)(static_cast<byte*>(Data) + Offset);
		}

		FORCEINLINE byte* ReadBytes(const uint64_t ReadSize, const uint64_t Offset) const
		{
			LK_CORE_ASSERT(Offset + ReadSize <= ReadSize, "Buffer overflow");
			byte* Buffer = new byte[ReadSize];
			std::memcpy(Buffer, (byte*)Data + Offset, ReadSize);

			return Buffer;
		}

		FORCEINLINE void Write(const void* InData, const uint64_t WriteSize, uint64_t Offset = 0)
		{
			LK_CORE_ASSERT(Offset + WriteSize <= Size, "FBuffer overflow");
			std::memcpy((byte*)Data + Offset, InData, WriteSize);
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

		FORCEINLINE uint64_t GetSize() const { return Size; }

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

		static FBufferSafe Copy(const void* Data, const uint64_t Size)
		{
			FBufferSafe Buffer;
			Buffer.Allocate(Size);
			std::memcpy(Buffer.Data, Data, Size);

			return Buffer;
		}
	};
}
