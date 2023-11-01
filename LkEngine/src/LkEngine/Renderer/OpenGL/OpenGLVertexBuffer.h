#pragma once

#include "LkEngine/Renderer/VertexBuffer.h"


namespace LkEngine {

    class OpenGLVertexBuffer : public VertexBuffer
    {
	public:
		OpenGLVertexBuffer(const void* data, unsigned int size);
		OpenGLVertexBuffer(unsigned int size);
		~OpenGLVertexBuffer();

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