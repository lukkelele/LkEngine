#include "LKpch.h"
#include "LkEngine/Renderer/UniformBuffer.h"
#include <glad/glad.h>


namespace LkEngine {

	UniformBuffer::UniformBuffer(uint32_t size, uint32_t binding)
	{
		glCreateBuffers(1, &m_RendererID);

		glNamedBufferData(m_RendererID, size, nullptr, GL_DYNAMIC_DRAW); 
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);
	}

	UniformBuffer::~UniformBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void UniformBuffer::Bind()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
	}

	void UniformBuffer::Unbind()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void UniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
		glNamedBufferSubData(m_RendererID, offset, size, data);
		// glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

}