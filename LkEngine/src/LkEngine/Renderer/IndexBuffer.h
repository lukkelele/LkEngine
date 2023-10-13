#pragma once

namespace LkEngine {

	class IndexBuffer
	{
	private:
		unsigned int m_RendererID;
		unsigned int m_Count;
		
	public:
		IndexBuffer();
		IndexBuffer(const void* data, unsigned int count);
		~IndexBuffer();

		void Bind() const;
		void Unbind() const;

		unsigned int GetCount() const { return m_Count;  }
	};
}
