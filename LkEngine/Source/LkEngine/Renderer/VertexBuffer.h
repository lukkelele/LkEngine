#pragma once

#include "LkEngine/Core/Core.h"
#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "IndexBuffer.h"
#include "VertexBufferLayout.h"


namespace LkEngine {

	/** EVertexBufferUsage */
	enum class EVertexBufferUsage 
	{ 
		None    = 0, 
		Static  = 1, 
		Dynamic = 2 
	};

	class LVertexBuffer : public LObject
	{
	public:
		virtual ~LVertexBuffer() = default;

		virtual void SetData(void* Buffer, const uint32_t Size, const uint32_t Offset = 0) = 0;

		virtual FBuffer GetLocalData() = 0;
		virtual void SetLayout(const FVertexBufferLayout& layout) = 0;
		virtual void SetIndexBuffer(const TObjectPtr<LIndexBuffer> indexBuffer) = 0;

		virtual void Bind() const = 0;
		virtual uint32_t GetSize() const = 0;

		virtual TObjectPtr<LIndexBuffer> GetIndexBuffer() = 0;

		virtual FVertexBufferLayout GetLayout() const = 0;
		virtual FVertexBufferLayout& GetLayout() = 0;

		static TObjectPtr<LVertexBuffer> Create(void* InBuffer, 
												const uint32_t Size, 
												const EVertexBufferUsage BufferUsage = EVertexBufferUsage::Dynamic);

		static TObjectPtr<LVertexBuffer> Create(const void* InBuffer, 
												const uint32_t Size, 
												const EVertexBufferUsage BufferUsage = EVertexBufferUsage::Dynamic);

		static TObjectPtr<LVertexBuffer> Create(const uint32_t Size, const EVertexBufferUsage BufferUsage = EVertexBufferUsage::Dynamic);

	private:
		LCLASS(LVertexBuffer)
	};

}
