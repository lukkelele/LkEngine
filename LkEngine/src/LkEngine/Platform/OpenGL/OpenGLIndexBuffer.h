#pragma once

#include "LkEngine/Renderer/IndexBuffer.h"

#include "LkOpenGL.h"


namespace LkEngine {

    class OpenGLIndexBuffer : public IndexBuffer
    {
	public:
		OpenGLIndexBuffer(uint32_t size);
		OpenGLIndexBuffer(const void* data, uint32_t size);
        ~OpenGLIndexBuffer();

		void Bind() const;
		void Unbind() const;
		unsigned int GetCount() const { return m_Count;  }
		void SetData(const void* data, uint32_t size, uint32_t offset = 0);
		uint32_t GetSize() const { return m_Size; }
		unsigned int GetID() const { return m_RendererID; }
		Buffer GetLocalData() { return Buffer::Copy(m_LocalData.Data, m_Size); }

	private:
		RendererID m_RendererID;
		uint32_t m_Size;
		unsigned int m_Count;
		Buffer m_LocalData;
    };

}