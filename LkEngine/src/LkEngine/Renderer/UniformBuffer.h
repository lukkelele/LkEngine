#pragma once

#include "LkEngine/Core/Base.h"


namespace LkEngine {

    class UniformBuffer
    {
    public:
        UniformBuffer(uint32_t size, uint32_t binding);
        ~UniformBuffer();

        static s_ptr<UniformBuffer> Create(uint32_t size, uint32_t binding) { return std::make_shared<UniformBuffer>(size, binding); }

        void SetData(const void* data, uint32_t size, uint32_t offset = 0);

    private:
        uint32_t m_RendererID = 0;
    };

}