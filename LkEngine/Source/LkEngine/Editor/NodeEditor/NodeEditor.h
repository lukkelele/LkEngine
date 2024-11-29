#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "NodeEditorContext.h"
#include "Nodes.h"


namespace LkEngine {

    namespace NE = ax::NodeEditor; 

    class LNodeEditor : public LObject
    {
    public:
        LNodeEditor(std::string_view InName);
        ~LNodeEditor() = default;

        void OnRender();
        void OnImGuiRender(const ImVec2& windowSize = ImVec2(0, 0));
        void Destroy();

        FORCEINLINE LNodeEditorContext* GetEditorContext() 
        { 
            return EditorContext; 
        }

        void ActivateContext();

        void BuildNode(Node* node);
        Node* SpawnInputActionNode();
        Node* SpawnBranchNode();
        int GetNextID();
        ed::LinkId GetNextLinkID();

        float GetTouchProgress(ed::NodeId id);
        void UpdateTouch();
        void TouchNode(NE::NodeId id);

        Node* FindNode(NE::NodeId id);
        Link* FindLink(NE::LinkId id);
        Pin* FindPin(NE::PinId id);
        void IterateNodes();

        bool IsPinLinked(NE::PinId id);
        bool CanCreateLink(Pin* a, Pin* b);
        void DrawPinIcon(const Pin& pin, bool connected, int alpha);
        ImColor GetIconColor(PinType type);

    private:
        void Save();

    public:
        Pin* NewNodeLinkPin = nullptr;
        Pin* NewLinkPin = nullptr;
        NE::NodeId ContextNodeID = 0;
        NE::LinkId ContextLinkID = 0;
        NE::PinId ContextPinID = 0;
        bool CreateNewNode = false;
    private:
        std::string Name{};
        LNodeEditorContext* EditorContext = nullptr;

        /* TODO: use std::array */
        std::vector<Node> m_Nodes{};
        std::vector<Link> m_Links{};

        int m_NextID = 1;

        std::map<NE::NodeId, float, NodeIdLess> m_NodeTouchTime;

        float m_TouchTime = 1.0f;
        int m_PinIconSize = 24;

        LCLASS(LNodeEditor);
    };

}
