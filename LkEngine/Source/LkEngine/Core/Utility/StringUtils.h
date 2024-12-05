#pragma once

#include "LkEngine/Core/CoreMacros.h"

#include <algorithm>
#include <cctype>
#include <codecvt>
#include <locale>
#include <stdint.h>
#include <string>
#include <string_view>


namespace LkEngine::StringUtils {

	template<typename TChar>
    static std::basic_string<TChar> ToLower(std::basic_string_view<TChar> String)
    {
        std::basic_string<TChar> Result(String);
        std::transform(Result.begin(), Result.end(), Result.begin(), [](TChar Character) 
		{
            return static_cast<TChar>(std::tolower(Character, std::locale{}));
        });

        return Result;
    }

    template<typename TChar>
    static std::basic_string<TChar> ToUpper(std::basic_string_view<TChar> String)
    {
        std::basic_string<TChar> Result(String);
        std::transform(Result.begin(), Result.end(), Result.begin(), [](TChar Character)
		{
            return static_cast<TChar>(std::toupper(Character, std::locale{}));
        });

        return Result;
    }

    static std::string ToLower(const char* String)
    {
        return ToLower(std::string_view(String));
    }

    static std::string ToUpper(const char* String)
    {
        return ToUpper(std::string_view(String));
    }

    static std::wstring ToLower(const wchar_t* String)
    {
        return ToLower(std::wstring_view(String));
    }

    static std::wstring ToUpper(const wchar_t* String)
    {
        return ToUpper(std::wstring_view(String));
    }

#if 0
	static std::string ToLower(std::string_view String)
	{
		std::string Result(String);
        std::transform(Result.begin(), Result.end(), Result.begin(), [](unsigned char Character) 
		{
            return std::tolower(Character);
        });

		return Result;
	}

	static std::string ToUpper(std::string_view String)
	{
		std::string Result(String);
        std::transform(Result.begin(), Result.end(), Result.begin(), [](unsigned char Character) 
		{
            return std::toupper(Character);
        });

        return Result;
	}
#endif

	static std::string BytesToString(const uint64_t Bytes)
	{
		static constexpr uint64_t GB = 1024 * 1024 * 1024;
		static constexpr uint64_t MB = 1024 * 1024;
		static constexpr uint64_t KB = 1024;

		static constexpr uint16_t BufSize = 32;
		char Buffer[BufSize + 1] {};

		if (Bytes > GB)
		{
			snprintf(Buffer, BufSize, "%.2f GB", (float)Bytes / (float)GB);
		}
		else if (Bytes > MB)
		{
			snprintf(Buffer, BufSize, "%.2f MB", (float)Bytes / (float)MB);
		}
		else if (Bytes > KB)
		{
			snprintf(Buffer, BufSize, "%.2f KB", (float)Bytes / (float)KB);
		}
		else
		{
			snprintf(Buffer, BufSize, "%.2f Bytes", (float)Bytes);
		}

		return std::string(Buffer);
	}

	/**
	 * @brief Convert a string to a different size, such as wide to narrow and vice versa.
	 */
	template<typename TargetString, typename SourceString>
	static TargetString Convert(const SourceString& Input)
	{
		/* Wide String to narrow. */
		if constexpr (std::is_same_v<SourceString, std::wstring> 
					  && std::is_same_v<TargetString, std::string>)
		{
			return std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.to_bytes(Input);
		}
		else if constexpr (std::is_same_v<SourceString, const wchar_t*> 
						   && std::is_same_v<TargetString, std::string>)
		{
			return std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.to_bytes(Input);
		}
		/* Narrow String to wide. */
		else if constexpr (std::is_same_v<SourceString, std::string> 
						   && std::is_same_v<TargetString, std::wstring>)
		{
			return std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.from_bytes(Input);
		}
		else if constexpr (std::is_same_v<SourceString, std::string> 
						   && std::is_same_v<TargetString, const wchar_t*>)
		{
			return std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.from_bytes(Input);
		}
		else
		{
			static_assert(sizeof(SourceString) <= 0, "Unsupported conversion");
		}
	}

}
