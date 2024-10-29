#pragma once

#include "LkEngine/Renderer/IndexBuffer.h"

#include "LkOpenGL.h"


namespace LkEngine {

    class OpenGLIndexBuffer : public LIndexBuffer
    {
	public:
		OpenGLIndexBuffer(const uint64_t InSize);
		OpenGLIndexBuffer(void* InData, const uint64_t InSize);
        ~OpenGLIndexBuffer();

		void SetData(void* InData, const uint64_t InSize, const uint64_t InOffset = 0) override;

		FORCEINLINE virtual FBuffer GetLocalData() override 
		{ 
			return FBuffer::Copy(m_LocalData.Data, m_Size); 
		}

		FORCEINLINE virtual uint64_t GetSize() const override { return m_Size; }
		FORCEINLINE virtual unsigned int GetCount() const override { return m_Count;  }

		virtual void Bind() const override;

		FORCEINLINE virtual LRendererID GetRendererID() const override { return m_RendererID; }
		FORCEINLINE virtual LRendererID& GetRendererID() override { return m_RendererID; }

	private:
		LRendererID m_RendererID = 0;
		FBuffer m_LocalData;

		uint32_t m_Size = 0;
		unsigned int m_Count = 0;

		LCLASS(OpenGLIndexBuffer)
    };

}