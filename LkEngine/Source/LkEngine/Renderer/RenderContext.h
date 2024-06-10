#pragma once

#include "LkEngine/Core/Base.h"

#include "BlendingSpecification.h"

struct GLFWwindow;

namespace LkEngine {

    class LWindow;

    class RenderContext : public RefCounted
    {
    public:
        enum class EProfile 
        { 
            Core = 0, 
            Compability = 1 
        };

    public:
        virtual ~RenderContext() = default;

        static void SetProfile(const EProfile& InProfile);
        static void SetVersion(const int MajorVersion, const int MinorVersion);

        virtual void Init(const ESourceBlendFunction& InSourceFunc, const EDestinationBlendFunction& InDestinatonFunc) = 0;
        virtual void Destroy() = 0;
        virtual GLFWwindow* GetGlfwWindow() = 0;
        virtual void SetViewport(const glm::vec2& pos, const glm::vec2& size) = 0;

        virtual void SetDepthEnabled(bool enabled) = 0;
        virtual void SetDepthFunction(const EDepthFunction& depthFunc) = 0;
        virtual void SetBlendingEnabled(bool enabled) = 0;
        virtual void SetBlendFunction(const ESourceBlendFunction& InSourceFunc, const EDestinationBlendFunction& InDestinationFunc) = 0;
        virtual void SetSourceBlendFunction(const ESourceBlendFunction& InSourceFunc) = 0;
        virtual void SetDestinationBlendFunction(const EDestinationBlendFunction& InDestinationFunc) = 0;

        virtual void UpdateResolution(uint16_t width, uint16_t height) = 0;
        virtual bool GetBlendingEnabled() const = 0;

        virtual void SetName(std::string_view name) = 0;
        virtual const std::string GetName() const = 0;

        virtual std::string GetCurrentSourceBlendFunctionName() const = 0;
        virtual std::string GetCurrentDestinationBlendFunctionName() const = 0;

        static std::string GetSourceBlendFunctionName(const ESourceBlendFunction& SourceBlendFunc);
        static std::string GetDestinationBlendFunctionName(const EDestinationBlendFunction& DestinationFunction);

        static Ref<RenderContext> Create(LWindow* WindowRef);

    protected:
        static void HandleViewportEvents();

    protected:
        friend class LEditor;
    };

}
