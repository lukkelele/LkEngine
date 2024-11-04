#pragma once

#include "RendererAPI.h"
#include "Renderer2DAPI.h"

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "LkEngine/Scene/Components.h"


namespace LkEngine {

	class LRenderer : public LObject
	{
	public:
		LRenderer();
		~LRenderer() = default;

		/** Initialize the renderer by creating the pipeline components. */
		static void Initialize();
		static void Shutdown();

		static void Clear();
		static void BeginFrame();
		static void EndFrame();
		static void SwapQueues();

		static void BeginRenderPass(TObjectPtr<LRenderCommandBuffer> renderCommandBuffer, 
									TObjectPtr<LRenderPass> renderPass);

		static void EndRenderPass(TObjectPtr<LRenderCommandBuffer> renderCommandBuffer);

		static void RenderGeometry(TObjectPtr<LRenderCommandBuffer> renderCommandBuffer, 
								   TObjectPtr<LPipeline> pipeline, 
								   TObjectPtr<LVertexBuffer> VertexBuffer, 
								   TObjectPtr<LIndexBuffer> indexBuffer, 
								   const glm::mat4& transform, 
								   const uint32_t indexCount = 0);

		static void RenderGeometry(TObjectPtr<LRenderCommandBuffer> renderCommandBuffer,
								   TObjectPtr<LPipeline> pipeline, 
								   TObjectPtr<LShader> shader, 
								   TObjectPtr<LVertexBuffer> VertexBuffer, 
								   TObjectPtr<LIndexBuffer> indexBuffer, 
								   const glm::mat4& transform, 
								   const uint32_t indexCount = 0);

		static void RenderGeometry(TObjectPtr<LRenderCommandBuffer> renderCommandBuffer,
								   TObjectPtr<LPipeline> pipeline, 
								   TObjectPtr<LMaterial> material, 
								   TObjectPtr<LVertexBuffer> VertexBuffer, 
								   TObjectPtr<LIndexBuffer> indexBuffer, 
								   const glm::mat4& transform, 
								   const uint32_t indexCount = 0);

		static void SubmitImage(const TObjectPtr<LImage> image);
		static void SubmitImage(const TObjectPtr<LImage2D> image);

		static void SubmitMesh(TObjectPtr<LMesh>& mesh, 
							   TObjectPtr<LShader>& shader, 
							   const glm::mat4& transform);

		static void SubmitLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color, uint32_t entityID = 0);

		static void SubmitLines(const LVertexBuffer& va, 
								const LIndexBuffer& ib, 
								const LShader& shader);

		// SubmitQuad
        static void SubmitQuad(const glm::vec2& pos, 
							   const glm::vec2& size, 
							   const glm::vec4& color, 
							   uint64_t entityID = 0);

        static void SubmitQuad(const glm::vec3& pos, 
							   const glm::vec2& size, 
							   const glm::vec4& color, 
							   uint64_t entityID = 0);

        static void SubmitQuad(const glm::vec2& pos, 
							   const glm::vec2& size, 
							   TObjectPtr<LTexture> texture, 
							   uint64_t entityID = 0);

        static void SubmitQuad(const glm::vec3& pos, 
							   const glm::vec2& size, 
							   TObjectPtr<LTexture> texture, 
							   uint64_t entityID = 0);
		// ~SubmitQuad

		static void SubmitSprite(LTransformComponent& tc, 
								 const glm::vec2& size, 
								 const glm::vec4 color, 
								 uint64_t entityID = 0);

		static void SubmitSprite(LTransformComponent& tc, 
								 const glm::vec2& size, 
								 TObjectPtr<LTexture> texture, 
								 uint64_t entityID = 0);

		static void SubmitSprite(LTransformComponent& tc, 
								 const glm::vec2& size, 
								 TObjectPtr<LTexture> texture, 
								 const glm::vec4& color, 
								 uint64_t entityID = 0);

		static void SubmitIndexed(LVertexBuffer& vb, const uint32_t count);

		static void DrawMesh(TObjectPtr<LMesh>& Mesh, const TObjectPtr<LShader> Shader);

		static uint32_t GetCurrentFrameIndex();
		static uint32_t RT_GetCurrentFrameIndex();

		static TObjectPtr<LFramebuffer> GetViewportFramebuffer();
		static RendererCapabilities& GetCapabilities();

		static void BeginScene(const LSceneCamera& camera);
		static void BeginScene(const glm::mat4& ViewProjection);
		static void EndScene();

		template<typename TFunction>
		static void Submit(TFunction&& Function)
		{
			auto RenderCommand = [](void* Ptr) 
			{
				auto FunctionPtr = (TFunction*)Ptr;
				(*FunctionPtr)();
				FunctionPtr->~TFunction();
			};

			auto StorageBuffer = GetRenderCommandQueue().Allocate(RenderCommand, sizeof(Function));
			new (StorageBuffer) TFunction(std::forward<TFunction>(Function));
		}

		template<typename TFunction>
		static void SubmitResourceFree(TFunction&& Function)
		{
			auto RenderCmd = [](void* Ptr) 
			{
				auto FunctionPtr = (TFunction*)Ptr;
				(*FunctionPtr)();
				FunctionPtr->~TFunction();
			};

			Submit([RenderCmd, Function]()
			{
				const uint32_t index = LRenderer::RT_GetCurrentFrameIndex();
				auto StorageBuffer = GetRenderCommandQueue().Allocate(RenderCmd, sizeof(Function));
				new (StorageBuffer) TFunction(std::forward<TFunction>((TFunction&&)Function));
			});
		}

		static uint32_t GetSamplersCount() { return m_SamplerCount; }
		static void IncrementSamplerCount() { m_SamplerCount++; }
		static void DecreaseSamplerCount() { m_SamplerCount--; }

		static uint32_t GetFramesInFlight() { return m_FramesInFlight; }

		static void SetPrimitiveTopology(const ERenderTopology& InRenderTopology);
		FORCEINLINE static ERenderTopology GetPrimitiveTopology() { return PrimitiveTopology; }

		static RenderCommandQueue& GetRenderCommandQueue();
		static uint32_t GetRenderQueueIndex();
		static uint32_t GetRenderQueueSubmissionIndex();

		static TObjectPtr<LShaderLibrary> GetShaderLibrary();

		static LRenderer* Get() { return s_Instance; }

		/// @FIXME: remove all these Getters
		static TObjectPtr<LRendererAPI> GetRendererAPI() { return RendererAPI; }

		static TObjectPtr<LTexture2D> GetWhiteTexture();
		static TObjectPtr<LTextureCube> GetWhiteTextureCube();

		static void RegisterShaderDependency(TObjectPtr<LShader> InShader, TObjectPtr<LMaterial> InMaterial);

		static void SetDepthFunction(const EDepthFunction& InDepthFunction);

	private:
		static void LoadTextures();

	public:
		inline static ERenderTopology PrimitiveTopology;
		inline static glm::vec4 ClearColor = { 0.216f, 0.240f, 0.250f, 1.0f };

		inline static glm::vec4 DEFAULT_CLEARCOLOR = { 0.200f, 0.200f, 0.250f, 1.0f };
	private:
		inline static uint32_t m_SamplerCount = 0; // Samplers
		inline static uint32_t m_FramesInFlight = 2;

		/* Render API */
		inline static TObjectPtr<LRendererAPI> RendererAPI = nullptr;

		inline static LRenderer* s_Instance = nullptr;

		friend class LEditorLayer;

		LCLASS(LRenderer)
	};

}
