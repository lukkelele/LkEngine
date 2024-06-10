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

	/// @TODO: Prefix E for enumeration
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

	/// @TODO: Prefix E for enumeration
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

	struct FPipelineSpecification
	{
		Ref<Shader> Shader{ nullptr };
		Ref<Framebuffer> TargetFramebuffer{ nullptr };

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

		ERendererAPI RenderAPI = ERendererAPI::None;

		void SetLayout(const VertexBufferLayout& layout) { Layout = layout; }
	};


}