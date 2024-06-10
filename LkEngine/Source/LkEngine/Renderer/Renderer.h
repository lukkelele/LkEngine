#pragma once

#include "RendererAPI.h"
#include "Renderer2DAPI.h"

#include "LkEngine/Scene/Components.h"


namespace LkEngine {

	class Renderer : public RefCounted
	{
	public:
		Renderer();
		~Renderer() = default;

		static void Init();
		static void Shutdown();
		static void Clear();
		static void BeginFrame();
		static void EndFrame();
		static void SwapQueues();

		static void BeginRenderPass(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<RenderPass> renderPass);
		static void EndRenderPass(Ref<RenderCommandBuffer> renderCommandBuffer);
		static void RenderGeometry(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline, Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer, const glm::mat4& transform, uint32_t indexCount = 0);
		static void RenderGeometry(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline, Ref<Shader> shader, Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer, const glm::mat4& transform, uint32_t indexCount = 0);
		static void RenderGeometry(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline, Ref<Material> material, Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer, const glm::mat4& transform, uint32_t indexCount = 0);

		static void SubmitImage(const Ref<Image> image);
		static void SubmitImage(const Ref<Image2D> image);

		static void SubmitMesh(Ref<Mesh>& mesh, Ref<Shader>& shader, const glm::mat4& transform);

		static void SubmitLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color, uint32_t entityID = 0);
		static void SubmitLines(const VertexBuffer& va, const IndexBuffer& ib, const Shader& shader);
        static void SubmitQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0);
        static void SubmitQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0);
        static void SubmitQuad(const glm::vec2& pos, const glm::vec2& size, Ref<Texture> texture, uint64_t entityID = 0);
        static void SubmitQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture> texture, uint64_t entityID = 0);
		static void SubmitSprite(TransformComponent& tc, const glm::vec2& size, const glm::vec4 color, uint64_t entityID = 0);
		static void SubmitSprite(TransformComponent& tc, const glm::vec2& size, Ref<Texture> texture, uint64_t entityID = 0);
		static void SubmitSprite(TransformComponent& tc, const glm::vec2& size, Ref<Texture> texture, const glm::vec4& color, uint64_t entityID = 0);
		static void SubmitIndexed(VertexBuffer& vb, unsigned int count);

		static void DrawMesh(Ref<Mesh>& mesh, const Ref<Shader> shader);

		static uint32_t GetCurrentFrameIndex();
		static uint32_t RT_GetCurrentFrameIndex();

		static Ref<Framebuffer>& GetViewportFramebuffer();
		static RendererCapabilities& GetCapabilities();

		static void BeginScene(const SceneCamera& camera);
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
				auto storageBuffer = GetRenderCommandQueue().Allocate(renderCmd, sizeof(func));
				new (storageBuffer) FuncT(std::forward<FuncT>((FuncT&&)func));
			});
		}

		static uint32_t GetSamplersCount() { return m_SamplerCount; }
		static void IncrementSamplerCount() { m_SamplerCount++; }
		static void DecreaseSamplerCount() { m_SamplerCount--; }

		static uint32_t GetFramesInFlight() { return m_FramesInFlight; }

		static void SetPrimitiveTopology(const RenderTopology& topology);
		static RenderTopology& GetPrimitiveTopology() { return PrimitiveTopology; }
		static RenderCommandQueue& GetRenderCommandQueue();
		static uint32_t GetRenderQueueIndex();
		static uint32_t GetRenderQueueSubmissionIndex();

		static Ref<ShaderLibrary> GetShaderLibrary();
		static Ref<TextureLibrary> GetTextureLibrary();
		static Ref<MaterialLibrary> GetMaterialLibrary();
		static Ref<RenderContext> GetContext();

		static Renderer* Get() { return s_Instance; }
		static Ref<RendererAPI> GetRendererAPI() { return s_RendererAPI; }
		static Ref<Renderer2DAPI> GetRenderer2D() { return s_Renderer2DAPI; }
		static Ref<Renderer2DAPI> GetRenderer2DAPI() { return s_Renderer2DAPI; }

		static Ref<Texture2D> GetWhiteTexture();
		static Ref<TextureCube> GetWhiteTextureCube();

		static void RegisterShaderDependency(Ref<Shader> shader, Ref<Material> material);

		static void SetDepthFunction(const DepthFunction& depthFunc);

	private:
		static void LoadTextures();

	public:
		inline static RenderTopology PrimitiveTopology;
		inline static glm::vec4 ClearColor = { 0.216f, 0.240f, 0.250f, 1.0f };
	private:
		inline static uint32_t m_SamplerCount = 0; // Samplers
		inline static uint32_t m_FramesInFlight = 2;

		inline static Ref<RendererAPI> s_RendererAPI = nullptr;
		inline static Ref<Renderer2DAPI> s_Renderer2DAPI = nullptr;

		inline static Renderer* s_Instance = nullptr;

		friend class EditorLayer;
	};

}
