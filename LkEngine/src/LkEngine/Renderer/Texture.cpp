#include "LKpch.h"
#include "LkEngine/Renderer/Texture.h"
#include "stb_image/stb_image.h"


namespace LkEngine {

	Texture::Texture(const std::string& path)
		: m_RendererID(0)
		, m_FilePath(path)
		, m_LocalBuffer(nullptr)
		, m_Width(0)
		, m_Height(0)
		, m_BPP(0)
	{
		stbi_set_flip_vertically_on_load(1);
		m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);

		GL_CALL(glGenTextures(1, &m_RendererID));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, m_RendererID));

		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)); // S: x
		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)); // T: y

		GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

		if (m_LocalBuffer)
			stbi_image_free(m_LocalBuffer);
	}

	Texture::~Texture()
	{
		GL_CALL(glDeleteTextures(1, &m_RendererID));
	}

	void Texture::Bind(unsigned int slot /*= 0*/) const
	{
		GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, m_RendererID));
	}

	void Texture::Unbind() const
	{
		GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
	}
}
