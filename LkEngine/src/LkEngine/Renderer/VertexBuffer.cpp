#include "LKpch.h"
#include "LkEngine/Renderer/VertexBuffer.h"
#include <glad/glad.h>


namespace LkEngine {

	VertexBuffer::VertexBuffer(const void* data, unsigned int arrsize)
	{
		GL_CALL(glGenBuffers(1, &m_RendererID));
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
		LK_ASSERT(sizeof(GLfloat) == sizeof(float));
		GL_CALL(glBufferData(GL_ARRAY_BUFFER, arrsize * sizeof(float), data, GL_STATIC_DRAW));
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

}
