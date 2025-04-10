/**
 * @file
 * @brief String utilities.
 */
#pragma once

/**
 * @ingroup Core
 * @defgroup StringUtility String Utility
 */

#include "LkEngine/Core/CoreMacros.h"

#include <algorithm>
#include <bitset>
#include <cctype>
#include <codecvt>
#include <locale>
#include <stdint.h>
#include <string>
#include <string_view>

/**
 * @brief String utility.
 * Contains various utility functions for strings.
 *
 * @ingroup StringUtility
 * @{
 */
namespace LkEngine::StringUtils {

	[[nodiscard]] static constexpr std::string_view RemovePrefix(std::string_view Str, char Prefix = 'L')
	{
		return (Str.starts_with(Prefix)) ? Str.substr(1) : Str;
	}

	/**
	 * @brief Convert a string to lowercase.
	 * @tparam TChar Character type.
	 * @tparam StringType String type (std::string, std::wstring, ...)
	 * @param Input String to convert to lowercase.
	 * @return The input string in lowercase.
	 *
	 * @note Should add support for converting the existing string reference instead of creating a string copy.
	 */
	template<typename TChar = char, typename StringType>
	[[nodiscard]] static std::basic_string<TChar> ToLower(const StringType& Input)
	{
		std::basic_string_view<TChar> StringView(Input);
		std::basic_string<TChar> Result(StringView);

		std::transform(Result.begin(), Result.end(), Result.begin(), [](TChar Character)
		{
			return static_cast<TChar>(std::tolower(Character, std::locale{}));
		});

		return Result;
	}

	template<typename TChar = char, typename StringType>
	[[nodiscard]] static std::basic_string<TChar> ToUpper(const StringType& Input)
	{
		std::basic_string_view<TChar> StringView(Input);
		std::basic_string<TChar> Result(StringView);

		std::transform(Result.begin(), Result.end(), Result.begin(), [](TChar Character)
		{
			return static_cast<TChar>(std::toupper(Character, std::locale{}));
		});

		return Result;
	}

	[[nodiscard]] static std::string ToLower(const char* String)
    {
        return ToLower<char>(std::string_view(String));
    }

    [[nodiscard]] static std::string ToUpper(const char* String)
    {
        return ToUpper<char>(std::string_view(String));
    }

    [[nodiscard]] static std::wstring ToLower(const wchar_t* String)
    {
        return ToLower<wchar_t>(std::wstring_view(String));
    }

    [[nodiscard]] static std::wstring ToUpper(const wchar_t* String)
    {
        return ToUpper<wchar_t>(std::wstring_view(String));
    }

    [[nodiscard]] static std::wstring ToLower(const std::wstring& String)
    {
        return ToLower<wchar_t>(String);
    }

    [[nodiscard]] static std::wstring ToUpper(const std::wstring& String)
    {
		return ToUpper<wchar_t>(String);
    }

	[[nodiscard]] static std::string BytesToString(const uint64_t Bytes)
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
	[[nodiscard]] static TargetString Convert(const SourceString& Input)
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

	static void ReplaceToken(std::string& String, const char* Token, const std::string& Value)
	{
		std::size_t Pos = 0;
		while ((Pos = String.find(Token, Pos)) != std::string::npos)
		{
			String.replace(Pos, strlen(Token), Value);
			Pos += strlen(Token);
		}
	}

	template<typename T>
	static std::string ConvertFlags(const T Flags)
	{
		static_assert(std::is_integral_v<T> || std::is_enum_v<T>, "Cannot convert non-integral types, only flags are allowed");
		constexpr uint32_t Bits = static_cast<uint32_t>(sizeof(decltype(Flags)) * 8);
		std::bitset<Bits> FlagBitset(Flags);
		return FlagBitset.to_string();
	}

}

/** @} */
