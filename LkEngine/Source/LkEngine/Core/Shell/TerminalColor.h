#pragma once

#include <stdint.h>
#include <string_view>


namespace LkEngine::Color
{
	namespace Console
	{
		namespace Foreground
		{
			constexpr std::string_view Black   = "\033[30m";
			constexpr std::string_view Red     = "\033[31m";
			constexpr std::string_view Green   = "\033[32m";
			constexpr std::string_view Yellow  = "\033[33m";
			constexpr std::string_view Blue    = "\033[34m";
			constexpr std::string_view Magenta = "\033[35m";
			constexpr std::string_view Cyan    = "\033[36m";
			constexpr std::string_view White   = "\033[37m";
		}

		namespace Background 
		{
			constexpr std::string_view OnBlack   = "\033[40m";
			constexpr std::string_view OnRed     = "\033[41m";
			constexpr std::string_view OnGreen   = "\033[42m";
			constexpr std::string_view OnYellow  = "\033[43m";
			constexpr std::string_view OnBlue    = "\033[44m";
			constexpr std::string_view OnMagenta = "\033[45m";
			constexpr std::string_view OnCyan    = "\033[46m";
			constexpr std::string_view OnWhite   = "\033[47m";
		}

		namespace Bold 
		{
			constexpr std::string_view Yellow    = "\033[33m\033[1m";
			constexpr std::string_view Red       = "\033[31m\033[1m";
		}

		namespace Formatter 
		{
			constexpr std::string_view Reset      = "\033[m";
			constexpr std::string_view Bold       = "\033[1m";
			constexpr std::string_view Dark       = "\033[2m";
			constexpr std::string_view Underline  = "\033[4m";
			constexpr std::string_view Blink      = "\033[5m";
			constexpr std::string_view Reverse    = "\033[7m";
			constexpr std::string_view Concealed  = "\033[8m";
			constexpr std::string_view Clear_line = "\033[K";
		}

	}

}
