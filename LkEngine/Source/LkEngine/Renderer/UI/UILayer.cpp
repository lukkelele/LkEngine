#include "LKpch.h"
#include "UILayer.h"

#include "LkEngine/Renderer/RendererAPI.h"
#include "LkEngine/Renderer/Backend/OpenGL/OpenGLImGuiLayer.h"


namespace LkEngine {

	TObjectPtr<LUILayer> LUILayer::Create()
	{
		switch (LRendererAPI::Get())
		{
			case ERendererAPI::OpenGL: return TObjectPtr<LOpenGLImGuiLayer>::Create();
			case ERendererAPI::None:   return nullptr;
		}

		LK_CORE_ASSERT(false, "");
		return nullptr;
	}

}
