/******************************************************************
 * Globals
 *
 *******************************************************************/
#pragma once

#include <filesystem>

#include "LkEngine/Core/CoreMacros.h"

namespace LkEngine {

	struct FRuntimeArguments
	{
		int Argc = 0;
		char** Argv{};

		std::vector<std::string> Args{};
	};

	struct Global
	{
		/**
		 * @brief Runtime arguments passed to main.
		 */
		static void SetRuntimeArguments(const int Argc, char* Argv[]);
		static const FRuntimeArguments& GetRuntimeArguments();
	};

}
