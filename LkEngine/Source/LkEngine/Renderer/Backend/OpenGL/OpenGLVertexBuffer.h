#pragma once

#include "LkEngine/Renderer/VertexBuffer.h"


namespace LkEngine {

    class LOpenGLVertexBuffer : public LVertexBuffer
    {
	public:
		LOpenGLVertexBuffer(void* data, const uint64_t InSize, 
							const EVertexBufferUsage InBufferUsage = EVertexBufferUsage::Dynamic);
		LOpenGLVertexBuffer(const uint64_t InSize, const EVertexBufferUsage InBufferUsage = EVertexBufferUsage::Dynamic);
		~LOpenGLVertexBuffer();

		virtual void SetData(void* data, uint64_t InSize, uint64_t offset = 0) override;
		virtual void RT_SetData(void* buffer, uint64_t InSize, uint64_t offset = 0) override;

		virtual FBuffer GetLocalData() override { return m_LocalData; }
		virtual uint64_t GetSize() const override { return m_Size; }

		virtual void SetIndexBuffer(const TObjectPtr<LIndexBuffer> ib) override;
		virtual void SetLayout(const FVertexBufferLayout& layout) override; 

		virtual TObjectPtr<LIndexBuffer> GetIndexBuffer() override 
		{ 
			return m_IndexBuffer; 
		}

		FORCEINLINE virtual FVertexBufferLayout GetLayout() const override 
		{ 
			return Layout; 
		}

		FORCEINLINE virtual FVertexBufferLayout& GetLayout() override 
		{ 
			return Layout; 
		}

		FORCEINLINE LRendererID GetRendererID() const 
		{ 
			return m_RendererID; 
		}

		virtual void Bind() const override;

	private:
		void AddVertexBufferToVertexArray();
		void AddVertexBuffer(LVertexBuffer& VertexBuffer);

	private:
		LRendererID m_RendererID = 0;
		FBuffer m_LocalData{};
		uint64_t m_Size = 0;
		EVertexBufferUsage m_Usage = EVertexBufferUsage::None;

		TObjectPtr<LIndexBuffer> m_IndexBuffer = nullptr;
		FVertexBufferLayout Layout{};

		LRendererID m_VertexArrayID = 0;
		unsigned int m_VertexBufferIndex = 0;
    };

}