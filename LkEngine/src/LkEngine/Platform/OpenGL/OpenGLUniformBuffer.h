#pragma once

#include "LkEngine/Core/Base.h"

#include "LkEngine/Renderer/UniformBuffer.h"


namespace LkEngine {

    class OpenGLUniformBuffer : public UniformBuffer
    {
    public:
        OpenGLUniformBuffer(uint64_t size);
        ~OpenGLUniformBuffer();

        void SetData(const void* data, uint64_t size, uint64_t offset = 0) override;
        void RT_SetData(const void* data, uint64_t size, uint64_t offset = 0) override;

        void Bind();
        void Unbind();

    private:
        RendererID m_ID;
		uint64_t m_Size = 0;
		std::string m_Name;
        uint8_t* m_LocalData = nullptr;
    };

}