/******************************************************************
 * Time
 *
 *******************************************************************/ 
#pragma once

#include <fstream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>

#include "LkEngine/Core/CoreMacros.h"


namespace LkEngine::Time {

	/**
	 * @brief Get the current timestamp.
	 */
    FORCEINLINE static std::string CurrentTimestamp(std::string_view InFormat = "%Y-%m-%d-%H%M%S")
    {
        using namespace std::chrono;
        time_point<system_clock> Now = system_clock::now();
        std::time_t CurrentTime = system_clock::to_time_t(Now);

        std::stringstream StrStream;
        StrStream << std::put_time(std::localtime(&CurrentTime), InFormat.data());

        return StrStream.str();
    }

}
