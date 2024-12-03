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

	namespace Global 
	{
		void SetRuntimeArguments(const int InArgc, char* Argv[]);

		const FRuntimeArguments& GetRuntimeArguments();

		/**
		 * @brief Get the working directory.
		 */
		std::filesystem::path GetWorkingDir();

		/**
		 * @brief Get the binary executable directory.
		 */
		std::filesystem::path GetBinaryDir();

		/**
		 * @brief Get the engine config.
		 */
		std::filesystem::path GetEngineConfig();

	}

}
