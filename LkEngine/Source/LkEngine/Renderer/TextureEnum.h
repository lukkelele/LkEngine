#pragma once

#include "LkEngine/Core/CoreTypes.h"
#include "LkEngine/Core/Assert.h"
#include "LkEngine/Core/Log.h" /* FIXME: Move to Assert.h */


namespace LkEngine {

	/**
	 * @enum EImageFormat
	 */
	enum class EImageFormat
	{
		None = 0,
		RED8UN,
		RED8UI,
		RED16UI,
		RED32UI,
		RED32F,
		RG8,
		RG16F,
		RG32F,
		RGB,
		RGBA,

		RGB8,
		RGBA8,

		RGBA16F,
		RGBA32F,

		B10R11G11UF,

		SRGB,
		SRGBA,

		DEPTH32FSTENCIL8UINT,
		DEPTH32F,
		DEPTH24STENCIL8,

		Depth = DEPTH24STENCIL8,
	};

	/**
	 * @enum EImageUsage
	 */
	enum class EImageUsage
	{
		None = 0,
		Texture,
		Attachment,
		Storage,
	};

	/**
	 * @enum ETextureWrap
	 */
	enum class ETextureWrap
	{
		None = 0,
		Clamp,
		Repeat
	};

	/**
	 * @enum ETextureFilter
	 */
	enum class ETextureFilter
	{
		None = 0,
		Linear,
		Nearest
	};

	/**
	 * @enum ETextureAnistropicFiltering
	 */
	enum class ETextureType
	{
		None = 0,
		Texture2D,
		TextureCube
	};

	/**
	 * @enum ETextureAnistropicFiltering
	 */
	enum class ETextureAnistropicFiltering
	{
		None = 0,
		Bilnear,
		Trilnear
	};

	/**
	 * @enum ETextureUniformType
	 */
	enum class ETextureUniformType : uint8_t
	{
		Diffuse = 0,
		Specular,
		Normal,
		Height,
		Emissive,
		DiffuseRoughness,
	};

	/**
	 * @enum ETextureArrayDimension
	 */
	enum class ETextureArrayDimension
	{
		Dim_200x200,
		Dim_512x512,
		Dim_1024x1024,
		Dim_2048x2048,
		Dim_4096x4096,
	};

	enum class EPrimitiveTopology
	{
		None = 0,
		Points,
		Lines,
		Triangles,
		LineStrip,
		TriangleStrip,
		TriangleFan
	};

	enum class EDepthCompareOperator
	{
	    None = 0,
		Never,
		NotEqual,
	    Less,
	    LessOrEqual,
	    Greater,
	    GreaterOrEqual,
		Equal,
	    Always,
	};


	namespace Enum
	{
		inline static constexpr const char* ToString(const EDepthCompareOperator CompareOperator)
		{
			switch (CompareOperator)
			{
				case EDepthCompareOperator::None:			return "None";
				case EDepthCompareOperator::Never:			return "Never";
				case EDepthCompareOperator::NotEqual:		return "NotEqual";
				case EDepthCompareOperator::Less:			return "Less";
				case EDepthCompareOperator::LessOrEqual:	return "LessOrEqual";
				case EDepthCompareOperator::Greater:		return "Greater";
				case EDepthCompareOperator::GreaterOrEqual:	return "GreaterOrEqual";
				case EDepthCompareOperator::Equal:			return "Equal";
				case EDepthCompareOperator::Always:			return "Always";
			}

			LK_CORE_ASSERT(false, "Invalid EDepthCompareOperator value");
			return nullptr;
		}

		inline static constexpr const char* ToString(const EPrimitiveTopology Topology)
		{
			switch (Topology)
			{
				case EPrimitiveTopology::None:			return "None";
				case EPrimitiveTopology::Points:		return "Points";
				case EPrimitiveTopology::Lines:			return "Lines";
				case EPrimitiveTopology::Triangles:		return "Triangles";
				case EPrimitiveTopology::LineStrip:		return "LineStrip";
				case EPrimitiveTopology::TriangleStrip:	return "TriangleStrip";
				case EPrimitiveTopology::TriangleFan:	return "TriangleFan";
			}

			LK_CORE_ASSERT(false, "Invalid EPrimitiveTopology value");
			return nullptr;
		}
	}


}
