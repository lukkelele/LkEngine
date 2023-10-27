#pragma once

#include "LkEngine/Core/Buffer.h"


namespace LkEngine {

	class IndexBuffer
	{	
	public:
		IndexBuffer(uint32_t size);
		//IndexBuffer(const void* data, unsigned int count);
		IndexBuffer(const void* data, uint32_t size);
		~IndexBuffer();

		static s_ptr<IndexBuffer> Create(const void* data, unsigned int count) { return std::make_shared<IndexBuffer>(data, count); }

		void Bind() const;
		void Unbind() const;
		unsigned int GetCount() const { return m_Count;  }
		void SetData(const void* data, uint32_t size, uint32_t offset = 0);
		uint32_t GetSize() const { return m_Size; }
		unsigned int GetID() const { return m_RendererID; }
		Buffer GetLocalData() { return Buffer::Copy(m_LocalData.Data, m_Size); }

		static void PrintBufferContent(IndexBuffer& indexbuffer, int entries_to_print);

	private:
		unsigned int m_RendererID;
		uint32_t m_Size;
		unsigned int m_Count;
		Buffer m_LocalData;
	};

}
