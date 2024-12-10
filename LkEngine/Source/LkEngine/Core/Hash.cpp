#include "LKpch.h"
#include "Hash.h"


static constexpr auto GenerateTableCrc32()
{
    constexpr int NumBytes = 256;
    constexpr int NumIterations = 8;
    constexpr uint32_t Polynomial = 0xEDB88320;

    std::array<uint32_t, NumBytes> TableCrc32{};
    for (int byte = 0; byte < NumBytes; ++byte)
	{
		uint32_t crc = (uint32_t)byte;
        for (int i = 0; i < NumIterations; ++i)
		{
			int mask = -((int)crc & 1);
            crc = (crc >> 1) ^ (Polynomial & mask);
        }

        TableCrc32[byte] = crc;
    }

    return TableCrc32;
}

static constexpr auto TableCrc32 = GenerateTableCrc32();
static_assert(
    TableCrc32.size() == 256 &&
    TableCrc32[1]     == 0x77073096 &&
    TableCrc32[255]   == 0x2D02EF8D,
    "GenerateTableCrc32 generated unexpected result."
);


namespace LkEngine {

    uint32_t LHash::CRC32(const char* String)
    {
        uint32_t CRC = 0xFFFFFFFFu;
        for (uint32_t Index = 0u; char Character = String[Index]; Index++)
        {
			static_assert(std::is_same_v<uint32_t, decltype(Index)>, "Invalid type for 'i'");
            CRC = TableCrc32[(CRC ^ Character) & 0xFF] ^ (CRC >> 8);
        }

        return ~CRC;
    }

    uint32_t LHash::CRC32(const std::string& InString)
    {
        return CRC32(InString.c_str());
    }

}

