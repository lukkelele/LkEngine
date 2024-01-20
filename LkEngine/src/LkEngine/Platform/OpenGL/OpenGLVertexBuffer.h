#pragma once

#include "LkEngine/Renderer/VertexBuffer.h"


namespace LkEngine {

    class OpenGLVertexBuffer : public VertexBuffer
    {
	public:
		OpenGLVertexBuffer(void* data, uint64_t size, VertexBufferUsage usage = VertexBufferUsage::Dynamic);
		OpenGLVertexBuffer(uint64_t size, VertexBufferUsage usage = VertexBufferUsage::Dynamic);
		~OpenGLVertexBuffer();

		void SetData(void* data, uint64_t size, uint64_t offset = 0) override;
		void RT_SetData(void* buffer, uint64_t size, uint64_t offset = 0) override;

		Buffer GetLocalData() override { return m_LocalData; }
		uint64_t GetSize() const override { return m_Size; }

		void SetIndexBuffer(const Ref<IndexBuffer> ib) override;
		void SetLayout(const VertexBufferLayout& layout) override; 
		Ref<IndexBuffer> GetIndexBuffer() override { return m_IndexBuffer; }
		VertexBufferLayout GetLayout() const override { return m_BufferLayout; }
		VertexBufferLayout& GetLayout() override { return m_BufferLayout; }

		RendererID GetRendererID() const { return m_RendererID; }

		void Bind() const override;

	private:
		void BindVertexArray();
		void UnbindVertexArray();
		void AddVertexBufferToVertexArray();
		void AddVertexBuffer(VertexBuffer& vb);

	private:
		RendererID m_RendererID;
		Buffer m_LocalData;
		uint64_t m_Size = 0;
		VertexBufferUsage m_Usage;

		Ref<IndexBuffer> m_IndexBuffer = nullptr;
		VertexBufferLayout m_BufferLayout;

		unsigned int m_VertexArrayID = 0;
		unsigned int m_VertexBufferIndex = 0;
    };

}