#pragma once

#include "LkEngine/Core/Base.h"
#include "LkEngine/Renderer/VertexBufferLayout.h"


namespace LkEngine {

	class VertexBuffer
	{
	public:
		VertexBuffer(const void* data, unsigned int size);
		~VertexBuffer();

		static s_ptr<VertexBuffer> Create(const void* data, unsigned int size) { return std::make_shared<VertexBuffer>(data, size); }

		void Bind() const;
		void Unbind() const;
		unsigned int GetRendererID() const { return m_RendererID; }
		void SetLayout(const VertexBufferLayout& layout) { m_BufferLayout = layout; }
		VertexBufferLayout GetLayout() const { return m_BufferLayout; }

	private:
		unsigned int m_RendererID;
		VertexBufferLayout m_BufferLayout;
	};

}
