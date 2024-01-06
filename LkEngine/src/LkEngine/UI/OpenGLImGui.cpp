#include "LKpch.h"
#include "OpenGLImGui.h"

#include "LkEngine/Renderer/Renderer.h"


namespace LkEngine::UI {

	s_ptr<Texture> Texture_1 = nullptr;
	s_ptr<Texture> Texture_2 = nullptr;

	void InitOpenGLImGui()
	{
		auto& whiteTexture = TextureLibrary::Get()->GetWhiteTexture();
		while (whiteTexture == nullptr)
			whiteTexture = TextureLibrary::Get()->GetWhiteTexture();

		Texture_1 = Texture::Create(whiteTexture->GetSpecification()); // Copy
		Texture_2 = Texture::Create(whiteTexture->GetSpecification()); // Copy
		LOG_DEBUG("Initialized OpenGLImGui");
	}

	void Image(const s_ptr<Texture2D>& texture, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
	{
		ImGui::Image((void*)(intptr_t)texture->GetRendererID(), size, uv0, uv1, tint_col, border_col);
	}

}