#pragma once

#include "LkEngine/Core/Core.h"

#include "LkEngine/Renderer/Material.h"

#include "NodeEditor/NodeEditor.h"


namespace LkEngine {

    enum class ETabType : int
    {
        None = 0,
        Viewport,
        BlankEditor,
        NodeEditor,
        MaterialEditor,
        TextureEditor
    };

    class LTab
    {
    public:
        virtual ~LTab() = default;

        virtual void OnRender() = 0;
        virtual void OnImGuiRender() = 0;

        FORCEINLINE ETabType GetTabType() const { return Type; }

    public:
        std::string Name{};
        ETabType Type = ETabType::None;

        bool Closed = false;
        int Index = 0;     
    };


    class ViewportTab : public LTab
    {
    public:
        ViewportTab(std::string_view InName) 
        {
            Name = std::string(InName);
            Type = ETabType::Viewport;
        }

        ViewportTab(std::string_view InName, const ETabType TabType)
            : ViewportTab(InName) 
        {
        }

        ~ViewportTab() = default;

        void OnRender() override {}
        void OnImGuiRender() override {}
    };


    /**
     * LMaterialEditorTab
     */
    class LNodeEditorTab : public LTab
    {
    public:
        LNodeEditorTab(std::string_view InName);
        ~LNodeEditorTab();

        virtual void OnRender() override {};
        virtual void OnImGuiRender() override;

    public:
        TObjectPtr<LNodeEditor> NodeEditor{};
    };


    /**
     * LMaterialEditorTab
     */
    class LMaterialEditorTab: public LTab
    {
    public:
        LMaterialEditorTab(std::string_view InName);
        ~LMaterialEditorTab();

        virtual void OnRender() override;
        virtual void OnImGuiRender() override;

    public:
        TObjectPtr<LNodeEditor> NodeEditor{};
        TObjectPtr<LMaterial> MaterialRef = nullptr;
    };


}