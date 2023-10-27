#include "LKpch.h"
#include "LkEngine/Renderer/VertexBuffer.h"
#include <glad/glad.h>


namespace LkEngine {

	VertexBuffer::VertexBuffer(unsigned int bufsize)
	{
		//GL_CALL(glCreateBuffers(1, &m_RendererID));
		GL_CALL(glGenBuffers(1, &m_RendererID));
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
		GL_CALL(glBufferData(GL_ARRAY_BUFFER, bufsize, nullptr, GL_STATIC_DRAW));
		LOG_TRACE("New VertexBuffer, id: {}", m_RendererID);
	}

	VertexBuffer::VertexBuffer(const void* data, unsigned int arrsize)
	{
		GL_CALL(glGenBuffers(1, &m_RendererID));
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
		GL_CALL(glBufferData(GL_ARRAY_BUFFER, arrsize * sizeof(float), data, GL_STATIC_DRAW));
		LOG_TRACE("New VertexBuffer, id: {}", m_RendererID);
	}
	 
	VertexBuffer::~VertexBuffer() 
	{
		GL_CALL(glDeleteBuffers(1, &m_RendererID));
	}

	void VertexBuffer::Bind() const
	{
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
	}

	void VertexBuffer::Unbind() const
	{
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	void VertexBuffer::SetData(const void* data, unsigned int size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

}
