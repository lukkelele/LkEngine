#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"
#include "LkEngine/Renderer/UI/Panel.h"

#include "NodeEditorContext.h"
#include "Nodes.h"

namespace LkEngine {

    namespace NE = ax::NodeEditor; 

    class LNodeEditorPanel : public IPanel
    {
    public:
        LNodeEditorPanel(std::string_view InName = PanelID::NodeEditor);
        ~LNodeEditorPanel() = default;

		virtual void Initialize() override;

		virtual void RenderUI(bool& IsOpen) override;
		virtual void Render() override {}

		virtual void SerializeToYaml(YAML::Emitter& Out) const override;
		virtual void DeserializeFromYaml(const YAML::Node& Data) override;

        TObjectPtr<LNodeEditorContext> GetEditorContext() { return EditorContext; }

        void ActivateContext();

        void BuildNode(LNode* node);
        LNode* SpawnInputActionNode();
        LNode* SpawnBranchNode();
        int GetNextID();
        NodeEd::LinkId GetNextLinkID();

        float GetTouchProgress(NodeEd::NodeId id);
        void UpdateTouch();
        void TouchNode(NE::NodeId id);

        LNode* FindNode(NE::NodeId id);
        LPinLink* FindLink(NE::LinkId id);
        LPin* FindPin(NE::PinId id);
        void IterateNodes();

        bool IsPinLinked(NE::PinId id);
        bool CanCreateLink(LPin* a, LPin* b);
        void DrawPinIcon(const LPin& pin, bool connected, int alpha);

        static ImColor GetIconColor(EPinType type);

    private:
        void Save();

    public:
        LPin* NewNodeLinkPin = nullptr;
        LPin* NewLinkPin = nullptr;
        NE::NodeId ContextNodeID = 0;
        NE::LinkId ContextLinkID = 0;
        NE::PinId ContextPinID = 0;
        bool bCreateNewNode = false;
    private:
        std::string Name{};
        TObjectPtr<LNodeEditorContext> EditorContext = nullptr;

        /* TODO: use std::array */
        std::vector<LNode> Nodes{};
        std::vector<LPinLink> Links{};

        int NextID = 1;

        std::map<NE::NodeId, float, NodeIdLess> NodeTouchTime;

        float TouchTime = 1.0f;
        int PinIconSize = 24;

        LPANEL(LNodeEditorPanel);
    };

}
