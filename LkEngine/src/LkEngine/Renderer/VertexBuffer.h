#pragma once

#include "LkEngine/Core/Base.h"
#include "LkEngine/Renderer/VertexBufferLayout.h"


namespace LkEngine {

	class VertexBuffer
	{
	public:
		VertexBuffer(const void* data, unsigned int size);
		VertexBuffer(unsigned int size);
		~VertexBuffer();

		static s_ptr<VertexBuffer> Create(const void* data, unsigned int size) { return std::make_shared<VertexBuffer>(data, size); }
		static s_ptr<VertexBuffer> Create(unsigned int size) { return std::make_shared<VertexBuffer>(size); }

		void Bind() const;
		void Unbind() const;
		unsigned int GetID() const { return m_RendererID; }
		void SetData(const void* data, unsigned int size);
		void SetLayout(const VertexBufferLayout& layout) { m_BufferLayout = layout; }
		VertexBufferLayout GetLayout() const { return m_BufferLayout; }
		VertexBufferElement& GetElement(const std::string& elementName);

	private:
		unsigned int m_RendererID;
		VertexBufferLayout m_BufferLayout;
	};

}
