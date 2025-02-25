#pragma once

#include "LkEngine/Core/CoreMacros.h"
#include "LkEngine/Core/CoreTypes.h"


namespace LkEngine {

	enum class ECameraProjection : uint8_t
	{ 
		Perspective, 
		Orthographic 
	};

	enum class ECameraType 
	{ 
		None = 0, 
		Scene = 1, 
		Editor = 2 
	};

	namespace Enum {

		FORCEINLINE static constexpr const char* ToString(const ECameraProjection CameraProjection)
		{
			switch (CameraProjection)
			{
				case ECameraProjection::Perspective:  return "Perspective";
				case ECameraProjection::Orthographic: return "Orthographic";
			}

			LK_CORE_VERIFY(false);
			return nullptr;
		}

		FORCEINLINE static constexpr const char* ToString(const ECameraType CameraType)
		{
			switch (CameraType)
			{
				case ECameraType::Scene:  return "Scene";
				case ECameraType::Editor: return "Editor";
				case ECameraType::None:   return "None";
			}

			LK_CORE_VERIFY(false);
			return nullptr;
		}
		
	}

}
