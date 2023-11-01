#pragma once

#include "LkEngine/Core/Base.h"
#include "LkEngine/Renderer/VertexBufferLayout.h"
#include "LkEngine/Renderer/IndexBuffer.h"


namespace LkEngine {

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;

		static s_ptr<VertexBuffer> Create(const void* data, unsigned int size);
		static s_ptr<VertexBuffer> Create(unsigned int size);

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void SetData(const void* data, unsigned int size) = 0;
		virtual void SetIndexBuffer(const s_ptr<IndexBuffer> indexBuffer) = 0;
		virtual void SetLayout(const VertexBufferLayout& layout) = 0;

		s_ptr<IndexBuffer>& GetIndexBuffer() { return m_IndexBuffer; }
		unsigned int GetID() const { return m_RendererID; }
		VertexBufferLayout GetLayout() const { return m_BufferLayout; }
		VertexBufferElement& GetElement(const std::string& elementName) { return m_BufferLayout.GetElement(elementName); }

	protected:
		unsigned int m_RendererID;
		VertexBufferLayout m_BufferLayout;
		s_ptr<IndexBuffer> m_IndexBuffer = nullptr;
	};

}
