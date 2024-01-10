#pragma once

#include "LkEngine/Core/Buffer.h"


namespace LkEngine {

	class IndexBuffer
	{	
	public:
		virtual ~IndexBuffer() = default;

		static s_ptr<IndexBuffer> Create(const void* data, unsigned int count);

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual unsigned int GetCount() const = 0;
		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;
		virtual uint32_t GetSize() const = 0;
		virtual unsigned int GetID() const = 0;
		virtual Buffer GetLocalData() = 0;
	};

}
