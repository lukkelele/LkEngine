#pragma once

#include "LkEngine/Core/Base.h"
#include "LkEngine/Core/Buffer.h"

#include "IndexBuffer.h"
#include "VertexBufferLayout.h"

namespace LkEngine {

	class VertexBuffer : public RefCounted
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void SetData(void* buffer, uint64_t size, uint64_t offset = 0) = 0;
		virtual void RT_SetData(void* buffer, uint64_t size, uint64_t offset = 0) = 0;

		virtual Buffer GetLocalData() = 0;
		virtual void SetLayout(const VertexBufferLayout& layout) = 0;
		virtual void SetIndexBuffer(const Ref<IndexBuffer> indexBuffer) = 0;

		virtual void Bind() const = 0;

		virtual uint64_t GetSize() const = 0;

		virtual Ref<IndexBuffer> GetIndexBuffer() = 0;

		virtual VertexBufferLayout GetLayout() const = 0;
		virtual VertexBufferLayout& GetLayout() = 0;

		enum class VertexBufferUsage 
		{ 
			None    = 0, 
			Static  = 1, 
			Dynamic = 2 
		};

		static Ref<VertexBuffer> Create(void* buffer, uint64_t size, VertexBufferUsage usage = VertexBufferUsage::Dynamic);
		static Ref<VertexBuffer> Create(uint64_t size, VertexBufferUsage usage = VertexBufferUsage::Dynamic);

	};

}
