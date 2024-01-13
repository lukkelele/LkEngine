#pragma once

#include "LkEngine/Renderer/UniformBuffer.h"

#include "LkOpenGL.h"


namespace LkEngine {

    class OpenGLUniformBuffer : public UniformBuffer
    {
    public:
        OpenGLUniformBuffer(uint32_t size, uint32_t binding);
        ~OpenGLUniformBuffer();

        virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;
        void SetData(const void* data, uint32_t size, uint32_t offset = 0);

        void Bind();
        void Unbind();

    private:
        RendererID m_RendererID = 0;
    };

}