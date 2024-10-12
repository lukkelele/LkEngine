#pragma once

#include "LkEngine/Renderer/IndexBuffer.h"

#include "LkOpenGL.h"


namespace LkEngine {

    class OpenGLIndexBuffer : public LIndexBuffer
    {
	public:
		OpenGLIndexBuffer(uint32_t size);
		OpenGLIndexBuffer(void* buffer, uint32_t size);
        ~OpenGLIndexBuffer();

		void SetData(void* buffer, uint64_t size, uint64_t offset = 0) override;

		FBuffer GetLocalData() override 
		{ 
			return FBuffer::Copy(m_LocalData.Data, m_Size); 
		}

		unsigned int GetCount() const { return m_Count;  }
		uint64_t GetSize() const override { return m_Size; }

		void Bind() const override;

		RendererID GetRendererID() const override { return m_RendererID; }
		RendererID& GetRendererID() override { return m_RendererID; }

	private:
		RendererID m_RendererID = 0;
		FBuffer m_LocalData;

		uint32_t m_Size = 0;
		unsigned int m_Count = 0;

		LCLASS(OpenGLIndexBuffer)
    };

}