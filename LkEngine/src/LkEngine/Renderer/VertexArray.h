#pragma once
#include "LkEngine/Core/Base.h"
#include "LkEngine/Renderer/VertexBuffer.h"
#include "LkEngine/Renderer/VertexBufferLayout.h"
#include "LkEngine/Renderer/IndexBuffer.h"


namespace LkEngine {

#if 0
	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();

		static s_ptr<VertexArray> Create() { return std::make_shared<VertexArray>(); }

		void Bind() const;
		void Unbind() const;
		void AddVertexBuffer(VertexBuffer& vb);
		s_ptr<IndexBuffer>& GetIndexBuffer() { return m_IndexBuffer; }
		void SetIndexBuffer(const s_ptr<IndexBuffer> ib);
		unsigned int GetID() { return m_RendererID; }

	private:
		unsigned int m_RendererID; // = 0;
		uint32_t m_VertexBufferIndex = 0;
		s_ptr<IndexBuffer> m_IndexBuffer;

	};
#endif

}
