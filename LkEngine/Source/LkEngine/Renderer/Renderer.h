#pragma once

#include "RendererAPI.h"

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"
#include "LkEngine/Core/Delegate/Delegate.h"

#include "LkEngine/Scene/Components.h"

namespace LkEngine {

	class LShader;

	/** @fixme: FIXME */
	//class LRenderer : public LObject
	class LRenderer
	{
	public:
		LRenderer();
		~LRenderer() = default;

		static void Initialize();
		static void Shutdown();

		static void Clear();
		static void BeginFrame();
		static void EndFrame();
		static void SwapQueues();

		static void RenderMesh(TObjectPtr<LMesh>& Mesh, TObjectPtr<LShader>& Shader, const glm::mat4& Transform);
		static void RenderQuad(const glm::vec2& Pos, const glm::vec2& Size, const glm::vec4& Color, uint64_t EntityID = 0);
		static void RenderGeometry(TObjectPtr<LRenderCommandBuffer> RenderCommandBuffer, 
								   TObjectPtr<LPipeline> Pipeline,
								   TObjectPtr<LMaterial> Material, 
								   TObjectPtr<LVertexBuffer> VertexBuffer,
								   TObjectPtr<LIndexBuffer> IndexBuffer, 
								   const glm::mat4& Transform,
								   const uint32_t IndexCount = 0);

		static uint32_t GetCurrentFrameIndex();

		static TObjectPtr<LFramebuffer> GetViewportFramebuffer();
		static FRendererCapabilities& GetCapabilities();

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

		static void BeginRenderPass(TObjectPtr<LRenderCommandBuffer> RenderCommandBuffer, TObjectPtr<LRenderPass> RenderPass);
		static void EndRenderPass(TObjectPtr<LRenderCommandBuffer> RenderCommandBuffer);

		static uint32_t GetSamplersCount() { return SamplerCount; }
		static void IncrementSamplerCount() { SamplerCount++; }
		static void DecreaseSamplerCount() { SamplerCount--; }

		FORCEINLINE static uint32_t GetFramesInFlight() { return FramesInFlight; }

		static void SetPrimitiveTopology(const ERenderTopology InRenderTopology);
		FORCEINLINE static ERenderTopology GetPrimitiveTopology() { return PrimitiveTopology; }

		static uint32_t GetRenderQueueIndex();
		static uint32_t GetRenderQueueSubmissionIndex();

		static TObjectPtr<LShaderLibrary> GetShaderLibrary();
		static TObjectPtr<LTexture2D> GetWhiteTexture();
		static TObjectPtr<LTextureCube> GetWhiteTextureCube();

		static void RegisterShader(TObjectPtr<LShader> InShader, TObjectPtr<LMaterial> InMaterial);
		static void SetDepthFunction(const EDepthFunction InDepthFunction);

		static const char* GetCurrentRenderApiName();

	private:
		static LRenderCommandQueue& GetRenderCommandQueue();

	public:
		inline static ERenderTopology PrimitiveTopology = ERenderTopology::Triangles;

		inline static glm::vec4 ClearColor = { 0.216f, 0.240f, 0.250f, 1.0f };
		inline static glm::vec4 DEFAULT_CLEARCOLOR = { 0.250f, 0.250f, 0.250f, 1.0f }; /* FIXME */
	private:
		inline static uint32_t SamplerCount = 0;
		inline static uint32_t FramesInFlight = 2;

		inline static TObjectPtr<LRendererAPI> RendererAPI{};

		friend class LEditorLayer;

		/** @fixme: FIXME */
		//LCLASS(LRenderer)
	};

}
