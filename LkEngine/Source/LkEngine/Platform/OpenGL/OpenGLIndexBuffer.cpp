#include "LKpch.h"
#include "OpenGLIndexBuffer.h"


namespace LkEngine {

    OpenGLIndexBuffer::OpenGLIndexBuffer(const uint32_t InSize)
		: m_Size(InSize)
		, m_Count(InSize / sizeof(decltype(InSize)))
    {
		LCLASS_REGISTER();

		LK_OpenGL(glCreateBuffers(1, &m_RendererID));
		LK_OpenGL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
		LK_OpenGL(glNamedBufferData(m_RendererID, m_Size, nullptr, GL_DYNAMIC_DRAW));
		//LK_OpenGL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0)); // UNBIND
    }

	OpenGLIndexBuffer::OpenGLIndexBuffer(void* InData, const uint32_t InSize)
		: m_Size(InSize)
		, m_Count(InSize / sizeof(decltype(InSize)))
	{
		LCLASS_REGISTER();

		LK_OpenGL(glCreateBuffers(1, &m_RendererID));
		LK_OpenGL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
		LK_OpenGL(glNamedBufferData(m_RendererID, m_Size, nullptr, GL_DYNAMIC_DRAW));
		
		SetData(InData, InSize, 0);
	}
	 
	OpenGLIndexBuffer::~OpenGLIndexBuffer() 
	{
		LK_OpenGL(glDeleteBuffers(1, &m_RendererID));
	}

	void OpenGLIndexBuffer::Bind() const
	{
		LK_OpenGL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
	}

	void OpenGLIndexBuffer::SetData(void* InData, const uint64_t InSize, const uint64_t InOffset)
	{
		if ((m_RendererID > 0) && InData)
		{
			m_LocalData = FBuffer::Copy(InData, InSize);
			m_Size = InSize;

			LK_OpenGL(glNamedBufferSubData(m_RendererID, InOffset, m_Size, m_LocalData.Data));
		}
	}

}