#include "LKpch.h"
#include "OpenGLIndexBuffer.h"


namespace LkEngine {

    OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t size)
		: m_Size(size)
    {
		LK_OpenGL(glCreateBuffers(1, &m_RendererID));
		LK_OpenGL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
		LK_OpenGL(glNamedBufferData(m_RendererID, m_Size, nullptr, GL_DYNAMIC_DRAW));

		//LK_OpenGL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0)); // UNBIND
    }

	OpenGLIndexBuffer::OpenGLIndexBuffer(void* buffer, uint32_t size)
		: m_Size(size)
		, m_Count(size / sizeof(unsigned int))
	{
		LK_OpenGL(glCreateBuffers(1, &m_RendererID));
		LK_OpenGL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
		LK_OpenGL(glNamedBufferData(m_RendererID, m_Size, nullptr, GL_DYNAMIC_DRAW));
		//LOG_DEBUG("New IndexBuffer, id: {}, size: {}, count: {}", m_RendererID, size, m_Count);
		
		SetData(buffer, size, 0);
	}
	 
	OpenGLIndexBuffer::~OpenGLIndexBuffer() 
	{
		LK_OpenGL(glDeleteBuffers(1, &m_RendererID));
	}

	void OpenGLIndexBuffer::Bind() const
	{
		LK_OpenGL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
	}

	void OpenGLIndexBuffer::SetData(void* buffer, uint64_t size, uint64_t offset)
	{
		m_LocalData = FBuffer::Copy(buffer, size);
		m_Size = size;
		glNamedBufferSubData(m_RendererID, offset, m_Size, m_LocalData.Data);
	}

}