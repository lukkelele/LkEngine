#pragma once

#include <stdint.h>
#include <string>


namespace LkEngine {

	struct FRendererCapabilities
	{
		std::string Vendor{};
		std::string Device{};
		std::string Version{};

		int MaxSamples = 0;
		float MaxAnisotropy = 0.0f;
		int MaxTextureUnits = 0;
	};

}
