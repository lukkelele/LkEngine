#include "LKpch.h"
#include "StringUtils.h"



namespace LkEngine::Utils {

    namespace String
    {
		std::string ToLower(const std::string_view& string)
		{
			std::string result;
			for (const auto& character : string)
				result += std::tolower(character);
			return result;
		}

		std::string ToUpper(const std::string_view& string)
		{
			std::string result;
			for (const auto& character : string)
				result += std::toupper(character);
			return result;
		}

		std::string BytesToString(uint64_t bytes)
		{
			static constexpr uint64_t GB = 1024 * 1024 * 1024;
			static constexpr uint64_t MB = 1024 * 1024;
			static constexpr uint64_t KB = 1024;

			char buffer[32 + 1] {};

			if (bytes > GB)
				snprintf(buffer, 32, "%.2f GB", (float)bytes / (float)GB);
			else if (bytes > MB)
				snprintf(buffer, 32, "%.2f MB", (float)bytes / (float)MB);
			else if (bytes > KB)
				snprintf(buffer, 32, "%.2f KB", (float)bytes / (float)KB);
			else
				snprintf(buffer, 32, "%.2f bytes", (float)bytes);

			return std::string(buffer);
		}


    }

}