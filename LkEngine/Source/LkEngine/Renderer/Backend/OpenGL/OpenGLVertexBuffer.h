#pragma once

#include "LkEngine/Renderer/VertexBuffer.h"


namespace LkEngine {

    class LOpenGLVertexBuffer : public LVertexBuffer
    {
	public:
		LOpenGLVertexBuffer(void* InData, const uint32_t InSize, const EVertexBufferUsage InBufferUsage = EVertexBufferUsage::Dynamic);
		LOpenGLVertexBuffer(const uint32_t InSize, const EVertexBufferUsage InBufferUsage = EVertexBufferUsage::Dynamic);
		LOpenGLVertexBuffer() = delete;
		~LOpenGLVertexBuffer();

		virtual void SetData(void* InData, const uint32_t InSize, const uint32_t InOffset = 0) override;

		virtual FBuffer GetLocalData() override { return m_LocalData; }
		virtual uint32_t GetSize() const override { return m_Size; }

		virtual void SetIndexBuffer(const TObjectPtr<LIndexBuffer> InIndexBuffer) override;
		virtual void SetLayout(const FVertexBufferLayout& InLayout) override; 

		FORCEINLINE LRendererID GetRendererID() const { return m_RendererID; }
		FORCEINLINE virtual TObjectPtr<LIndexBuffer> GetIndexBuffer() override { return m_IndexBuffer; }

		virtual FVertexBufferLayout GetLayout() const override { return Layout; }
		virtual FVertexBufferLayout& GetLayout() override { return Layout; }

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

		LCLASS(LOpenGLVertexBuffer);
    };

}
