#pragma once

#include "LkEngine/Core/Core.h"

namespace LkEngine {

	/**
	 * @ingroup Core
	 *
	 * @defgroup Hash Hash Algorithms
	 * @{
	 */

	/**
	 * @enum EHash
	 */
	enum class EHash
	{
		None = 0,
		FNV, /* Fowler-Noll-Vo */
	};

	/**
	 * @enum EChecksum
	 */
	enum class EChecksum
	{
		None = 0,
		Crc32,
	};

	/**
	 * @class LHash
	 */
	class LHash
	{
	private:
		static constexpr std::uint32_t FNV_PRIME = 16777619u;
		static constexpr std::uint32_t FNV_OFFSET_BASIS = 2166136261u;

		template<std::size_t N = 256>
		static constexpr std::array<std::uint32_t, N> GenerateTableCrc32()
		{
			constexpr int NumIterations = 8;
			constexpr std::uint32_t Polynomial = 0xEDB88320;

			std::array<std::uint32_t, N> TableCrc32{};
			for (int Byte = 0; Byte < N; ++Byte)
			{
				std::uint32_t Crc = static_cast<std::uint32_t>(Byte);
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
		inline static const auto TableCrc32 = GenerateTableCrc32<256>();
	#elif defined(LK_ENGINE_GCC) || defined(LK_ENGINE_CLANG)
		inline static auto TableCrc32 = GenerateTableCrc32<256>();
	#endif

	public:
		template<EHash Type>
		static constexpr std::uint32_t Generate(std::string_view String)
		{
			return 0;
		}

		template<EChecksum Type>
		static constexpr std::uint32_t GenerateChecksum(std::string_view String)
		{
			return 0;
		}
	};

	/**
	 * @brief Generate hash.
	 *
	 * @details Algorithm: FNV.
	 *          Fast and efficient hash function.
	 */
	template<>
	constexpr uint32_t LHash::Generate<EHash::FNV>(std::string_view String)
	{
		const std::size_t Length = String.length();
		const char* Data = String.data();

		std::uint32_t Hash = FNV_OFFSET_BASIS;
		for (std::size_t i = 0; i < Length; i++)
		{
			Hash ^= *Data++;
			Hash *= FNV_PRIME;
		}
		Hash ^= '\0';
		Hash *= FNV_PRIME;

		return Hash;
	}

	/**
	 * @brief Generate a checksum based on a string input.
	 */
	template<>
	constexpr std::uint32_t LHash::GenerateChecksum<EChecksum::Crc32>(std::string_view String)
	{
		std::uint32_t Crc = 0xFFFFFFFFUL;
		for (std::uint32_t Index = 0; char Character = String[Index]; Index++)
		{
			Crc = TableCrc32[(Crc ^ Character) & 0xFF] ^ (Crc >> 8);
		}

		return ~Crc;
	}

	/** @} */

}
