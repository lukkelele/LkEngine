/**
 * @file
 * @brief Globals.
 */
#pragma once

#include <filesystem>

#include "LkEngine/Core/CoreMacros.h"

namespace LkEngine {

	/**
	 * @defgroup Globals
	 * @ingroup Core
	 * @{
	 */

	/**
	 * @struct FRuntimeArguments
	 *
	 * @details Runtime arguments.
	 *          Contains a string vector of the passed char* Argv[] array.
	 */
	struct FRuntimeArguments
	{
		int Argc = 0;
		char** Argv{};

		std::vector<std::string> Args{};
	};

	/**
	 * @struct Global
	 */
	struct Global
	{
		/**
		 * @brief Runtime arguments passed to main.
		 */
		static void SetRuntimeArguments(const int Argc, char* Argv[]);

		static const FRuntimeArguments& GetRuntimeArguments();
	};

	/** @} */

}
