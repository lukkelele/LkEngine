#pragma once

#include "LkEngine/Core/Core.h"

#include "TextureEnum.h"
#include "VertexBuffer.h"
#include "Framebuffer.h"
#include "Shader.h"


namespace LkEngine {

	class LFramebuffer;
	class LShader;
	struct FVertexBufferLayout;

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