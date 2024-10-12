#pragma once

#include "LkEngine/Core/Core.h"

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/LObjectPtr.h"


namespace LkEngine {

    class LUniformBuffer : public LObject
    {
    public:
        virtual ~LUniformBuffer() = default;

        static TObjectPtr<LUniformBuffer> Create(const uint64_t size);

        virtual void SetData(const void* data, 
                             const uint64_t size, 
                             const uint64_t offset = 0) = 0;

        virtual void RT_SetData(const void* data, 
                                const uint64_t size, 
                                const uint64_t offset = 0) = 0;

    private:
        LCLASS(LUniformBuffer)
    };

}