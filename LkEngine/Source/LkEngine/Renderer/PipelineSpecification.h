#pragma once

#include "LkEngine/Core/Core.h"

#include "VertexBuffer.h"
#include "Framebuffer.h"
#include "Shader.h"


namespace LkEngine {

	class LFramebuffer;
	class LShader;
	struct FVertexBufferLayout;

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

	/**
	 * FPipelineSpecification
	 */
	struct FPipelineSpecification
	{
		TObjectPtr<LShader> Shader = nullptr;
		TObjectPtr<LFramebuffer> TargetFramebuffer = nullptr;

		/* TODO: Investigate the forward declaration of FVertexBufferLayout here. */
		FVertexBufferLayout Layout;
		FVertexBufferLayout InstanceLayout;
		FVertexBufferLayout BoneInfluenceLayout;

		EPrimitiveTopology Topology = EPrimitiveTopology::Triangles;
		EDepthCompareOperator DepthOperator = EDepthCompareOperator::GreaterOrEqual;

		bool BackfaceCulling = true;
		bool DepthTest = true;
		bool DepthWrite = true;
		bool Wireframe = false;
		float LineWidth = 1.0f;

		std::string DebugName{};

		ERendererAPI RenderAPI = ERendererAPI::None;

		FORCEINLINE void SetLayout(const FVertexBufferLayout& InLayout) 
		{ 
			Layout = InLayout; 
		}
	};

}