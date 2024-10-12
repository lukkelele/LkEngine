#pragma once

#include "LkEngine/Core/Core.h"
#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/LObjectPtr.h"

#include "BlendingSpecification.h"

struct GLFWwindow;


namespace LkEngine {

    class LWindow;

    //class LRenderContext : public RefCounted
    class LRenderContext : public LObject
    {
    public:
        enum class EProfile 
        { 
            Core = 0, 
            Compability = 1 
        };

    public:
        virtual ~LRenderContext() = default;

        //static Ref<RenderContext> Create(LWindow* WindowRef);
        static TObjectPtr<LRenderContext> Create(LWindow* InWindow);

        static void SetProfile(const EProfile& InProfile);
        static void SetVersion(const int MajorVersion, const int MinorVersion);

        virtual void Init(const ESourceBlendFunction& InSourceFunc, const EDestinationBlendFunction& InDestinatonFunc) = 0;
        virtual void Destroy() = 0;
        virtual GLFWwindow* GetGlfwWindow() = 0;
        virtual void SetViewport(const glm::vec2& pos, const glm::vec2& size) = 0;

        virtual void SetDepthEnabled(const bool InEnabled) = 0;
        virtual void SetDepthFunction(const EDepthFunction& InDepthFunc) = 0;
        virtual void SetBlendingEnabled(const bool InEnabled) = 0;
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

    protected:
        static void HandleViewportEvents();

    protected:
        friend class LEditor;

    private:
        LCLASS(LRenderContext);
    };

}
