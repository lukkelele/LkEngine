#pragma once

#include "LkEngine/Renderer/RenderContext.h"


namespace LkEngine {

	class LWindow;
	class OpenGLSwapChain;

	class LOpenGLContext : public LRenderContext
	{
	public:
		LOpenGLContext(LWindow* InWindow);
		~LOpenGLContext() = default;

		virtual void Initialize(const ESourceBlendFunction InSourceBlendFunction,
								const EDestinationBlendFunction InDestinationBlendFunction) override;
		virtual void Destroy() override;

		FORCEINLINE virtual GLFWwindow* GetGlfwWindow() override
		{
			LK_CORE_ASSERT(m_GlfwWindow, "GlfwWindow is nullptr");
			return m_GlfwWindow;
		}

		virtual void SetViewport(const glm::vec2& pos, const glm::vec2& size) override;
		virtual void UpdateResolution(const uint16_t width, const uint16_t height) override;

		virtual bool GetDepthEnabled() const override { return bDepthEnabled; }
		virtual void SetDepthEnabled(const bool InEnabled) override;
		virtual void SetDepthFunction(const EDepthFunction depthFunc) override;

		virtual bool GetBlendingEnabled() const override { return bBlendingEnabled; }
		virtual void SetBlendingEnabled(const bool InEnabled) override;
		virtual void SetBlendFunction(const ESourceBlendFunction InSourceBlendFunction,
									  const EDestinationBlendFunction InDestinationBlendFunction) override;

		virtual void SetSourceBlendFunction(const ESourceBlendFunction InSourceBlendFunction) override;
		virtual void SetDestinationBlendFunction(const EDestinationBlendFunction InDestinationBlendFunction) override;

		FORCEINLINE virtual ESourceBlendFunction GetSourceBlendFunction() const override
		{
			return BlendFunction.Source;
		}

		FORCEINLINE virtual EDestinationBlendFunction GetDestinationBlendFunction() const override
		{
			return BlendFunction.Destination;
		}

	private:
		std::string m_GlslVersion{};

		bool bDepthEnabled = true;
		bool bBlendingEnabled = true;

		FBlendFunction BlendFunction{};
		EDepthFunction DepthFunction{};

		LWindow* Window{};
		GLFWwindow* m_GlfwWindow{};

		TObjectPtr<OpenGLSwapChain> SwapChain;

		friend class Editor;

		LCLASS(LOpenGLContext);
	};

}
