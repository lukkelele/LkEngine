#pragma once

#include "LkEngine/Renderer/VertexBuffer.h"


namespace LkEngine {

    class OpenGLVertexBuffer : public LVertexBuffer
    {
	public:
		OpenGLVertexBuffer(void* data, 
						   const uint64_t InSize, 
						   const EVertexBufferUsage InBufferUsage = EVertexBufferUsage::Dynamic);
		OpenGLVertexBuffer(const uint64_t InSize, 
						   const EVertexBufferUsage InBufferUsage = EVertexBufferUsage::Dynamic);
		~OpenGLVertexBuffer();

		virtual void SetData(void* data, uint64_t InSize, uint64_t offset = 0) override;
		virtual void RT_SetData(void* buffer, uint64_t InSize, uint64_t offset = 0) override;

		virtual FBuffer GetLocalData() override { return m_LocalData; }
		virtual uint64_t GetSize() const override { return m_Size; }

		virtual void SetIndexBuffer(const TObjectPtr<LIndexBuffer> ib) override;
		virtual void SetLayout(const VertexBufferLayout& layout) override; 

		virtual TObjectPtr<LIndexBuffer> GetIndexBuffer() override 
		{ 
			return m_IndexBuffer; 
		}

		FORCEINLINE virtual VertexBufferLayout GetLayout() const override 
		{ 
			return m_BufferLayout; 
		}

		FORCEINLINE virtual VertexBufferLayout& GetLayout() override 
		{ 
			return m_BufferLayout; 
		}

		FORCEINLINE RendererID GetRendererID() const 
		{ 
			return m_RendererID; 
		}

		virtual void Bind() const override;

	private:
		void AddVertexBufferToVertexArray();
		void AddVertexBuffer(LVertexBuffer& VertexBuffer);

	private:
		RendererID m_RendererID = 0;
		FBuffer m_LocalData{};
		uint64_t m_Size = 0;
		EVertexBufferUsage m_Usage = EVertexBufferUsage::None;

		TObjectPtr<LIndexBuffer> m_IndexBuffer = nullptr;
		VertexBufferLayout m_BufferLayout;

		RendererID m_VertexArrayID = 0;
		unsigned int m_VertexBufferIndex = 0;
    };

}