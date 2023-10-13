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

	private:
		unsigned int m_Count = 0;
	};

}
