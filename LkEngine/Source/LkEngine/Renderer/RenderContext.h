#pragma once

#include "LkEngine/Core/Core.h"
#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "BlendingSpecification.h"

struct GLFWwindow;

namespace LkEngine {

    class LWindow;

    class LRenderContext : public LObject
    {
    public:
        enum class EProfile 
        { 
            Core, 
            Compability,
        };

    public:
        virtual ~LRenderContext() = default;

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

        virtual void UpdateResolution(const uint16_t Width, const uint16_t Height) = 0;
        virtual bool GetBlendingEnabled() const = 0;

        virtual void SetName(std::string_view name) = 0;
        virtual const std::string GetName() const = 0;

        /* FIXME: REMOVE */
        virtual std::string GetCurrentSourceBlendFunctionName() const = 0;
        virtual std::string GetCurrentDestinationBlendFunctionName() const = 0;

    protected:
        static void HandleViewportEvents();

    protected:
        friend class LEditor; /* FIXME: REMOVE */

    private:
        LCLASS(LRenderContext);
    };


    namespace Enum
    {
		FORCEINLINE static constexpr const char* ToString(const ESourceBlendFunction InSourceFunction)
		{
			switch (InSourceFunction)
			{
				case ESourceBlendFunction::Zero:  return "Zero";
				case ESourceBlendFunction::One:   return "One";
				case ESourceBlendFunction::Alpha: return "Alpha";
				case ESourceBlendFunction::Color: return "Color";
				case ESourceBlendFunction::One_Minus_DestinationAlpha: return "One_Minus_DestinationAlpha";
			}

            LK_CORE_ASSERT(false, "Invalid source blendfunction name");
			return "";
		}

		FORCEINLINE static constexpr const char* ToString(const EDestinationBlendFunction InDestinationFunction)
		{
			switch (InDestinationFunction)
			{
				case EDestinationBlendFunction::Zero:  return "Zero";
				case EDestinationBlendFunction::One:   return "One";
				case EDestinationBlendFunction::Alpha: return "Alpha";
				case EDestinationBlendFunction::Color: return "Color";
				case EDestinationBlendFunction::One_Minus_SourceAlpha: return "One_Minus_SourceAlpha";
			}

            LK_CORE_ASSERT(false, "Invalid destination blendfunction name");
			return "";
		}
    }

}
