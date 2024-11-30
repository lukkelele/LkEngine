#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <string_view>
#include <locale>
#include <codecvt>
#include <cassert>

#include "LkEngine/Core/Assert.h"


namespace LkEngine {

    /** Returns char value of Nibble */
	[[nodiscard]] inline char NibbleToChar(uint8_t Num)
	{
		if (Num > 9)
		{
			return (char)('A' + (Num - 10));
		}
		return (char)('0' + Num);
	}

}
