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
        //std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
        time_point<system_clock> now = system_clock::now();
        std::time_t currentTime = system_clock::to_time_t(now);

        std::stringstream timestampStream;
        timestampStream << std::put_time(std::localtime(&currentTime), InFormat.data());

        return timestampStream.str();
    }

}
