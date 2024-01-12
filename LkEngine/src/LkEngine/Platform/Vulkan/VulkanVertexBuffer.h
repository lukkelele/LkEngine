#pragma once

#include "LkEngine/Renderer/VertexBuffer.h"


namespace LkEngine {

    class VulkanVertexBuffer : public VertexBuffer
    {
	public:
		VulkanVertexBuffer(const void* data, unsigned int size);
		VulkanVertexBuffer(unsigned int size);
		~VulkanVertexBuffer();

		void Bind() const;
		void Unbind() const;
		void SetData(const void* data, unsigned int size);
		void SetIndexBuffer(const s_ptr<IndexBuffer> ib);
		void SetLayout(const VertexBufferLayout& layout); // { m_BufferLayout = layout; }

	private:
		void BindVertexArray();
		void UnbindVertexArray();
		void AddVertexBufferToVertexArray();
		void AddVertexBuffer(VertexBuffer& vb);

	private:
		unsigned int m_VertexBufferIndex = 0;
		unsigned int m_VertexArrayID = 0;
    };

}
