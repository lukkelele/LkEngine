#pragma once



namespace LkEngine {

	enum class RendererAPIType
	{
		None,
		OpenGL,
		Vulkan
	};

    class RendererAPI
    {
		virtual void Init() = 0;
		virtual void Shutdown() = 0;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		static RendererAPIType Current() { return m_CurrentRendererAPI; }
		static void SetAPI(RendererAPIType api);

	private:
		inline static RendererAPIType m_CurrentRendererAPI = RendererAPIType::OpenGL;
    };

}
