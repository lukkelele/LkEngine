#pragma once

#include <string>
#include <ctime>

#include "LkEngine/Core/CoreMacros.h"


namespace LkEngine {

	enum class EConsoleVerbosity : int16_t
	{
		None    = -1,
		Debug   = LK_BIT(0),
		Info    = LK_BIT(1),
		Warning = LK_BIT(2),
		Error   = LK_BIT(3),

		All = Debug | Info | Warning | Error
	};
	LK_ENUM_CLASS_FLAGS(EConsoleVerbosity);

	/**
	 * FConsoleMessage
	 *
	 *  Message visible in the editor console.
	 */
	struct FConsoleMessage
	{
		std::string ShortMessage{};
		std::string LongMessage{};
		int16_t Flags = (std::underlying_type_t<EConsoleVerbosity>)EConsoleVerbosity::None;

		std::time_t Time;
	};
	
}
