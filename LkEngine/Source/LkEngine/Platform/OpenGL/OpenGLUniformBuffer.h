#pragma once

#include "LkEngine/Renderer/UniformBuffer.h"


namespace LkEngine {

    class LOpenGLUniformBuffer : public LUniformBuffer
    {
    public:
        LOpenGLUniformBuffer(const uint64_t size);
		LOpenGLUniformBuffer() = delete;
        ~LOpenGLUniformBuffer();

        virtual void SetData(const void* InData, const uint64_t InSize, const uint64_t InOffset = 0) override;
        virtual void RT_SetData(const void* InData, const uint64_t InSize, const uint64_t InOffset = 0) override;

        /**
         * @brief Set uniform binding.
         */
        virtual void SetBinding(const TObjectPtr<LShader> Shader, std::string_view UBName, const uint32_t BlockIndex) override;

        void Bind();
        void Unbind();

    private:
        LRendererID m_ID;
		uint64_t m_Size = 0;
		std::string m_Name{};
        uint8_t* m_LocalData = nullptr;

        LCLASS(LOpenGLUniformBuffer)
    };

}