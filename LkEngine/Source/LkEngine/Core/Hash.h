#pragma once

#include "LkEngine/Core/Core.h"


namespace LkEngine {

	class LHash
	{
	public:
		FORCEINLINE static constexpr uint32_t GenerateFNVHash(std::string_view String)
		{
			constexpr uint32_t FNV_PRIME = 16777619u;
			constexpr uint32_t OFFSET_BASIS = 2166136261u;

			const std::size_t Length = String.length();
			const char* Data = String.data();

			uint32_t Hash = OFFSET_BASIS;
			for (std::size_t i = 0; i < Length; i++)
			{
				Hash ^= *Data++;
				Hash *= FNV_PRIME;
			}
			Hash ^= '\0';
			Hash *= FNV_PRIME;

			return Hash;
		}

		static uint32_t CRC32(const char* String);
		static uint32_t CRC32(const std::string& String);
	};

}
