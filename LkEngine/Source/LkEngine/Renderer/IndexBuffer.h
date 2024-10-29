#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"
#include "LkEngine/Core/Memory/Buffer.h"


namespace LkEngine {

	class LIndexBuffer : public LObject
	{	
	public:
		virtual ~LIndexBuffer() = default;

		virtual void SetData(void* InData, 
							 const uint64_t InSize, 
							 const uint64_t InOffset = 0) = 0;

		virtual FBuffer GetLocalData() = 0;

		virtual void Bind() const = 0;

		virtual uint32_t GetCount() const = 0;
		virtual uint64_t GetSize() const = 0;

		virtual LRendererID GetRendererID() const = 0;
		virtual LRendererID& GetRendererID() = 0;

		static TObjectPtr<LIndexBuffer> Create(const uint64_t InSize);
		static TObjectPtr<LIndexBuffer> Create(void* Data, const uint64_t InSize = 0);

	private:
		LCLASS(LIndexBuffer)
	};

}
