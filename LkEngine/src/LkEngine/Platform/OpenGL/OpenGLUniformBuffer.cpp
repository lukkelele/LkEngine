#include "LKpch.h"
#include "OpenGLUniformBuffer.h"

#include "LkOpenGL.h"


namespace LkEngine {

	OpenGLUniformBuffer::OpenGLUniformBuffer(uint64_t size)
	{
		glCreateBuffers(1, &m_ID);

		glNamedBufferData(m_ID, size, nullptr, GL_DYNAMIC_DRAW); 
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_ID);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		glDeleteBuffers(1, &m_ID);
	}

	void OpenGLUniformBuffer::SetData(const void* data, uint64_t size, uint64_t offset)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_ID);
		glNamedBufferSubData(m_ID, offset, size, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void OpenGLUniformBuffer::RT_SetData(const void* data, uint64_t size, uint64_t offset)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_ID);
		glNamedBufferSubData(m_ID, offset, size, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void OpenGLUniformBuffer::Bind()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_ID);
	}

	void OpenGLUniformBuffer::Unbind()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

}