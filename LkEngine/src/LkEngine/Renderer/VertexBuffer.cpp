#include "LKpch.h"
#include "LkEngine/Renderer/VertexBuffer.h"
#include <glad/glad.h>


namespace LkEngine {

	VertexBuffer::VertexBuffer(const void* data, unsigned int size)
	{
		GL_CALL(glGenBuffers(1, &m_Count));
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_Count));
		GL_CALL(glBufferData(GL_ARRAY_BUFFER,  size, data, GL_STATIC_DRAW));
	}
	 
	VertexBuffer::~VertexBuffer() 
	{
		GL_CALL(glDeleteBuffers(1, &m_Count));
	}

	void VertexBuffer::Bind() const
	{
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_Count));

	}

	void VertexBuffer::Unbind() const
	{
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));

	}

}
