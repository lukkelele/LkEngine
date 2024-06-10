#pragma once

#include <stddef.h>
#include <stdint.h>
#include <cstring>


namespace LkEngine {

	namespace EncodingFormat
	{
		enum Type : int
		{
			Null = 0,
			Base64,
			Hex,
		};
	}

}