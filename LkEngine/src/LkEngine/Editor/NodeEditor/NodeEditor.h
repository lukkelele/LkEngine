#pragma once

#include <imgui-node-editor/imgui_node_editor.h>
#include <imgui-node-editor/imgui_node_editor_internal.h>

#include "NodeEditorContext.h"
#include "Nodes.h"


namespace LkEngine {

    namespace NE = ax::NodeEditor; 

    class NodeEditor
    {
    public:
        NodeEditor(std::string_view name);
        ~NodeEditor();

        void OnRender();
        void OnImGuiRender();
        void Destroy();

        void DrawPinIcon(const Pin& pin, bool connected, int alpha);
        ImColor GetIconColor(PinType type);

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
        bool IsPinLinked(NE::PinId id);
        bool CanCreateLink(Pin* a, Pin* b);

        void IterateNodes();

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
        std::string m_Name;
        NodeEditorContext* m_NodeEditorContext = nullptr;

        std::vector<Node> m_Nodes;
        std::vector<Link> m_Links;

        int m_NextID = 1;

        std::map<NE::NodeId, float, NodeIdLess> m_NodeTouchTime;
        float m_TouchTime = 1.0f;

        int m_PinIconSize = 24;
    };

}