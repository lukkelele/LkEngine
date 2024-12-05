#pragma once

#include "LkEngine/Core/CoreMacros.h"
#include "LkEngine/Core/CoreTypes.h"

#include <sstream>
#include <string>


namespace LkEngine {

	class LVersion
	{
	public:
		static constexpr uint8_t GetMajor(uint32_t EncodedVersion)
		{
			return (EncodedVersion >> 24) & 0xFF;
		}

		static constexpr uint8_t GetMinor(uint32_t EncodedVersion)
		{
			return (EncodedVersion >> 16) & 0xFF;
		}

		static constexpr uint16_t GetPatch(uint32_t EncodedVersion)
		{
			return EncodedVersion & 0xFFFF;
		}

		static std::string ToString(const uint32_t EncodedVersion)
		{
			std::ostringstream Stream;
			Stream << static_cast<int>(GetMajor(EncodedVersion)) << "."
				   << static_cast<int>(GetMinor(EncodedVersion)) << "."
				   << GetPatch(EncodedVersion);
			return Stream.str();
		}
	};

}
