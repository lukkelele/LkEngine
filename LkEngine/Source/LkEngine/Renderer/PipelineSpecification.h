#pragma once

#include "LkEngine/Core/Base.h"

#include "VertexBuffer.h"
#include "Framebuffer.h"
#include "Shader.h"


namespace LkEngine {

	class Framebuffer;
	class VertexBuffer;
	class VertexBufferLayout;
	class Shader;

	enum class PrimitiveTopology
	{
		None = 0,
		Points,
		Lines,
		Triangles,
		LineStrip,
		TriangleStrip,
		TriangleFan
	};

	enum class DepthCompareOperator
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

	struct PipelineSpecification
	{
		Ref<Shader> Shader;
		Ref<Framebuffer> TargetFramebuffer;
		VertexBufferLayout Layout;
		VertexBufferLayout InstanceLayout;
		VertexBufferLayout BoneInfluenceLayout;

		PrimitiveTopology Topology = PrimitiveTopology::Triangles;
		DepthCompareOperator DepthOperator = DepthCompareOperator::GreaterOrEqual;

		bool BackfaceCulling = true;
		bool DepthTest = true;
		bool DepthWrite = true;
		bool Wireframe = false;
		float LineWidth = 1.0f;

		std::string DebugName;

		RendererAPIType RenderAPI = RendererAPIType::None;

		void SetLayout(const VertexBufferLayout& layout) { Layout = layout; }
	};


}