#pragma once


namespace LkEngine {

	class IndexBuffer
	{	
	public:
		IndexBuffer(const void* data, unsigned int count);
		~IndexBuffer();

		static s_ptr<IndexBuffer> Create(const void* data, unsigned int count) { return std::make_shared<IndexBuffer>(data, count); }

		void Bind() const;
		void Unbind() const;
		unsigned int GetCount() const { return m_Count;  }

	private:
		unsigned int m_RendererID;
		unsigned int m_Count;
	};

}
