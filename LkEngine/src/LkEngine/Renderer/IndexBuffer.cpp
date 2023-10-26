#include "LKpch.h"
#include "LkEngine/Renderer/IndexBuffer.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Core/Base.h"


namespace LkEngine {

	IndexBuffer::IndexBuffer(uint32_t size)
		: m_Size(size)
	{
		//m_LocalData = Buffer(size);
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferData(m_RendererID, m_Size, nullptr, GL_DYNAMIC_DRAW);
	}


	//IndexBuffer::IndexBuffer(const void* data, unsigned int count)
	IndexBuffer::IndexBuffer(const void* data, uint32_t size)
		: m_Size(size), m_Count(size / sizeof(unsigned int))
	{
		//GL_CALL(glGenBuffers(1, &m_RendererID));
		//GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
		//GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER,  size, data, GL_STATIC_DRAW));
		//GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER,  count * sizeof(unsigned int), data, GL_STATIC_DRAW));
		GL_CALL(glCreateBuffers(1, &m_RendererID));
		GL_CALL(glNamedBufferData(m_RendererID, m_Size, nullptr, GL_DYNAMIC_DRAW));
		SetData(data, size, 0);
	}
	 
	IndexBuffer::~IndexBuffer() 
	{
		GL_CALL(glDeleteBuffers(1, &m_RendererID));
	}

	void IndexBuffer::Bind() const
	{
		GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
	}

	void IndexBuffer::Unbind() const
	{
		GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}

	void IndexBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		m_LocalData = Buffer::Copy(data, size);
		m_Size = size;
		glNamedBufferSubData(m_RendererID, offset, m_Size, m_LocalData.Data);
	}


}
