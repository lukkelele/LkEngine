#pragma once

#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>


namespace LkEngine {

	namespace ECastCheckedType
	{
		enum Type
		{
			NullAllowed, /** Null is okay, only assert on incorrect type */
			NullChecked  /** Null is not allowed, assert on incorrect type or null */
		};
	};

}