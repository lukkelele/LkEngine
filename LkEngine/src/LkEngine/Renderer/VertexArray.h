#pragma once
#include "LkEngine/Core/Base.h"
#include "LkEngine/Renderer/VertexBuffer.h"
#include "LkEngine/Renderer/VertexBufferLayout.h"
#include "LkEngine/Renderer/IndexBuffer.h"


namespace LkEngine {

	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();

		void Bind() const;
		void Unbind() const;
		void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
		std::vector<s_ptr<VertexBuffer>>& GetVertexBuffers() { return m_VertexBuffers;  }
		s_ptr<IndexBuffer>& GetIndexBuffer() { return m_IndexBuffer; }

	private:
		unsigned int m_RendererID; // = 0;
		uint32_t m_VertexBufferIndex = 0;
		std::vector<s_ptr<VertexBuffer>> m_VertexBuffers;
		s_ptr<IndexBuffer> m_IndexBuffer;

	};

}