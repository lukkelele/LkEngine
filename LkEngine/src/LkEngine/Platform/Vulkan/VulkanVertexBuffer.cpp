#include "LKpch.h"
#include "VulkanVertexBuffer.h"



namespace LkEngine {

	VulkanVertexBuffer::VulkanVertexBuffer(unsigned int bufsize)
	{
	}

	VulkanVertexBuffer::VulkanVertexBuffer(const void* data, unsigned int arrsize)
	{
		//LOG_TRACE("New VulkanVertexBuffer, id: {}", m_RendererID);
	}
	 
	VulkanVertexBuffer::~VulkanVertexBuffer() 
	{
	}

	void VulkanVertexBuffer::Bind() const
	{
	}

	void VulkanVertexBuffer::Unbind() const
	{
	}

	void VulkanVertexBuffer::SetData(const void* data, unsigned int size)
	{
	}

	void VulkanVertexBuffer::SetLayout(const VertexBufferLayout& layout)
	{
	}

	void VulkanVertexBuffer::AddVertexBufferToVertexArray()
	{
	}

	void VulkanVertexBuffer::AddVertexBuffer(VertexBuffer& vb)
	{
	}

	void VulkanVertexBuffer::BindVertexArray()
	{
	}

	void VulkanVertexBuffer::UnbindVertexArray()
	{
	}

	void VulkanVertexBuffer::SetIndexBuffer(const s_ptr<IndexBuffer> indexBuffer)
	{
		BindVertexArray();
		Bind();
		indexBuffer->Bind();
		m_IndexBuffer = indexBuffer;
	}

}