#pragma once

#include "LkEngine/Renderer/IndexBuffer.h"

#include "LkOpenGL.h"


namespace LkEngine {

    class OpenGLIndexBuffer : public IndexBuffer
    {
	public:
		OpenGLIndexBuffer(uint32_t size);
		OpenGLIndexBuffer(void* buffer, uint32_t size);
        ~OpenGLIndexBuffer();

		void SetData(void* buffer, uint64_t size, uint64_t offset = 0) override;
		Buffer GetLocalData() override { return Buffer::Copy(m_LocalData.Data, m_Size); }

		unsigned int GetCount() const { return m_Count;  }
		uint64_t GetSize() const override { return m_Size; }

		void Bind() const override;

		RendererID GetID() const { return m_RendererID; }

	private:
		Buffer m_LocalData;
		RendererID m_RendererID;

		uint32_t m_Size;
		unsigned int m_Count;
    };

}