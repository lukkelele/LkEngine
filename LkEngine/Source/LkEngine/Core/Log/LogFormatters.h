/****************************************************************
 * Log Formatters
 *
 ****************************************************************/
#pragma once

#include <stdint.h>
#include <codecvt>
#include <format>
#include <filesystem>
#include <locale>
#include <string>


/**
 * Formatter: std::wstring
 */
template<> 
struct std::formatter<std::wstring>
{
	template<typename ParseContext>
    constexpr auto parse(ParseContext& Context)
    {
        return Context.begin();
    }

    template<typename FormatContext>
    auto format(const std::wstring& WideString, FormatContext& Context) const
    {
		const std::string NarrowString = std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.to_bytes(WideString);
        return std::format_to(Context.out(), "{}", NarrowString);
    }
};

/**
 * Formatter: const wchar_t*
 */
template<>
struct std::formatter<const wchar_t*> 
{
	template<typename ParseContext>
    constexpr auto parse(ParseContext& Context)
    {
        return Context.begin();
    }
	
	template<typename FormatContext>
    auto format(const wchar_t* WideString, FormatContext& Context) const
    {
        if (!WideString)
        {
            return std::format_to(Context.out(), "(NULL)");
        }

		const std::string NarrowString = std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.to_bytes(WideString);
        return std::format_to(Context.out(), "{}", NarrowString);
    }
};

/**
 * Formatter: std::filesystem::path
 */
template<>
struct std::formatter<std::filesystem::path>
{
	template<typename ParseContext>
    constexpr auto parse(ParseContext& Context)
    {
        return Context.begin();
    }
	
	template<typename FormatContext>
    auto format(const std::filesystem::path& Input, FormatContext& Context) const
    {
        return std::format_to(Context.out(), "{}", Input.generic_string());
    }
};

