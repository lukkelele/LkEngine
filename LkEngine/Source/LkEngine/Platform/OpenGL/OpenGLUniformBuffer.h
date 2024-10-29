#pragma once

#include "LkEngine/Renderer/UniformBuffer.h"


namespace LkEngine {

    class LShader;

    class LOpenGLUniformBuffer : public LUniformBuffer
    {
    public:
        LOpenGLUniformBuffer(const uint64_t size);
        ~LOpenGLUniformBuffer();

        virtual void SetData(const void* data, 
                             const uint64_t size, 
                             const uint64_t offset = 0) override;

        virtual void RT_SetData(const void* data, 
                                const uint64_t size, 
                                const uint64_t offset = 0) override;

        void Bind();
        void Unbind();

        void SetBinding(const TObjectPtr<LShader>& targetShader, 
                        std::string_view ubName, 
                        const uint32_t blockIndex);

    private:
        LRendererID m_ID;
		uint64_t m_Size = 0;
		std::string m_Name;
        uint8_t* m_LocalData = nullptr;

        LCLASS(LOpenGLUniformBuffer)
    };

}