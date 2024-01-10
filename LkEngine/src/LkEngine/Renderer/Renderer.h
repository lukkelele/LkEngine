#pragma once

#include "LkEngine/Core/Base.h"

#include "RendererAPI.h"
#include "Renderer2D.h"
#include "RenderCommand.h"
#include "RenderCommandQueue.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "UniformBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "MaterialLibrary.h"

#include "LkEngine/Scene/Components.h"

// FIXME
#include <glad/glad.h>
#define LK_DRAWMODE_TRIANGLES  GL_TRIANGLES
#define LK_DRAWMODE_LINES	   GL_LINES

namespace LkEngine {

	// Forward declaration
	class Entity;

	class Renderer
	{
	public:
		Renderer();
		~Renderer() = default;

		static Renderer* Get() { return m_Instance; }

		static void Init();
		static void Shutdown();
		static void Clear();
		static void BeginFrame();
		static void EndFrame();
		static void SwapQueues();

		static void SetDrawMode(const RendererDrawMode& drawMode);
		static RendererDrawMode& GetDrawMode() { return DrawMode; }
		static std::string GetDrawModeStr();
		static RenderCommandQueue& GetRenderCommandQueue();
		static uint32_t GetRenderQueueIndex();
		static uint32_t GetRenderQueueSubmissionIndex();
		static s_ptr<RendererAPI> GetRendererAPI() { return m_RendererAPI; }
		static s_ptr<ShaderLibrary> GetShaderLibrary();

		static void SubmitLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color, uint32_t entityID = 0);
		static void SubmitLines(const VertexBuffer& va, const IndexBuffer& ib, const Shader& shader);
        static void SubmitQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0);
        static void SubmitQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0);
        static void SubmitQuad(const glm::vec2& pos, const glm::vec2& size, s_ptr<Texture> texture, uint64_t entityID = 0);
        static void SubmitQuad(const glm::vec3& pos, const glm::vec2& size, s_ptr<Texture> texture, uint64_t entityID = 0);
		static void SubmitSprite(TransformComponent& tc, const glm::vec2& size, const glm::vec4 color, uint64_t entityID = 0);
		static void SubmitSprite(TransformComponent& tc, const glm::vec2& size, s_ptr<Texture> texture, uint64_t entityID = 0);
		static void SubmitSprite(TransformComponent& tc, const glm::vec2& size, s_ptr<Texture> texture, const glm::vec4& color, uint64_t entityID = 0);
		static void SubmitIndexed(VertexBuffer& vb, unsigned int count);

		static void BeginScene(const Camera& camera);
		static void BeginScene(const glm::mat4& viewProjection);

		template<typename FuncT>
		static void Submit(FuncT&& func)
		{
			auto renderCmd = [](void* ptr) 
			{
				auto pFunc = (FuncT*)ptr;
				(*pFunc)();
				pFunc->~FuncT();
			};

			auto storageBuffer = GetRenderCommandQueue().Allocate(renderCmd, sizeof(func));
			new (storageBuffer) FuncT(std::forward<FuncT>(func));
		}

		template<typename FuncT>
		static void SubmitResourceFree(FuncT&& func)
		{
			auto renderCmd = [](void* ptr) 
			{
				auto pFunc = (FuncT*)ptr;
				(*pFunc)();
				pFunc->~FuncT();
			};

			Submit([renderCmd, func]()
			{
				const uint32_t index = Renderer::RT_GetCurrentFrameIndex();
				auto storageBuffer = GetRenderResourceReleaseQueue(index).Allocate(renderCmd, sizeof(func));
				new (storageBuffer) FuncT(std::forward<FuncT>((FuncT&&)func));
			});
		}

	public:
		inline static RendererDrawMode DrawMode;
		inline static glm::vec4 BackgroundColor = { 0.216f, 0.240f, 0.350f, 1.0f };
	private:
		inline static s_ptr<RendererAPI> m_RendererAPI = nullptr;

		inline static Renderer* m_Instance = nullptr;

		friend class Editor;
	};

}
