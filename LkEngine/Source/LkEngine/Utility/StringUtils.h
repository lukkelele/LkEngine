#pragma once

#include <stdint.h>
#include <string>


namespace LkEngine::Utils {

    namespace String
    {
	    std::string ToLower(const std::string_view& string);
	    std::string ToUpper(const std::string_view& string);
	    std::string BytesToString(uint64_t bytes);
    }

}