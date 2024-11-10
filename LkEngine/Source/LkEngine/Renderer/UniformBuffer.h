#pragma once

#include "LkEngine/Core/Core.h"

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

//#include "LkEngine/Renderer/Shader.h"


namespace LkEngine {

    class LShader;

    class LUniformBuffer : public LObject
    {
    public:
        virtual ~LUniformBuffer() = default;

        /**
         * @brief Set buffer data.
         */
        virtual void SetData(const void* InData, const uint64_t InSize, const uint64_t InOffset = 0) = 0;

        /**
         * @brief Set buffer data.
         * @note Render thread.
         */
        virtual void RT_SetData(const void* InData, const uint64_t InSize, const uint64_t InOffset = 0) = 0;

        /**
         * @brief Set binding.
         */
        virtual void SetBinding(const TObjectPtr<LShader> Shader, 
                                std::string_view UniformName, 
                                const uint32_t BlockIndex) = 0;

        /**
         * @brief Factory function.
         */
        static TObjectPtr<LUniformBuffer> Create(const uint64_t Size);

    private:
        LCLASS(LUniformBuffer)
    };

}