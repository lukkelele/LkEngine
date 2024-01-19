#pragma once

#include "LkEngine/Core/Base.h"


namespace LkEngine {

    class UniformBuffer : public RefCounted
    {
    public:
        virtual ~UniformBuffer() = default;

        static Ref<UniformBuffer> Create(uint64_t size);

        virtual void SetData(const void* data, uint64_t size, uint64_t offset = 0) = 0;
        virtual void RT_SetData(const void* data, uint64_t size, uint64_t offset) = 0;
    };

}