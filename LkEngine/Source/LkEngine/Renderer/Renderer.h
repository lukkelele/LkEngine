#pragma once

#include "RendererAPI.h"

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"
#include "LkEngine/Core/Delegate/Delegate.h"

#include "LkEngine/Scene/Components.h"


namespace LkEngine {

	class LShader;

	class LRenderer : public LObject
	{
	public:
		LK_DECLARE_MULTICAST_DELEGATE(FOnMeshSubmission, 
			TObjectPtr<LMesh>, 
			TObjectPtr<LShader>,
			TObjectPtr<LMaterial>
		);

	public:
		LRenderer();
		~LRenderer() = default;

		/**
		 * @brief Create Pipeline components.
		 */
		static void Initialize();
		static void Destroy();

		static void Clear();
		static void BeginFrame();
		static void EndFrame();
		static void SwapQueues();

		static void RenderGeometry(TObjectPtr<LRenderCommandBuffer> RenderCommandBuffer, 
								   TObjectPtr<LPipeline> Pipeline,
								   TObjectPtr<LVertexBuffer> VertexBuffer, 
								   TObjectPtr<LIndexBuffer> IndexBuffer,
								   const glm::mat4& Transform, 
								   const uint32_t IndexCount = 0);

		static void RenderGeometry(TObjectPtr<LRenderCommandBuffer> RenderCommandBuffer, 
								   TObjectPtr<LPipeline> Pipeline,
								   TObjectPtr<LShader> Shader, 
								   TObjectPtr<LVertexBuffer> VertexBuffer,
								   TObjectPtr<LIndexBuffer> IndexBuffer, 
								   const glm::mat4& Transform,
								   const uint32_t IndexCount = 0);

		static void RenderGeometry(TObjectPtr<LRenderCommandBuffer> RenderCommandBuffer, 
								   TObjectPtr<LPipeline> Pipeline,
								   TObjectPtr<LMaterial> Material, 
								   TObjectPtr<LVertexBuffer> VertexBuffer,
								   TObjectPtr<LIndexBuffer> IndexBuffer, 
								   const glm::mat4& Transform,
								   const uint32_t IndexCount = 0);

		static void SubmitImage(const TObjectPtr<LImage> Image);
		static void SubmitImage(const TObjectPtr<LImage2D> Image);

		static void SubmitMesh(TObjectPtr<LMesh>& Mesh, TObjectPtr<LShader>& Shader, const glm::mat4& Transform);

		static void SubmitLine(const glm::vec2& P0, const glm::vec2& P1, const glm::vec4& Color, uint32_t EntityID = 0);

		static void SubmitLines(const LVertexBuffer& va, const LIndexBuffer& ib, const LShader& Shader);

		// Quad
		static void SubmitQuad(const glm::vec2& Pos, const glm::vec2& Size, const glm::vec4& Color, uint64_t EntityID = 0);
		static void SubmitQuad(const glm::vec3& Pos, const glm::vec2& Size, const glm::vec4& Color, uint64_t EntityID = 0);
		static void SubmitQuad(const glm::vec2& Pos, const glm::vec2& Size, TObjectPtr<LTexture> Texture, uint64_t EntityID = 0);
		static void SubmitQuad(const glm::vec3& Pos, const glm::vec2& Size, TObjectPtr<LTexture> Texture, uint64_t EntityID = 0);
		// ~Quad

		static void SubmitSprite(LTransformComponent& TransformComponent, const glm::vec2& Size, const glm::vec4 Color, uint64_t EntityID = 0);
		static void SubmitSprite(LTransformComponent& TransformComponent, const glm::vec2& Size, TObjectPtr<LTexture> Texture, uint64_t EntityID = 0);
		static void SubmitSprite(LTransformComponent& TransformComponent, const glm::vec2& Size, TObjectPtr<LTexture> Texture, const glm::vec4& Color, uint64_t EntityID = 0);
		static void SubmitIndexed(LVertexBuffer& VertexBuffer, const uint32_t Count);

		static void DrawMesh(TObjectPtr<LMesh>& Mesh, const TObjectPtr<LShader> Shader);

		static uint32_t GetCurrentFrameIndex();
		static uint32_t RT_GetCurrentFrameIndex();

		static TObjectPtr<LFramebuffer> GetViewportFramebuffer();
		static RendererCapabilities& GetCapabilities();

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
				const uint32_t Index = LRenderer::RT_GetCurrentFrameIndex();
				auto StorageBuffer = GetRenderCommandQueue().Allocate(RenderCmd, sizeof(Function));
				new (StorageBuffer) TFunction(std::forward<TFunction>((TFunction&&)Function));
			});
		}

		static void BeginRenderPass(TObjectPtr<LRenderCommandBuffer> RenderCommandBuffer,
									TObjectPtr<LRenderPass> RenderPass);

		static void EndRenderPass(TObjectPtr<LRenderCommandBuffer> RenderCommandBuffer);


		FORCEINLINE static uint32_t GetSamplersCount() { return SamplerCount; }
		FORCEINLINE static void IncrementSamplerCount() { SamplerCount++; }
		FORCEINLINE static void DecreaseSamplerCount() { SamplerCount--; }

		FORCEINLINE static uint32_t GetFramesInFlight() { return FramesInFlight; }

		static void SetPrimitiveTopology(const ERenderTopology& InRenderTopology);

		FORCEINLINE static ERenderTopology GetPrimitiveTopology() { return PrimitiveTopology; }

		static LRenderCommandQueue& GetRenderCommandQueue();
		static uint32_t GetRenderQueueIndex();
		static uint32_t GetRenderQueueSubmissionIndex();

		static TObjectPtr<LShaderLibrary> GetShaderLibrary();

		static LRenderer* Get();

		static TObjectPtr<LTexture2D> GetWhiteTexture();
		static TObjectPtr<LTextureCube> GetWhiteTextureCube();

		static void RegisterShaderDependency(TObjectPtr<LShader> InShader, TObjectPtr<LMaterial> InMaterial);

		static void SetDepthFunction(const EDepthFunction InDepthFunction);

	private:
		static void LoadTextures();

	public:
		inline static glm::vec4 ClearColor = { 0.216f, 0.240f, 0.250f, 1.0f };
		inline static glm::vec4 DEFAULT_CLEARCOLOR = { 0.200f, 0.200f, 0.250f, 1.0f };

		static FOnMeshSubmission OnMeshSubmission;
		static ERenderTopology PrimitiveTopology;

	private:
		inline static uint32_t SamplerCount = 0;
		inline static uint32_t FramesInFlight = 2;

		/* Render API */
		inline static TObjectPtr<LRendererAPI> RendererAPI = nullptr;

		friend class LEditorLayer;

		LCLASS(LRenderer)
	};

}
