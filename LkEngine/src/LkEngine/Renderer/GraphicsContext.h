#pragma once

#include "LkEngine/Core/Base.h"

#include "BlendingSpecification.h"

struct GLFWwindow;

namespace LkEngine {

    class Window;

    class GraphicsContext : public RefCounted
    {
    public:
        enum class Profile { Core = 0, Compability = 1 };

    public:
        virtual ~GraphicsContext() = default;

        static Ref<GraphicsContext> Get() { return m_Instance; }
        static Ref<GraphicsContext> Create(Window* window);

        static void SetProfile(const Profile& profile);
        static void SetVersion(int majorVersion, int minorVersion);

        virtual void Init(const SourceBlendFunction& srcFunc, const DestinationBlendFunction& dstFunc) = 0;
        virtual void Destroy() = 0;
        virtual GLFWwindow* GetGlfwWindow() = 0;
        virtual void SetViewport(const glm::vec2& pos, const glm::vec2& size) = 0;
        virtual void SetDepthEnabled(bool enabled) = 0;
        virtual void SetBlendingEnabled(bool enabled) = 0;
        virtual void SetBlendFunction(const SourceBlendFunction& srcFunc, const DestinationBlendFunction& dstFunc) = 0;
        virtual void SetSourceBlendFunction(const SourceBlendFunction& srcFunc) = 0;
        virtual void SetDestinationBlendFunction(const DestinationBlendFunction& dstFunc) = 0;
        virtual void UpdateResolution(uint16_t width, uint16_t height) = 0;

        virtual void SetName(std::string_view name) = 0;
        virtual const std::string GetName() const = 0;

        virtual std::string GetCurrentSourceBlendFunctionName() const = 0;
        virtual std::string GetCurrentDestinationBlendFunctionName() const = 0;

        static std::string GetSourceBlendFunctionName(const SourceBlendFunction& srcFunc);
        static std::string GetDestinationBlendFunctionName(const DestinationBlendFunction& dstFunc);

    protected:
        static void HandleViewportEvents();

        // For editor
        virtual bool& GetBlending() = 0;
    
    protected:
        inline static Ref<GraphicsContext> m_Instance = nullptr;

        friend class Editor;
    };

}
