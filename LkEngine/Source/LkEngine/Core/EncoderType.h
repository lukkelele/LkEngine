#pragma once

#include <stddef.h>
#include <stdint.h>
#include <cstring>


namespace LkEngine {

	enum class EEncodingFormat : int
	{
		Null = 0,
		Base64,
		Hex,
	};

}
