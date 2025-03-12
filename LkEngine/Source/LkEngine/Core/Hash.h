#pragma once

#include "LkEngine/Core/Core.h"


namespace LkEngine {

	enum class EHash
	{
		None = 0,
		FNV, /* Fowler-Noll-Vo */
	};

	enum class EChecksum
	{
		None = 0,
		Crc32,
	};

	class LHash
	{
	private:
		inline static constexpr uint32_t FNV_PRIME = 16777619u;
		inline static constexpr uint32_t FNV_OFFSET_BASIS = 2166136261u;

		template<std::size_t N = 256>
		static constexpr std::array<uint32_t, N> GenerateTableCrc32()
		{
			constexpr int NumIterations = 8;
			constexpr uint32_t Polynomial = 0xEDB88320;

			std::array<uint32_t, N> TableCrc32{};
			for (int Byte = 0; Byte < N; ++Byte)
			{
				uint32_t Crc = (uint32_t)Byte;
				for (int i = 0; i < NumIterations; ++i)
				{
					const int Mask = -((int)Crc & 1);
					Crc = (Crc >> 1) ^ (Polynomial & Mask);
				}

				TableCrc32[Byte] = Crc;
			}

			return TableCrc32;
		}
	#if defined(LK_ENGINE_MSVC)
		inline static constexpr auto TableCrc32 = GenerateTableCrc32<256>();
		static_assert((
			TableCrc32.size() == 256 &&
			TableCrc32[1]     == 0x77073096 &&
			TableCrc32[255]   == 0x2D02EF8D), 
			"LHash::GenerateTableCrc32 generated an unexpected result");
	#elif defined(LK_ENGINE_GCC) || defined(LK_ENGINE_CLANG)
		inline static auto TableCrc32 = GenerateTableCrc32<256>();
	#endif

	public:
		template<EHash Type>
		static constexpr uint32_t Generate(std::string_view String)
		{
			LK_CORE_VERIFY(false);
			return 0;
		}

		template<EChecksum Type>
		static constexpr uint32_t GenerateChecksum(std::string_view String)
		{
			LK_CORE_VERIFY(false);
			return 0;
		}

	};

	/**
	 * Algorithm: FNV
	 * 
	 *  Fast and efficient hash function.
	 */
	template<>
	constexpr uint32_t LHash::Generate<EHash::FNV>(std::string_view String)
	{
		const std::size_t Length = String.length();
		const char* Data = String.data();

		uint32_t Hash = FNV_OFFSET_BASIS;
		for (std::size_t i = 0; i < Length; i++)
		{
			Hash ^= *Data++;
			Hash *= FNV_PRIME;
		}
		Hash ^= '\0';
		Hash *= FNV_PRIME;

		return Hash;
	}

	template<>
	constexpr uint32_t LHash::GenerateChecksum<EChecksum::Crc32>(std::string_view String)
	{
		uint32_t Crc = 0xFFFFFFFFu;
		for (uint32_t Index = 0u; char Character = String[Index]; Index++)
		{
			static_assert(std::is_same_v<uint32_t, decltype(Index)>, "Invalid type for 'i'");
			Crc = TableCrc32[(Crc ^ Character) & 0xFF] ^ (Crc >> 8);
		}

		return ~Crc;
	}

}
