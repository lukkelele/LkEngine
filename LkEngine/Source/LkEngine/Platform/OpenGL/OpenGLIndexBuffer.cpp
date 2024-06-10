#include "LKpch.h"
#include "OpenGLIndexBuffer.h"


namespace LkEngine {

    OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t size)
		: m_Size(size)
    {
		GL_CALL(glCreateBuffers(1, &m_RendererID));
		GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
		GL_CALL(glNamedBufferData(m_RendererID, m_Size, nullptr, GL_DYNAMIC_DRAW));

		//GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0)); // UNBIND
    }

	OpenGLIndexBuffer::OpenGLIndexBuffer(void* buffer, uint32_t size)
		: m_Size(size)
		, m_Count(size / sizeof(unsigned int))
	{
		GL_CALL(glCreateBuffers(1, &m_RendererID));
		GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
		GL_CALL(glNamedBufferData(m_RendererID, m_Size, nullptr, GL_DYNAMIC_DRAW));
		//LOG_DEBUG("New IndexBuffer, id: {}, size: {}, count: {}", m_RendererID, size, m_Count);
		
		SetData(buffer, size, 0);
	}
	 
	OpenGLIndexBuffer::~OpenGLIndexBuffer() 
	{
		GL_CALL(glDeleteBuffers(1, &m_RendererID));
	}

	void OpenGLIndexBuffer::Bind() const
	{
		GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
	}

	void OpenGLIndexBuffer::SetData(void* buffer, uint64_t size, uint64_t offset)
	{
		m_LocalData = Buffer::Copy(buffer, size);
		m_Size = size;
		glNamedBufferSubData(m_RendererID, offset, m_Size, m_LocalData.Data);
	}

}