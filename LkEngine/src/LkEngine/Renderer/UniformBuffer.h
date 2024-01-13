#pragma once

#include "LkEngine/Core/Base.h"


namespace LkEngine {

    class UniformBuffer
    {
    public:
        virtual ~UniformBuffer();

        static Ref<UniformBuffer> Create(uint32_t size, uint32_t binding);

        virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;
        virtual void RT_SetData(const void* data, uint32_t size, uint32_t offset) = 0;

    private:
        uint32_t m_RendererID = 0;
    };

}