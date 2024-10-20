/******************************************************************
 * Globals
 *
 *******************************************************************/
#pragma once

#include <filesystem>

#include "LkEngine/Core/CoreMacros.h"


namespace LkEngine::Global {

	struct FRuntimeArguments
	{
		int Argc = 0;
		char** Argv{};
	};

	void SetRuntimeArguments(const int InArgc, char* InArgv[]);
	const FRuntimeArguments& GetRuntimeArguments();

	/**
	 * @brief Get the working directory.
	 */
	std::filesystem::path GetWorkingDir();

	/**
	 * @brief Get the binary executable directory.
	 */
	std::filesystem::path GetBinaryDir();

}
