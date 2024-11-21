#pragma once

#include "LkEngine/Core/CoreTypes.h"
#include "LkEngine/Renderer/TextureEnum.h"


namespace LkEngine {

	/**
	 * FTextureArraySpecification
	 *
	 *  Specification for the construction of an array texture.
	 */
	struct FArrayTextureSpecification
	{
		EArrayTextureDimension Dimension = EArrayTextureDimension::Dim_1024x1024;
		EImageFormat ImageFormat = EImageFormat::RGBA;
		int TextureSlot = 0;
		int Layers = 10;
		int Mipmaps = 5;

		std::string DebugName{};
	};

}
