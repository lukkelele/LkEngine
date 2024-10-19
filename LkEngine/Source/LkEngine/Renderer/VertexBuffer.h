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

		virtual void SetData(void* InBuffer, 
							 const uint64_t Size, 
							 const uint64_t Offset = 0) = 0;

		virtual void RT_SetData(void* InBuffer, 
								const uint64_t size, 
								const uint64_t offset = 0) = 0;

		virtual FBuffer GetLocalData() = 0;
		virtual void SetLayout(const VertexBufferLayout& layout) = 0;
		virtual void SetIndexBuffer(const TObjectPtr<LIndexBuffer> indexBuffer) = 0;

		virtual void Bind() const = 0;
		virtual uint64_t GetSize() const = 0;

		virtual TObjectPtr<LIndexBuffer> GetIndexBuffer() = 0;

		virtual VertexBufferLayout GetLayout() const = 0;
		virtual VertexBufferLayout& GetLayout() = 0;

		static TObjectPtr<LVertexBuffer> Create(void* InBuffer, 
												uint64_t InSize, 
												const EVertexBufferUsage InBufferUsage = EVertexBufferUsage::Dynamic);

		static TObjectPtr<LVertexBuffer> Create(const uint64_t size, 
											    const EVertexBufferUsage InBufferUsage = EVertexBufferUsage::Dynamic);

	private:
		LCLASS(LVertexBuffer)
	};

}
