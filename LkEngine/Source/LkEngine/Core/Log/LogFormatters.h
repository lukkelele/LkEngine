/**
 * @file
 * @brief Log formatters.
 */
#pragma once

#include <stdint.h>
#include <codecvt>
#if defined(LK_PLATFORM_WINDOWS)
#   include <format>
#elif defined(LK_PLATFORM_LINUX)
#   include <spdlog/fmt/fmt.h>
#endif
#include <filesystem>
#include <locale>
#include <string>

#include <glm/glm.hpp>

#include "LkEngine/Core/Hash/UUID.h"

/**
 * @ingroup Core
 * @{
 */

/**
 * @ingroup Log
 * @{
 */

/**
 * @struct Formatter: std::wstring
 */
template<> 
struct LK_FMT_LIB::formatter<std::wstring>
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
        return LK_FMT_LIB::format_to(Context.out(), "{}", NarrowString);
    }
};

/**
 * @struct Formatter: const wchar_t*
 */
template<>
struct LK_FMT_LIB::formatter<const wchar_t*> 
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
            return LK_FMT_LIB::format_to(Context.out(), "(NULL)");
        }

		const std::string NarrowString = std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.to_bytes(WideString);
        return LK_FMT_LIB::format_to(Context.out(), "{}", NarrowString);
    }
};

/**
 * @struct Formatter: std::filesystem::path
 */
template<>
struct LK_FMT_LIB::formatter<std::filesystem::path>
{
	template<typename ParseContext>
    constexpr auto parse(ParseContext& Context)
    {
        return Context.begin();
    }
	
	template<typename FormatContext>
    auto format(const std::filesystem::path& Input, FormatContext& Context) const
    {
        return LK_FMT_LIB::format_to(Context.out(), "{}", Input.generic_string());
    }
};

/**
 * @struct Formatter: std::array<char, N>
 */
template<std::size_t N>
struct LK_FMT_LIB::formatter<std::array<char, N>>
{
	template<typename ParseContext>
    constexpr auto parse(ParseContext& Context)
    {
        return Context.begin();
    }
	
	template<typename FormatContext>
    auto format(const std::array<char, N>& Input, FormatContext& Context) const
    {
        return LK_FMT_LIB::format_to(Context.out(), "{}", Input.data());
    }
};

/**
 * @struct Formatter: glm::vec2
 */
template<>
struct LK_FMT_LIB::formatter<glm::vec2>
{
	template<typename ParseContext>
    constexpr auto parse(ParseContext& Context)
    {
        return Context.begin();
    }
	
	template<typename FormatContext>
    auto format(const glm::vec2& Input, FormatContext& Context) const
    {
        return LK_FMT_LIB::format_to(Context.out(), "({:.2f}, {:.2f})", Input.x, Input.y);
    }
};

/**
 * @struct Formatter: glm::vec3
 */
template<>
struct LK_FMT_LIB::formatter<glm::vec3>
{
	template<typename ParseContext>
    constexpr auto parse(ParseContext& Context)
    {
        return Context.begin();
    }
	
	template<typename FormatContext>
    auto format(const glm::vec3& Input, FormatContext& Context) const
    {
        return LK_FMT_LIB::format_to(Context.out(), "({:.2f}, {:.2f}, {:.2f})", Input.x, Input.y, Input.z);
    }
};

/**
 * @struct Formatter: glm::vec4
 */
template<>
struct LK_FMT_LIB::formatter<glm::vec4>
{
	template<typename ParseContext>
    constexpr auto parse(ParseContext& Context)
    {
        return Context.begin();
    }
	
	template<typename FormatContext>
    auto format(const glm::vec4& Input, FormatContext& Context) const
    {
        return LK_FMT_LIB::format_to(Context.out(), "({:.2f}, {:.2f}, {:.2f}, {:.2f})", Input.x, Input.y, Input.z, Input.z);
    }
};

/**
 * @struct Formatter: glm::vec4
 */
template<> 
struct LK_FMT_LIB::formatter<LkEngine::LUUID> : LK_FMT_LIB::formatter<std::string>
{
	template<typename ParseContext>
    constexpr auto parse(ParseContext& Context)
    {
        return Context.begin();
    }

    template<typename FormatContext>
    auto format(const LkEngine::LUUID& Uuid, FormatContext& Context) const
    {
		return LK_FMT_LIB::format_to(Context.out(), "{}", static_cast<::LkEngine::LUUID::SizeType>(Uuid));
    }
};
/** @} */

/** @} */
