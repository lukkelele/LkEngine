#pragma once
#include "LkEngine/Core/Base.h"


namespace LkEngine {

	class VertexBuffer
	{
	public:
		VertexBuffer(const void* data, unsigned int size);
		~VertexBuffer();

		void Bind() const;
		void Unbind() const;
		unsigned int GetRendererID() const { return m_RendererID; }

	private:
		unsigned int m_RendererID;
	};

}
