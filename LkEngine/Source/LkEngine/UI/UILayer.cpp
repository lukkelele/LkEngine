#include "LKpch.h"
#include "UILayer.h"

#include "LkEngine/Renderer/RendererAPI.h"

#include "LkEngine/Renderer/Backend/OpenGL/OpenGLImGuiLayer.h"

#include "ImGui/ImGuiUtilities.h"


namespace LkEngine {

	TObjectPtr<LUILayer> LUILayer::Create()
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: return TObjectPtr<LOpenGLImGuiLayer>::Create();
			case ERendererAPI::None:   return nullptr;
		}

		LK_CORE_ASSERT(false, "");
		return nullptr;
	}

}