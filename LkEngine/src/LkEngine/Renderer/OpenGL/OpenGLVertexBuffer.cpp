#include "LKpch.h"
#include "LkEngine/Renderer/OpenGL/OpenGLVertexBuffer.h"


namespace LkEngine {

	OpenGLVertexBuffer::OpenGLVertexBuffer(unsigned int bufsize)
	{
		//GL_CALL(glCreateBuffers(1, &m_RendererID));
		GL_CALL(glGenBuffers(1, &m_RendererID));
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
		GL_CALL(glBufferData(GL_ARRAY_BUFFER, bufsize, nullptr, GL_STATIC_DRAW));
		LOG_TRACE("New OpenGLVertexBuffer, id: {}", m_RendererID);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(const void* data, unsigned int arrsize)
	{
		GL_CALL(glGenBuffers(1, &m_RendererID));
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
		GL_CALL(glBufferData(GL_ARRAY_BUFFER, arrsize * sizeof(float), data, GL_STATIC_DRAW));
		LOG_TRACE("New OpenGLVertexBuffer, id: {}", m_RendererID);
	}
	 
	OpenGLVertexBuffer::~OpenGLVertexBuffer() 
	{
		GL_CALL(glDeleteBuffers(1, &m_RendererID));
	}

	void OpenGLVertexBuffer::Bind() const
	{
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	void OpenGLVertexBuffer::SetData(const void* data, unsigned int size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

}