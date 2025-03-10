#pragma once

#include "LkEngine/Core/CoreMacros.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui-node-editor/imgui_node_editor.h>


namespace LkEngine {
    
    namespace NodeEd = ax::NodeEditor;

    enum class EPinType
    {
        Flow,
        Bool,
        Int,
        Float,
        String,
        Object,
        Function,
        Delegate,
    };
	LK_ENUM_CLASS(EPinType);

    enum class EPinKind
    {
        Input,
        Output,
    };
	LK_ENUM_CLASS(EPinKind);

    enum class ENodeType
    {
        Blueprint,
        Simple,
        Tree,
        Comment,
        Houdini
    };
	LK_ENUM_CLASS(ENodeType);

    struct LNode;

    struct LPin
    {
        NodeEd::PinId ID;
		LNode* Node{};
        std::string Name;
        EPinType Type;
        EPinKind Kind;

        LPin(const int InID, const char* InName, const EPinType InType) 
			: ID(InID)
			, Node(nullptr)
			, Name(InName)
			, Type(InType)
			, Kind(EPinKind::Input)
        {
        }
    };

	enum class ENodeIconType : uint32_t
	{ 
		Flow, 
		Circle, 
		Square, 
		Grid, 
		RoundSquare, 
		Diamond 
	};
    
    struct LNode
    {
		NodeEd::NodeId ID{};
		std::string Name{};
        ImColor Color;
        ENodeType Type;
		ImVec2 Size{};

		std::vector<LPin> Inputs{};
		std::vector<LPin> Outputs{};

        std::string State;
        std::string SavedState;

        LNode(const int InID, const char* InName, const ImColor InColor = ImColor(255, 255, 255)) 
			: ID(InID)
			, Name(InName)
			, Color(InColor)
			, Type(ENodeType::Blueprint)
			, Size(0, 0)
        {
        }
    };

    struct LPinLink
    {
        NodeEd::LinkId ID;

        NodeEd::PinId StartPinID;
        NodeEd::PinId EndPinID;

        ImColor Color;

        LPinLink(const NodeEd::LinkId InID, NodeEd::PinId InStartPinId, NodeEd::PinId InEndPinId) 
			: ID(InID)
			, StartPinID(InStartPinId)
			, EndPinID(InEndPinId)
			, Color(255, 255, 255)
        {
        }
    };

    struct NodeIdLess
    {
        bool operator()(const NodeEd::NodeId& lhs, const NodeEd::NodeId& rhs) const
        {
            return lhs.AsPointer() < rhs.AsPointer();
        }
    };

	static NodeEd::PinKind GetPinKind(const EPinKind Kind)
	{
		switch (Kind)
		{
			case EPinKind::Input:  return NodeEd::PinKind::Input;
			case EPinKind::Output: return NodeEd::PinKind::Output;
		}

		LK_CORE_VERIFY(false, "GetPinKind failed for value: {}", static_cast<int>(Kind));
		return NodeEd::PinKind::Input;
	}

	static NodeEd::PinKind GetPinKind(const LPin& Pin)
	{
		switch (Pin.Kind)
		{
			case EPinKind::Input:  return NodeEd::PinKind::Input;
			case EPinKind::Output: return NodeEd::PinKind::Output;
		}

		LK_CORE_VERIFY(false, "GetPinKind failed for value: {}", static_cast<int>(Pin.Kind));
		return NodeEd::PinKind::Input;
	}


#if 0
    void BuildNode(Node* node)
    {
        for (auto& input : node->Inputs)
        {
            input.Node = node;
            input.Kind = PinKind::Input;
        }

        for (auto& output : node->Outputs)
        {
            output.Node = node;
            output.Kind = PinKind::Output;
        }
    }

    Node* SpawnInputActionNode()
    {
        m_Nodes.emplace_back(GetNextId(), "InputAction Fire", ImColor(255, 128, 128));
        m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Delegate);
        m_Nodes.back().Outputs.emplace_back(GetNextId(), "Pressed", PinType::Flow);
        m_Nodes.back().Outputs.emplace_back(GetNextId(), "Released", PinType::Flow);

        BuildNode(&m_Nodes.back());

        return &m_Nodes.back();
    }

    Node* SpawnBranchNode()
    {
        m_Nodes.emplace_back(GetNextId(), "Branch");
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "Condition", PinType::Bool);
        m_Nodes.back().Outputs.emplace_back(GetNextId(), "True", PinType::Flow);
        m_Nodes.back().Outputs.emplace_back(GetNextId(), "False", PinType::Flow);

        BuildNode(&m_Nodes.back());

        return &m_Nodes.back();
    }

    Node* SpawnDoNNode()
    {
        m_Nodes.emplace_back(GetNextId(), "Do N");
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "Enter", PinType::Flow);
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "N", PinType::Int);
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "Reset", PinType::Flow);
        m_Nodes.back().Outputs.emplace_back(GetNextId(), "Exit", PinType::Flow);
        m_Nodes.back().Outputs.emplace_back(GetNextId(), "Counter", PinType::Int);

        BuildNode(&m_Nodes.back());

        return &m_Nodes.back();
    }

    Node* SpawnOutputActionNode()
    {
        m_Nodes.emplace_back(GetNextId(), "OutputAction");
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "Sample", PinType::Float);
        m_Nodes.back().Outputs.emplace_back(GetNextId(), "Condition", PinType::Bool);
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "Event", PinType::Delegate);

        BuildNode(&m_Nodes.back());

        return &m_Nodes.back();
    }

    Node* SpawnPrintStringNode()
    {
        m_Nodes.emplace_back(GetNextId(), "Print String");
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "In String", PinType::String);
        m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Flow);

        BuildNode(&m_Nodes.back());

        return &m_Nodes.back();
    }

    Node* SpawnMessageNode()
    {
        m_Nodes.emplace_back(GetNextId(), "", ImColor(128, 195, 248));
        m_Nodes.back().Type = NodeType::Simple;
        m_Nodes.back().Outputs.emplace_back(GetNextId(), "Message", PinType::String);

        BuildNode(&m_Nodes.back());

        return &m_Nodes.back();
    }

    Node* SpawnSetTimerNode()
    {
        m_Nodes.emplace_back(GetNextId(), "Set Timer", ImColor(128, 195, 248));
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "Object", PinType::Object);
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "Function Name", PinType::Function);
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "Time", PinType::Float);
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "Looping", PinType::Bool);
        m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Flow);

        BuildNode(&m_Nodes.back());

        return &m_Nodes.back();
    }

    static bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f)
    {
        using namespace ImGui;
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        ImGuiID id = window->GetID("##Splitter");
        ImRect bb;
        bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
        bb.Max = bb.Min + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
        return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
    }

        int GetNextId()
        {
            return m_NextId++;
        }

        //NodeEd::NodeId GetNextNodeId()
        //{
        //    return NodeEd::NodeId(GetNextId());
        //}

        NodeEd::LinkId GetNextLinkId()
        {
            return NodeEd::LinkId(GetNextId());
        }

        void TouchNode(NodeEd::NodeId id)
        {
            m_NodeTouchTime[id] = m_TouchTime;
        }

        float GetTouchProgress(NodeEd::NodeId id)
        {
            auto it = m_NodeTouchTime.find(id);
            if (it != m_NodeTouchTime.end() && it->second > 0.0f)
                return (m_TouchTime - it->second) / m_TouchTime;
            else
                return 0.0f;
        }

        void UpdateTouch()
        {
            const auto deltaTime = ImGui::GetIO().DeltaTime;
            for (auto& entry : m_NodeTouchTime)
            {
                if (entry.second > 0.0f)
                    entry.second -= deltaTime;
            }
        }

        Node* FindNode(NodeEd::NodeId id)
        {
            for (auto& node : m_Nodes)
                if (node.ID == id)
                    return &node;

            return nullptr;
        }

        Link* FindLink(NodeEd::LinkId id)
        {
            for (auto& link : m_Links)
                if (link.ID == id)
                    return &link;

            return nullptr;
        }

        Pin* FindPin(NodeEd::PinId id)
        {
            if (!id)
                return nullptr;

            for (auto& node : m_Nodes)
            {
                for (auto& pin : node.Inputs)
                    if (pin.ID == id)
                        return &pin;

                for (auto& pin : node.Outputs)
                    if (pin.ID == id)
                        return &pin;
            }

            return nullptr;
        }

        bool IsPinLinked(NodeEd::PinId id)
        {
            if (!id)
                return false;

            for (auto& link : m_Links)
                if (link.StartPinID == id || link.EndPinID == id)
                    return true;

            return false;
        }

        bool CanCreateLink(Pin* a, Pin* b)
        {
            if (!a || !b || a == b || a->Kind == b->Kind || a->Type != b->Type || a->Node == b->Node)
                return false;

            return true;
        }

        //void DrawItemRect(ImColor color, float expand = 0.0f)
        //{
        //    ImGui::GetWindowDrawList()->AddRect(
        //        ImGui::GetItemRectMin() - ImVec2(expand, expand),
        //        ImGui::GetItemRectMax() + ImVec2(expand, expand),
        //        color);
        //};

        //void FillItemRect(ImColor color, float expand = 0.0f, float rounding = 0.0f)
        //{
        //    ImGui::GetWindowDrawList()->AddRectFilled(
        //        ImGui::GetItemRectMin() - ImVec2(expand, expand),
        //        ImGui::GetItemRectMax() + ImVec2(expand, expand),
        //        color, rounding);
        //};

        void BuildNode(Node* node)
        {
            for (auto& input : node->Inputs)
            {
                input.Node = node;
                input.Kind = PinKind::Input;
            }

            for (auto& output : node->Outputs)
            {
                output.Node = node;
                output.Kind = PinKind::Output;
            }
        }

        Node* SpawnInputActionNode()
        {
            m_Nodes.emplace_back(GetNextId(), "InputAction Fire", ImColor(255, 128, 128));
            m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Delegate);
            m_Nodes.back().Outputs.emplace_back(GetNextId(), "Pressed", PinType::Flow);
            m_Nodes.back().Outputs.emplace_back(GetNextId(), "Released", PinType::Flow);

            BuildNode(&m_Nodes.back());

            return &m_Nodes.back();
        }

        Node* SpawnBranchNode()
        {
            m_Nodes.emplace_back(GetNextId(), "Branch");
            m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
            m_Nodes.back().Inputs.emplace_back(GetNextId(), "Condition", PinType::Bool);
            m_Nodes.back().Outputs.emplace_back(GetNextId(), "True", PinType::Flow);
            m_Nodes.back().Outputs.emplace_back(GetNextId(), "False", PinType::Flow);

            BuildNode(&m_Nodes.back());

            return &m_Nodes.back();
        }

        Node* SpawnDoNNode()
        {
            m_Nodes.emplace_back(GetNextId(), "Do N");
            m_Nodes.back().Inputs.emplace_back(GetNextId(), "Enter", PinType::Flow);
            m_Nodes.back().Inputs.emplace_back(GetNextId(), "N", PinType::Int);
            m_Nodes.back().Inputs.emplace_back(GetNextId(), "Reset", PinType::Flow);
            m_Nodes.back().Outputs.emplace_back(GetNextId(), "Exit", PinType::Flow);
            m_Nodes.back().Outputs.emplace_back(GetNextId(), "Counter", PinType::Int);

            BuildNode(&m_Nodes.back());

            return &m_Nodes.back();
        }

        Node* SpawnOutputActionNode()
        {
            m_Nodes.emplace_back(GetNextId(), "OutputAction");
            m_Nodes.back().Inputs.emplace_back(GetNextId(), "Sample", PinType::Float);
            m_Nodes.back().Outputs.emplace_back(GetNextId(), "Condition", PinType::Bool);
            m_Nodes.back().Inputs.emplace_back(GetNextId(), "Event", PinType::Delegate);

            BuildNode(&m_Nodes.back());

            return &m_Nodes.back();
        }

        Node* SpawnPrintStringNode()
        {
            m_Nodes.emplace_back(GetNextId(), "Print String");
            m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
            m_Nodes.back().Inputs.emplace_back(GetNextId(), "In String", PinType::String);
            m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Flow);

            BuildNode(&m_Nodes.back());

            return &m_Nodes.back();
        }

        Node* SpawnMessageNode()
        {
            m_Nodes.emplace_back(GetNextId(), "", ImColor(128, 195, 248));
            m_Nodes.back().Type = NodeType::Simple;
            m_Nodes.back().Outputs.emplace_back(GetNextId(), "Message", PinType::String);

            BuildNode(&m_Nodes.back());

            return &m_Nodes.back();
        }

        Node* SpawnSetTimerNode()
        {
            m_Nodes.emplace_back(GetNextId(), "Set Timer", ImColor(128, 195, 248));
            m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
            m_Nodes.back().Inputs.emplace_back(GetNextId(), "Object", PinType::Object);
            m_Nodes.back().Inputs.emplace_back(GetNextId(), "Function Name", PinType::Function);
            m_Nodes.back().Inputs.emplace_back(GetNextId(), "Time", PinType::Float);
            m_Nodes.back().Inputs.emplace_back(GetNextId(), "Looping", PinType::Bool);
            m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Flow);

            BuildNode(&m_Nodes.back());

            return &m_Nodes.back();
        }

        Node* SpawnLessNode()
        {
            m_Nodes.emplace_back(GetNextId(), "<", ImColor(128, 195, 248));
            m_Nodes.back().Type = NodeType::Simple;
            m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Float);
            m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Float);
            m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Float);

            BuildNode(&m_Nodes.back());

            return &m_Nodes.back();
        }

        Node* SpawnWeirdNode()
        {
            m_Nodes.emplace_back(GetNextId(), "o.O", ImColor(128, 195, 248));
            m_Nodes.back().Type = NodeType::Simple;
            m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Float);
            m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Float);
            m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Float);

            BuildNode(&m_Nodes.back());

            return &m_Nodes.back();
        }

        Node* SpawnTraceByChannelNode()
        {
            m_Nodes.emplace_back(GetNextId(), "Single Line Trace by Channel", ImColor(255, 128, 64));
            m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
            m_Nodes.back().Inputs.emplace_back(GetNextId(), "Start", PinType::Flow);
            m_Nodes.back().Inputs.emplace_back(GetNextId(), "End", PinType::Int);
            m_Nodes.back().Inputs.emplace_back(GetNextId(), "Trace Channel", PinType::Float);
            m_Nodes.back().Inputs.emplace_back(GetNextId(), "Trace Complex", PinType::Bool);
            m_Nodes.back().Inputs.emplace_back(GetNextId(), "Actors to Ignore", PinType::Int);
            m_Nodes.back().Inputs.emplace_back(GetNextId(), "Draw Debug Type", PinType::Bool);
            m_Nodes.back().Inputs.emplace_back(GetNextId(), "Ignore Self", PinType::Bool);
            m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Flow);
            m_Nodes.back().Outputs.emplace_back(GetNextId(), "Out Hit", PinType::Float);
            m_Nodes.back().Outputs.emplace_back(GetNextId(), "Return Value", PinType::Bool);

            BuildNode(&m_Nodes.back());

            return &m_Nodes.back();
        }

        Node* SpawnTreeSequenceNode()
        {
            m_Nodes.emplace_back(GetNextId(), "Sequence");
            m_Nodes.back().Type = NodeType::Tree;
            m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
            m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Flow);

            BuildNode(&m_Nodes.back());

            return &m_Nodes.back();
        }

        Node* SpawnTreeTaskNode()
        {
            m_Nodes.emplace_back(GetNextId(), "Move To");
            m_Nodes.back().Type = NodeType::Tree;
            m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);

            BuildNode(&m_Nodes.back());

            return &m_Nodes.back();
        }

        Node* SpawnTreeTask2Node()
        {
            m_Nodes.emplace_back(GetNextId(), "Random Wait");
            m_Nodes.back().Type = NodeType::Tree;
            m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);

            BuildNode(&m_Nodes.back());

            return &m_Nodes.back();
        }

        Node* SpawnComment()
        {
            m_Nodes.emplace_back(GetNextId(), "Test Comment");
            m_Nodes.back().Type = NodeType::Comment;
            m_Nodes.back().Size = ImVec2(300, 200);

            return &m_Nodes.back();
        }

        Node* SpawnHoudiniTransformNode()
        {
            m_Nodes.emplace_back(GetNextId(), "Transform");
            m_Nodes.back().Type = NodeType::Houdini;
            m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
            m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Flow);

            BuildNode(&m_Nodes.back());

            return &m_Nodes.back();
        }

        Node* SpawnHoudiniGroupNode()
        {
            m_Nodes.emplace_back(GetNextId(), "Group");
            m_Nodes.back().Type = NodeType::Houdini;
            m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
            m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
            m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Flow);

            BuildNode(&m_Nodes.back());

            return &m_Nodes.back();
        }

        void BuildNodes()
        {
            for (auto& node : m_Nodes)
                BuildNode(&node);
        }

        void OnStart() override
        {
            NodeEd::Config config;

            config.SettingsFile = "Blueprints.json";

            config.UserPointer = this;

            config.LoadNodeSettings = [](NodeEd::NodeId nodeId, char* data, void* userPointer) -> size_t
                {
                    auto self = static_cast<Example*>(userPointer);

                    auto node = self->FindNode(nodeId);
                    if (!node)
                        return 0;

                    if (data != nullptr)
                        memcpy(data, node->State.data(), node->State.size());
                    return node->State.size();
                };

            config.SaveNodeSettings = [](NodeEd::NodeId nodeId, const char* data, size_t size, NodeEd::SaveReasonFlags reason, void* userPointer) -> bool
                {
                    auto self = static_cast<Example*>(userPointer);

                    auto node = self->FindNode(nodeId);
                    if (!node)
                        return false;

                    node->State.assign(data, size);

                    self->TouchNode(nodeId);

                    return true;
                };

            m_Editor = NodeEd::CreateEditor(&config);
            NodeEd::SetCurrentEditor(m_Editor);

            Node* node;
            node = SpawnInputActionNode();      NodeEd::SetNodePosition(node->ID, ImVec2(-252, 220));
            node = SpawnBranchNode();           NodeEd::SetNodePosition(node->ID, ImVec2(-300, 351));
            node = SpawnDoNNode();              NodeEd::SetNodePosition(node->ID, ImVec2(-238, 504));
            node = SpawnOutputActionNode();     NodeEd::SetNodePosition(node->ID, ImVec2(71, 80));
            node = SpawnSetTimerNode();         NodeEd::SetNodePosition(node->ID, ImVec2(168, 316));

            node = SpawnTreeSequenceNode();     NodeEd::SetNodePosition(node->ID, ImVec2(1028, 329));
            node = SpawnTreeTaskNode();         NodeEd::SetNodePosition(node->ID, ImVec2(1204, 458));
            node = SpawnTreeTask2Node();        NodeEd::SetNodePosition(node->ID, ImVec2(868, 538));

            node = SpawnComment();              NodeEd::SetNodePosition(node->ID, ImVec2(112, 576)); NodeEd::SetGroupSize(node->ID, ImVec2(384, 154));
            node = SpawnComment();              NodeEd::SetNodePosition(node->ID, ImVec2(800, 224)); NodeEd::SetGroupSize(node->ID, ImVec2(640, 400));

            node = SpawnLessNode();             NodeEd::SetNodePosition(node->ID, ImVec2(366, 652));
            node = SpawnWeirdNode();            NodeEd::SetNodePosition(node->ID, ImVec2(144, 652));
            node = SpawnMessageNode();          NodeEd::SetNodePosition(node->ID, ImVec2(-348, 698));
            node = SpawnPrintStringNode();      NodeEd::SetNodePosition(node->ID, ImVec2(-69, 652));

            node = SpawnHoudiniTransformNode(); NodeEd::SetNodePosition(node->ID, ImVec2(500, -70));
            node = SpawnHoudiniGroupNode();     NodeEd::SetNodePosition(node->ID, ImVec2(500, 42));

            NodeEd::NavigateToContent();

            BuildNodes();

            m_Links.push_back(Link(GetNextLinkId(), m_Nodes[5].Outputs[0].ID, m_Nodes[6].Inputs[0].ID));
            m_Links.push_back(Link(GetNextLinkId(), m_Nodes[5].Outputs[0].ID, m_Nodes[7].Inputs[0].ID));

            m_Links.push_back(Link(GetNextLinkId(), m_Nodes[14].Outputs[0].ID, m_Nodes[15].Inputs[0].ID));

            m_HeaderBackground = LoadTexture("data/BlueprintBackground.png");
            m_SaveIcon = LoadTexture("data/ic_save_white_24dp.png");
            m_RestoreIcon = LoadTexture("data/ic_restore_white_24dp.png");


            //auto& io = ImGui::GetIO();
        }

        void OnStop() override
        {
            auto releaseTexture = [this](ImTextureID& id)
                {
                    if (id)
                    {
                        DestroyTexture(id);
                        id = nullptr;
                    }
                };

            releaseTexture(m_RestoreIcon);
            releaseTexture(m_SaveIcon);
            releaseTexture(m_HeaderBackground);

            if (m_Editor)
            {
                NodeEd::DestroyEditor(m_Editor);
                m_Editor = nullptr;
            }
        }

        ImColor GetIconColor(PinType type)
        {
            switch (type)
            {
            default:
            case PinType::Flow:     return ImColor(255, 255, 255);
            case PinType::Bool:     return ImColor(220, 48, 48);
            case PinType::Int:      return ImColor(68, 201, 156);
            case PinType::Float:    return ImColor(147, 226, 74);
            case PinType::String:   return ImColor(124, 21, 153);
            case PinType::Object:   return ImColor(51, 150, 215);
            case PinType::Function: return ImColor(218, 0, 183);
            case PinType::Delegate: return ImColor(255, 48, 48);
            }
        };

        void DrawPinIcon(const Pin& pin, bool connected, int alpha)
        {
            IconType iconType;
            ImColor  color = GetIconColor(pin.Type);
            color.Value.w = alpha / 255.0f;
            switch (pin.Type)
            {
            case PinType::Flow:     iconType = IconType::Flow;   break;
            case PinType::Bool:     iconType = IconType::Circle; break;
            case PinType::Int:      iconType = IconType::Circle; break;
            case PinType::Float:    iconType = IconType::Circle; break;
            case PinType::String:   iconType = IconType::Circle; break;
            case PinType::Object:   iconType = IconType::Circle; break;
            case PinType::Function: iconType = IconType::Circle; break;
            case PinType::Delegate: iconType = IconType::Square; break;
            default:
                return;
            }

            ax::Widgets::Icon(ImVec2(static_cast<float>(m_PinIconSize), static_cast<float>(m_PinIconSize)), iconType, connected, color, ImColor(32, 32, 32, alpha));
        };

        void ShowStyleEditor(bool* show = nullptr)
        {
            if (!ImGui::Begin("Style", show))
            {
                ImGui::End();
                return;
            }

            auto paneWidth = ImGui::GetContentRegionAvail().x;

            auto& editorStyle = NodeEd::GetStyle();
            ImGui::BeginHorizontal("Style buttons", ImVec2(paneWidth, 0), 1.0f);
            ImGui::TextUnformatted("Values");
            ImGui::Spring();
            if (ImGui::Button("Reset to defaults"))
                editorStyle = NodeEd::Style();
            ImGui::EndHorizontal();
            ImGui::Spacing();
            ImGui::DragFloat4("Node Padding", &editorStyle.NodePadding.x, 0.1f, 0.0f, 40.0f);
            ImGui::DragFloat("Node Rounding", &editorStyle.NodeRounding, 0.1f, 0.0f, 40.0f);
            ImGui::DragFloat("Node Border Width", &editorStyle.NodeBorderWidth, 0.1f, 0.0f, 15.0f);
            ImGui::DragFloat("Hovered Node Border Width", &editorStyle.HoveredNodeBorderWidth, 0.1f, 0.0f, 15.0f);
            ImGui::DragFloat("Hovered Node Border Offset", &editorStyle.HoverNodeBorderOffset, 0.1f, -40.0f, 40.0f);
            ImGui::DragFloat("Selected Node Border Width", &editorStyle.SelectedNodeBorderWidth, 0.1f, 0.0f, 15.0f);
            ImGui::DragFloat("Selected Node Border Offset", &editorStyle.SelectedNodeBorderOffset, 0.1f, -40.0f, 40.0f);
            ImGui::DragFloat("Pin Rounding", &editorStyle.PinRounding, 0.1f, 0.0f, 40.0f);
            ImGui::DragFloat("Pin Border Width", &editorStyle.PinBorderWidth, 0.1f, 0.0f, 15.0f);
            ImGui::DragFloat("Link Strength", &editorStyle.LinkStrength, 1.0f, 0.0f, 500.0f);
            //ImVec2  SourceDirection;
            //ImVec2  TargetDirection;
            ImGui::DragFloat("Scroll Duration", &editorStyle.ScrollDuration, 0.001f, 0.0f, 2.0f);
            ImGui::DragFloat("Flow Marker Distance", &editorStyle.FlowMarkerDistance, 1.0f, 1.0f, 200.0f);
            ImGui::DragFloat("Flow Speed", &editorStyle.FlowSpeed, 1.0f, 1.0f, 2000.0f);
            ImGui::DragFloat("Flow Duration", &editorStyle.FlowDuration, 0.001f, 0.0f, 5.0f);
            //ImVec2  PivotAlignment;
            //ImVec2  PivotSize;
            //ImVec2  PivotScale;
            //float   PinCorners;
            //float   PinRadius;
            //float   PinArrowSize;
            //float   PinArrowWidth;
            ImGui::DragFloat("Group Rounding", &editorStyle.GroupRounding, 0.1f, 0.0f, 40.0f);
            ImGui::DragFloat("Group Border Width", &editorStyle.GroupBorderWidth, 0.1f, 0.0f, 15.0f);

            ImGui::Separator();

            static ImGuiColorEditFlags edit_mode = ImGuiColorEditFlags_DisplayRGB;
            ImGui::BeginHorizontal("Color Mode", ImVec2(paneWidth, 0), 1.0f);
            ImGui::TextUnformatted("Filter Colors");
            ImGui::Spring();
            ImGui::RadioButton("RGB", &edit_mode, ImGuiColorEditFlags_DisplayRGB);
            ImGui::Spring(0);
            ImGui::RadioButton("HSV", &edit_mode, ImGuiColorEditFlags_DisplayHSV);
            ImGui::Spring(0);
            ImGui::RadioButton("HEX", &edit_mode, ImGuiColorEditFlags_DisplayHex);
            ImGui::EndHorizontal();

            static ImGuiTextFilter filter;
            filter.Draw("##filter", paneWidth);

            ImGui::Spacing();

            ImGui::PushItemWidth(-160);
            for (int i = 0; i < NodeEd::StyleColor_Count; ++i)
            {
                auto name = NodeEd::GetStyleColorName((NodeEd::StyleColor)i);
                if (!filter.PassFilter(name))
                    continue;

                ImGui::ColorEdit4(name, &editorStyle.Colors[i].x, edit_mode);
            }
            ImGui::PopItemWidth();

            ImGui::End();
        }

        void ShowLeftPane(float paneWidth)
        {
            auto& io = ImGui::GetIO();

            ImGui::BeginChild("Selection", ImVec2(paneWidth, 0));

            paneWidth = ImGui::GetContentRegionAvail().x;

            static bool showStyleEditor = false;
            ImGui::BeginHorizontal("Style Editor", ImVec2(paneWidth, 0));
            ImGui::Spring(0.0f, 0.0f);
            if (ImGui::Button("Zoom to Content"))
                NodeEd::NavigateToContent();
            ImGui::Spring(0.0f);
            if (ImGui::Button("Show Flow"))
            {
                for (auto& link : m_Links)
                    NodeEd::Flow(link.ID);
            }
            ImGui::Spring();
            if (ImGui::Button("Edit Style"))
                showStyleEditor = true;
            ImGui::EndHorizontal();
            ImGui::Checkbox("Show Ordinals", &m_ShowOrdinals);

            if (showStyleEditor)
                ShowStyleEditor(&showStyleEditor);

            std::vector<NodeEd::NodeId> selectedNodes;
            std::vector<NodeEd::LinkId> selectedLinks;
            selectedNodes.resize(NodeEd::GetSelectedObjectCount());
            selectedLinks.resize(NodeEd::GetSelectedObjectCount());

            int nodeCount = NodeEd::GetSelectedNodes(selectedNodes.data(), static_cast<int>(selectedNodes.size()));
            int linkCount = NodeEd::GetSelectedLinks(selectedLinks.data(), static_cast<int>(selectedLinks.size()));

            selectedNodes.resize(nodeCount);
            selectedLinks.resize(linkCount);

            int saveIconWidth = GetTextureWidth(m_SaveIcon);
            int saveIconHeight = GetTextureWidth(m_SaveIcon);
            int restoreIconWidth = GetTextureWidth(m_RestoreIcon);
            int restoreIconHeight = GetTextureWidth(m_RestoreIcon);

            ImGui::GetWindowDrawList()->AddRectFilled(
                ImGui::GetCursorScreenPos(),
                ImGui::GetCursorScreenPos() + ImVec2(paneWidth, ImGui::GetTextLineHeight()),
                ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]), ImGui::GetTextLineHeight() * 0.25f);
            ImGui::Spacing(); ImGui::SameLine();
            ImGui::TextUnformatted("Nodes");
            ImGui::Indent();
            for (auto& node : m_Nodes)
            {
                ImGui::PushID(node.ID.AsPointer());
                auto start = ImGui::GetCursorScreenPos();

                if (const auto progress = GetTouchProgress(node.ID))
                {
                    ImGui::GetWindowDrawList()->AddLine(
                        start + ImVec2(-8, 0),
                        start + ImVec2(-8, ImGui::GetTextLineHeight()),
                        IM_COL32(255, 0, 0, 255 - (int)(255 * progress)), 4.0f);
                }

                bool isSelected = std::find(selectedNodes.begin(), selectedNodes.end(), node.ID) != selectedNodes.end();
# if IMGUI_VERSION_NUM >= 18967
                ImGui::SetNextItemAllowOverlap();
# endif
                if (ImGui::Selectable((node.Name + "##" + std::to_string(reinterpret_cast<uintptr_t>(node.ID.AsPointer()))).c_str(), &isSelected))
                {
                    if (io.KeyCtrl)
                    {
                        if (isSelected)
                            NodeEd::SelectNode(node.ID, true);
                        else
                            NodeEd::DeselectNode(node.ID);
                    }
                    else
                        NodeEd::SelectNode(node.ID, false);

                    NodeEd::NavigateToSelection();
                }
                if (ImGui::IsItemHovered() && !node.State.empty())
                    ImGui::SetTooltip("State: %s", node.State.c_str());

                auto id = std::string("(") + std::to_string(reinterpret_cast<uintptr_t>(node.ID.AsPointer())) + ")";
                auto textSize = ImGui::CalcTextSize(id.c_str(), nullptr);
                auto iconPanelPos = start + ImVec2(
                    paneWidth - ImGui::GetStyle().FramePadding.x - ImGui::GetStyle().IndentSpacing - saveIconWidth - restoreIconWidth - ImGui::GetStyle().ItemInnerSpacing.x * 1,
                    (ImGui::GetTextLineHeight() - saveIconHeight) / 2);
                ImGui::GetWindowDrawList()->AddText(
                    ImVec2(iconPanelPos.x - textSize.x - ImGui::GetStyle().ItemInnerSpacing.x, start.y),
                    IM_COL32(255, 255, 255, 255), id.c_str(), nullptr);

                auto drawList = ImGui::GetWindowDrawList();
                ImGui::SetCursorScreenPos(iconPanelPos);
# if IMGUI_VERSION_NUM < 18967
                ImGui::SetItemAllowOverlap();
# else
                ImGui::SetNextItemAllowOverlap();
# endif
                if (node.SavedState.empty())
                {
                    if (ImGui::InvisibleButton("save", ImVec2((float)saveIconWidth, (float)saveIconHeight)))
                        node.SavedState = node.State;

                    if (ImGui::IsItemActive())
                        drawList->AddImage(m_SaveIcon, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 96));
                    else if (ImGui::IsItemHovered())
                        drawList->AddImage(m_SaveIcon, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 255));
                    else
                        drawList->AddImage(m_SaveIcon, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 160));
                }
                else
                {
                    ImGui::Dummy(ImVec2((float)saveIconWidth, (float)saveIconHeight));
                    drawList->AddImage(m_SaveIcon, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 32));
                }

                ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
# if IMGUI_VERSION_NUM < 18967
                ImGui::SetItemAllowOverlap();
# else
                ImGui::SetNextItemAllowOverlap();
# endif
                if (!node.SavedState.empty())
                {
                    if (ImGui::InvisibleButton("restore", ImVec2((float)restoreIconWidth, (float)restoreIconHeight)))
                    {
                        node.State = node.SavedState;
                        NodeEd::RestoreNodeState(node.ID);
                        node.SavedState.clear();
                    }

                    if (ImGui::IsItemActive())
                        drawList->AddImage(m_RestoreIcon, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 96));
                    else if (ImGui::IsItemHovered())
                        drawList->AddImage(m_RestoreIcon, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 255));
                    else
                        drawList->AddImage(m_RestoreIcon, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 160));
                }
                else
                {
                    ImGui::Dummy(ImVec2((float)restoreIconWidth, (float)restoreIconHeight));
                    drawList->AddImage(m_RestoreIcon, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 32));
                }

                ImGui::SameLine(0, 0);
# if IMGUI_VERSION_NUM < 18967
                ImGui::SetItemAllowOverlap();
# endif
                ImGui::Dummy(ImVec2(0, (float)restoreIconHeight));

                ImGui::PopID();
            }
            ImGui::Unindent();

            static int changeCount = 0;

            ImGui::GetWindowDrawList()->AddRectFilled(
                ImGui::GetCursorScreenPos(),
                ImGui::GetCursorScreenPos() + ImVec2(paneWidth, ImGui::GetTextLineHeight()),
                ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]), ImGui::GetTextLineHeight() * 0.25f);
            ImGui::Spacing(); ImGui::SameLine();
            ImGui::TextUnformatted("Selection");

            ImGui::BeginHorizontal("Selection Stats", ImVec2(paneWidth, 0));
            ImGui::Text("Changed %d time%s", changeCount, changeCount > 1 ? "s" : "");
            ImGui::Spring();
            if (ImGui::Button("Deselect All"))
                NodeEd::ClearSelection();
            ImGui::EndHorizontal();
            ImGui::Indent();
            for (int i = 0; i < nodeCount; ++i) ImGui::Text("Node (%p)", selectedNodes[i].AsPointer());
            for (int i = 0; i < linkCount; ++i) ImGui::Text("Link (%p)", selectedLinks[i].AsPointer());
            ImGui::Unindent();

            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Z)))
                for (auto& link : m_Links)
                    NodeEd::Flow(link.ID);

            if (NodeEd::HasSelectionChanged())
                ++changeCount;

            ImGui::EndChild();
        }

        void OnFrame(float deltaTime) override
        {
            UpdateTouch();

            auto& io = ImGui::GetIO();

            ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);

            NodeEd::SetCurrentEditor(m_Editor);

            //auto& style = ImGui::GetStyle();

# if 0
            {
                for (auto x = -io.DisplaySize.y; x < io.DisplaySize.x; x += 10.0f)
                {
                    ImGui::GetWindowDrawList()->AddLine(ImVec2(x, 0), ImVec2(x + io.DisplaySize.y, io.DisplaySize.y),
                        IM_COL32(255, 255, 0, 255));
                }
            }
# endif

            static NodeEd::NodeId contextNodeId = 0;
            static NodeEd::LinkId contextLinkId = 0;
            static NodeEd::PinId  contextPinId = 0;
            static bool createNewNode = false;
            static Pin* newNodeLinkPin = nullptr;
            static Pin* newLinkPin = nullptr;

            static float leftPaneWidth = 400.0f;
            static float rightPaneWidth = 800.0f;
            Splitter(true, 4.0f, &leftPaneWidth, &rightPaneWidth, 50.0f, 50.0f);

            ShowLeftPane(leftPaneWidth - 4.0f);

            ImGui::SameLine(0.0f, 12.0f);

            NodeEd::Begin("Node editor");
            {
                auto cursorTopLeft = ImGui::GetCursorScreenPos();

                util::BlueprintNodeBuilder builder(m_HeaderBackground, GetTextureWidth(m_HeaderBackground), GetTextureHeight(m_HeaderBackground));

                for (auto& node : m_Nodes)
                {
                    if (node.Type != NodeType::Blueprint && node.Type != NodeType::Simple)
                        continue;

                    const auto isSimple = node.Type == NodeType::Simple;

                    bool hasOutputDelegates = false;
                    for (auto& output : node.Outputs)
                        if (output.Type == PinType::Delegate)
                            hasOutputDelegates = true;

                    builder.Begin(node.ID);
                    if (!isSimple)
                    {
                        builder.Header(node.Color);
                        ImGui::Spring(0);
                        ImGui::TextUnformatted(node.Name.c_str());
                        ImGui::Spring(1);
                        ImGui::Dummy(ImVec2(0, 28));
                        if (hasOutputDelegates)
                        {
                            ImGui::BeginVertical("delegates", ImVec2(0, 28));
                            ImGui::Spring(1, 0);
                            for (auto& output : node.Outputs)
                            {
                                if (output.Type != PinType::Delegate)
                                    continue;

                                auto alpha = ImGui::GetStyle().Alpha;
                                if (newLinkPin && !CanCreateLink(newLinkPin, &output) && &output != newLinkPin)
                                    alpha = alpha * (48.0f / 255.0f);

                                NodeEd::BeginPin(output.ID, NodeEd::PinKind::Output);
                                NodeEd::PinPivotAlignment(ImVec2(1.0f, 0.5f));
                                NodeEd::PinPivotSize(ImVec2(0, 0));
                                ImGui::BeginHorizontal(output.ID.AsPointer());
                                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
                                if (!output.Name.empty())
                                {
                                    ImGui::TextUnformatted(output.Name.c_str());
                                    ImGui::Spring(0);
                                }
                                DrawPinIcon(output, IsPinLinked(output.ID), (int)(alpha * 255));
                                ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.x / 2);
                                ImGui::EndHorizontal();
                                ImGui::PopStyleVar();
                                NodeEd::EndPin();

                                //DrawItemRect(ImColor(255, 0, 0));
                            }
                            ImGui::Spring(1, 0);
                            ImGui::EndVertical();
                            ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.x / 2);
                        }
                        else
                            ImGui::Spring(0);
                        builder.EndHeader();
                    }

                    for (auto& input : node.Inputs)
                    {
                        auto alpha = ImGui::GetStyle().Alpha;
                        if (newLinkPin && !CanCreateLink(newLinkPin, &input) && &input != newLinkPin)
                            alpha = alpha * (48.0f / 255.0f);

                        builder.Input(input.ID);
                        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
                        DrawPinIcon(input, IsPinLinked(input.ID), (int)(alpha * 255));
                        ImGui::Spring(0);
                        if (!input.Name.empty())
                        {
                            ImGui::TextUnformatted(input.Name.c_str());
                            ImGui::Spring(0);
                        }
                        if (input.Type == PinType::Bool)
                        {
                            ImGui::Button("Hello");
                            ImGui::Spring(0);
                        }
                        ImGui::PopStyleVar();
                        builder.EndInput();
                    }

                    if (isSimple)
                    {
                        builder.Middle();

                        ImGui::Spring(1, 0);
                        ImGui::TextUnformatted(node.Name.c_str());
                        ImGui::Spring(1, 0);
                    }

                    for (auto& output : node.Outputs)
                    {
                        if (!isSimple && output.Type == PinType::Delegate)
                            continue;

                        auto alpha = ImGui::GetStyle().Alpha;
                        if (newLinkPin && !CanCreateLink(newLinkPin, &output) && &output != newLinkPin)
                            alpha = alpha * (48.0f / 255.0f);

                        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
                        builder.Output(output.ID);
                        if (output.Type == PinType::String)
                        {
                            static char buffer[128] = "Edit Me\nMultiline!";
                            static bool wasActive = false;

                            ImGui::PushItemWidth(100.0f);
                            ImGui::InputText("##edit", buffer, 127);
                            ImGui::PopItemWidth();
                            if (ImGui::IsItemActive() && !wasActive)
                            {
                                NodeEd::EnableShortcuts(false);
                                wasActive = true;
                            }
                            else if (!ImGui::IsItemActive() && wasActive)
                            {
                                NodeEd::EnableShortcuts(true);
                                wasActive = false;
                            }
                            ImGui::Spring(0);
                        }
                        if (!output.Name.empty())
                        {
                            ImGui::Spring(0);
                            ImGui::TextUnformatted(output.Name.c_str());
                        }
                        ImGui::Spring(0);
                        DrawPinIcon(output, IsPinLinked(output.ID), (int)(alpha * 255));
                        ImGui::PopStyleVar();
                        builder.EndOutput();
                    }

                    builder.End();
                }

                for (auto& node : m_Nodes)
                {
                    if (node.Type != NodeType::Tree)
                        continue;

                    const float rounding = 5.0f;
                    const float padding = 12.0f;

                    const auto pinBackground = NodeEd::GetStyle().Colors[NodeEd::StyleColor_NodeBg];

                    NodeEd::PushStyleColor(NodeEd::StyleColor_NodeBg, ImColor(128, 128, 128, 200));
                    NodeEd::PushStyleColor(NodeEd::StyleColor_NodeBorder, ImColor(32, 32, 32, 200));
                    NodeEd::PushStyleColor(NodeEd::StyleColor_PinRect, ImColor(60, 180, 255, 150));
                    NodeEd::PushStyleColor(NodeEd::StyleColor_PinRectBorder, ImColor(60, 180, 255, 150));

                    NodeEd::PushStyleVar(NodeEd::StyleVar_NodePadding, ImVec4(0, 0, 0, 0));
                    NodeEd::PushStyleVar(NodeEd::StyleVar_NodeRounding, rounding);
                    NodeEd::PushStyleVar(NodeEd::StyleVar_SourceDirection, ImVec2(0.0f, 1.0f));
                    NodeEd::PushStyleVar(NodeEd::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
                    NodeEd::PushStyleVar(NodeEd::StyleVar_LinkStrength, 0.0f);
                    NodeEd::PushStyleVar(NodeEd::StyleVar_PinBorderWidth, 1.0f);
                    NodeEd::PushStyleVar(NodeEd::StyleVar_PinRadius, 5.0f);
                    NodeEd::BeginNode(node.ID);

                    ImGui::BeginVertical(node.ID.AsPointer());
                    ImGui::BeginHorizontal("inputs");
                    ImGui::Spring(0, padding * 2);

                    ImRect inputsRect;
                    int inputAlpha = 200;
                    if (!node.Inputs.empty())
                    {
                        auto& pin = node.Inputs[0];
                        ImGui::Dummy(ImVec2(0, padding));
                        ImGui::Spring(1, 0);
                        inputsRect = ImGui_GetItemRect();

                        NodeEd::PushStyleVar(NodeEd::StyleVar_PinArrowSize, 10.0f);
                        NodeEd::PushStyleVar(NodeEd::StyleVar_PinArrowWidth, 10.0f);
#if IMGUI_VERSION_NUM > 18101
                        NodeEd::PushStyleVar(NodeEd::StyleVar_PinCorners, ImDrawFlags_RoundCornersBottom);
#else
                        NodeEd::PushStyleVar(NodeEd::StyleVar_PinCorners, 12);
#endif
                        NodeEd::BeginPin(pin.ID, NodeEd::PinKind::Input);
                        NodeEd::PinPivotRect(inputsRect.GetTL(), inputsRect.GetBR());
                        NodeEd::PinRect(inputsRect.GetTL(), inputsRect.GetBR());
                        NodeEd::EndPin();
                        NodeEd::PopStyleVar(3);

                        if (newLinkPin && !CanCreateLink(newLinkPin, &pin) && &pin != newLinkPin)
                            inputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
                    }
                    else
                        ImGui::Dummy(ImVec2(0, padding));

                    ImGui::Spring(0, padding * 2);
                    ImGui::EndHorizontal();

                    ImGui::BeginHorizontal("content_frame");
                    ImGui::Spring(1, padding);

                    ImGui::BeginVertical("content", ImVec2(0.0f, 0.0f));
                    ImGui::Dummy(ImVec2(160, 0));
                    ImGui::Spring(1);
                    ImGui::TextUnformatted(node.Name.c_str());
                    ImGui::Spring(1);
                    ImGui::EndVertical();
                    auto contentRect = ImGui_GetItemRect();

                    ImGui::Spring(1, padding);
                    ImGui::EndHorizontal();

                    ImGui::BeginHorizontal("outputs");
                    ImGui::Spring(0, padding * 2);

                    ImRect outputsRect;
                    int outputAlpha = 200;
                    if (!node.Outputs.empty())
                    {
                        auto& pin = node.Outputs[0];
                        ImGui::Dummy(ImVec2(0, padding));
                        ImGui::Spring(1, 0);
                        outputsRect = ImGui_GetItemRect();

#if IMGUI_VERSION_NUM > 18101
                        NodeEd::PushStyleVar(NodeEd::StyleVar_PinCorners, ImDrawFlags_RoundCornersTop);
#else
                        NodeEd::PushStyleVar(NodeEd::StyleVar_PinCorners, 3);
#endif
                        NodeEd::BeginPin(pin.ID, NodeEd::PinKind::Output);
                        NodeEd::PinPivotRect(outputsRect.GetTL(), outputsRect.GetBR());
                        NodeEd::PinRect(outputsRect.GetTL(), outputsRect.GetBR());
                        NodeEd::EndPin();
                        NodeEd::PopStyleVar();

                        if (newLinkPin && !CanCreateLink(newLinkPin, &pin) && &pin != newLinkPin)
                            outputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
                    }
                    else
                        ImGui::Dummy(ImVec2(0, padding));

                    ImGui::Spring(0, padding * 2);
                    ImGui::EndHorizontal();

                    ImGui::EndVertical();

                    NodeEd::EndNode();
                    NodeEd::PopStyleVar(7);
                    NodeEd::PopStyleColor(4);

                    auto drawList = NodeEd::GetNodeBackgroundDrawList(node.ID);

                    //const auto fringeScale = ImGui::GetStyle().AntiAliasFringeScale;
                    //const auto unitSize    = 1.0f / fringeScale;

                    //const auto ImDrawList_AddRect = [](ImDrawList* drawList, const ImVec2& a, const ImVec2& b, ImU32 col, float rounding, int rounding_corners, float thickness)
                    //{
                    //    if ((col >> 24) == 0)
                    //        return;
                    //    drawList->PathRect(a, b, rounding, rounding_corners);
                    //    drawList->PathStroke(col, true, thickness);
                    //};

#if IMGUI_VERSION_NUM > 18101
                    const auto    topRoundCornersFlags = ImDrawFlags_RoundCornersTop;
                    const auto bottomRoundCornersFlags = ImDrawFlags_RoundCornersBottom;
#else
                    const auto    topRoundCornersFlags = 1 | 2;
                    const auto bottomRoundCornersFlags = 4 | 8;
#endif

                    drawList->AddRectFilled(inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
                        IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, bottomRoundCornersFlags);
                    //ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
                    drawList->AddRect(inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
                        IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, bottomRoundCornersFlags);
                    //ImGui::PopStyleVar();
                    drawList->AddRectFilled(outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
                        IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, topRoundCornersFlags);
                    //ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
                    drawList->AddRect(outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
                        IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, topRoundCornersFlags);
                    //ImGui::PopStyleVar();
                    drawList->AddRectFilled(contentRect.GetTL(), contentRect.GetBR(), IM_COL32(24, 64, 128, 200), 0.0f);
                    //ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
                    drawList->AddRect(
                        contentRect.GetTL(),
                        contentRect.GetBR(),
                        IM_COL32(48, 128, 255, 100), 0.0f);
                    //ImGui::PopStyleVar();
                }

                for (auto& node : m_Nodes)
                {
                    if (node.Type != NodeType::Houdini)
                        continue;

                    const float rounding = 10.0f;
                    const float padding = 12.0f;


                    NodeEd::PushStyleColor(NodeEd::StyleColor_NodeBg, ImColor(229, 229, 229, 200));
                    NodeEd::PushStyleColor(NodeEd::StyleColor_NodeBorder, ImColor(125, 125, 125, 200));
                    NodeEd::PushStyleColor(NodeEd::StyleColor_PinRect, ImColor(229, 229, 229, 60));
                    NodeEd::PushStyleColor(NodeEd::StyleColor_PinRectBorder, ImColor(125, 125, 125, 60));

                    const auto pinBackground = NodeEd::GetStyle().Colors[NodeEd::StyleColor_NodeBg];

                    NodeEd::PushStyleVar(NodeEd::StyleVar_NodePadding, ImVec4(0, 0, 0, 0));
                    NodeEd::PushStyleVar(NodeEd::StyleVar_NodeRounding, rounding);
                    NodeEd::PushStyleVar(NodeEd::StyleVar_SourceDirection, ImVec2(0.0f, 1.0f));
                    NodeEd::PushStyleVar(NodeEd::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
                    NodeEd::PushStyleVar(NodeEd::StyleVar_LinkStrength, 0.0f);
                    NodeEd::PushStyleVar(NodeEd::StyleVar_PinBorderWidth, 1.0f);
                    NodeEd::PushStyleVar(NodeEd::StyleVar_PinRadius, 6.0f);
                    NodeEd::BeginNode(node.ID);

                    ImGui::BeginVertical(node.ID.AsPointer());
                    if (!node.Inputs.empty())
                    {
                        ImGui::BeginHorizontal("inputs");
                        ImGui::Spring(1, 0);

                        ImRect inputsRect;
                        int inputAlpha = 200;
                        for (auto& pin : node.Inputs)
                        {
                            ImGui::Dummy(ImVec2(padding, padding));
                            inputsRect = ImGui_GetItemRect();
                            ImGui::Spring(1, 0);
                            inputsRect.Min.y -= padding;
                            inputsRect.Max.y -= padding;

#if IMGUI_VERSION_NUM > 18101
                            const auto allRoundCornersFlags = ImDrawFlags_RoundCornersAll;
#else
                            const auto allRoundCornersFlags = 15;
#endif
                            //NodeEd::PushStyleVar(NodeEd::StyleVar_PinArrowSize, 10.0f);
                            //NodeEd::PushStyleVar(NodeEd::StyleVar_PinArrowWidth, 10.0f);
                            NodeEd::PushStyleVar(NodeEd::StyleVar_PinCorners, allRoundCornersFlags);

                            NodeEd::BeginPin(pin.ID, NodeEd::PinKind::Input);
                            NodeEd::PinPivotRect(inputsRect.GetCenter(), inputsRect.GetCenter());
                            NodeEd::PinRect(inputsRect.GetTL(), inputsRect.GetBR());
                            NodeEd::EndPin();
                            //NodeEd::PopStyleVar(3);
                            NodeEd::PopStyleVar(1);

                            auto drawList = ImGui::GetWindowDrawList();
                            drawList->AddRectFilled(inputsRect.GetTL(), inputsRect.GetBR(),
                                IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, allRoundCornersFlags);
                            drawList->AddRect(inputsRect.GetTL(), inputsRect.GetBR(),
                                IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, allRoundCornersFlags);

                            if (newLinkPin && !CanCreateLink(newLinkPin, &pin) && &pin != newLinkPin)
                                inputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
                        }

                        //ImGui::Spring(1, 0);
                        ImGui::EndHorizontal();
                    }

                    ImGui::BeginHorizontal("content_frame");
                    ImGui::Spring(1, padding);

                    ImGui::BeginVertical("content", ImVec2(0.0f, 0.0f));
                    ImGui::Dummy(ImVec2(160, 0));
                    ImGui::Spring(1);
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
                    ImGui::TextUnformatted(node.Name.c_str());
                    ImGui::PopStyleColor();
                    ImGui::Spring(1);
                    ImGui::EndVertical();
                    auto contentRect = ImGui_GetItemRect();

                    ImGui::Spring(1, padding);
                    ImGui::EndHorizontal();

                    if (!node.Outputs.empty())
                    {
                        ImGui::BeginHorizontal("outputs");
                        ImGui::Spring(1, 0);

                        ImRect outputsRect;
                        int outputAlpha = 200;
                        for (auto& pin : node.Outputs)
                        {
                            ImGui::Dummy(ImVec2(padding, padding));
                            outputsRect = ImGui_GetItemRect();
                            ImGui::Spring(1, 0);
                            outputsRect.Min.y += padding;
                            outputsRect.Max.y += padding;

#if IMGUI_VERSION_NUM > 18101
                            const auto allRoundCornersFlags = ImDrawFlags_RoundCornersAll;
                            const auto topRoundCornersFlags = ImDrawFlags_RoundCornersTop;
#else
                            const auto allRoundCornersFlags = 15;
                            const auto topRoundCornersFlags = 3;
#endif

                            NodeEd::PushStyleVar(NodeEd::StyleVar_PinCorners, topRoundCornersFlags);
                            NodeEd::BeginPin(pin.ID, NodeEd::PinKind::Output);
                            NodeEd::PinPivotRect(outputsRect.GetCenter(), outputsRect.GetCenter());
                            NodeEd::PinRect(outputsRect.GetTL(), outputsRect.GetBR());
                            NodeEd::EndPin();
                            NodeEd::PopStyleVar();


                            auto drawList = ImGui::GetWindowDrawList();
                            drawList->AddRectFilled(outputsRect.GetTL(), outputsRect.GetBR(),
                                IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, allRoundCornersFlags);
                            drawList->AddRect(outputsRect.GetTL(), outputsRect.GetBR(),
                                IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, allRoundCornersFlags);


                            if (newLinkPin && !CanCreateLink(newLinkPin, &pin) && &pin != newLinkPin)
                                outputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
                        }

                        ImGui::EndHorizontal();
                    }

                    ImGui::EndVertical();

                    NodeEd::EndNode();
                    NodeEd::PopStyleVar(7);
                    NodeEd::PopStyleColor(4);

                    // auto drawList = NodeEd::GetNodeBackgroundDrawList(node.ID);

                    //const auto fringeScale = ImGui::GetStyle().AntiAliasFringeScale;
                    //const auto unitSize    = 1.0f / fringeScale;

                    //const auto ImDrawList_AddRect = [](ImDrawList* drawList, const ImVec2& a, const ImVec2& b, ImU32 col, float rounding, int rounding_corners, float thickness)
                    //{
                    //    if ((col >> 24) == 0)
                    //        return;
                    //    drawList->PathRect(a, b, rounding, rounding_corners);
                    //    drawList->PathStroke(col, true, thickness);
                    //};

                    //drawList->AddRectFilled(inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
                    //    IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, 12);
                    //ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
                    //drawList->AddRect(inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
                    //    IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, 12);
                    //ImGui::PopStyleVar();
                    //drawList->AddRectFilled(outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
                    //    IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, 3);
                    ////ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
                    //drawList->AddRect(outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
                    //    IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, 3);
                    ////ImGui::PopStyleVar();
                    //drawList->AddRectFilled(contentRect.GetTL(), contentRect.GetBR(), IM_COL32(24, 64, 128, 200), 0.0f);
                    //ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
                    //drawList->AddRect(
                    //    contentRect.GetTL(),
                    //    contentRect.GetBR(),
                    //    IM_COL32(48, 128, 255, 100), 0.0f);
                    //ImGui::PopStyleVar();
                }

                for (auto& node : m_Nodes)
                {
                    if (node.Type != NodeType::Comment)
                        continue;

                    const float commentAlpha = 0.75f;

                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha);
                    NodeEd::PushStyleColor(NodeEd::StyleColor_NodeBg, ImColor(255, 255, 255, 64));
                    NodeEd::PushStyleColor(NodeEd::StyleColor_NodeBorder, ImColor(255, 255, 255, 64));
                    NodeEd::BeginNode(node.ID);
                    ImGui::PushID(node.ID.AsPointer());
                    ImGui::BeginVertical("content");
                    ImGui::BeginHorizontal("horizontal");
                    ImGui::Spring(1);
                    ImGui::TextUnformatted(node.Name.c_str());
                    ImGui::Spring(1);
                    ImGui::EndHorizontal();
                    NodeEd::Group(node.Size);
                    ImGui::EndVertical();
                    ImGui::PopID();
                    NodeEd::EndNode();
                    NodeEd::PopStyleColor(2);
                    ImGui::PopStyleVar();

                    if (NodeEd::BeginGroupHint(node.ID))
                    {
                        //auto alpha   = static_cast<int>(commentAlpha * ImGui::GetStyle().Alpha * 255);
                        auto bgAlpha = static_cast<int>(ImGui::GetStyle().Alpha * 255);

                        //ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha * ImGui::GetStyle().Alpha);

                        auto min = NodeEd::GetGroupMin();
                        //auto max = NodeEd::GetGroupMax();

                        ImGui::SetCursorScreenPos(min - ImVec2(-8, ImGui::GetTextLineHeightWithSpacing() + 4));
                        ImGui::BeginGroup();
                        ImGui::TextUnformatted(node.Name.c_str());
                        ImGui::EndGroup();

                        auto drawList = NodeEd::GetHintBackgroundDrawList();

                        auto hintBounds = ImGui_GetItemRect();
                        auto hintFrameBounds = ImRect_Expanded(hintBounds, 8, 4);

                        drawList->AddRectFilled(
                            hintFrameBounds.GetTL(),
                            hintFrameBounds.GetBR(),
                            IM_COL32(255, 255, 255, 64 * bgAlpha / 255), 4.0f);

                        drawList->AddRect(
                            hintFrameBounds.GetTL(),
                            hintFrameBounds.GetBR(),
                            IM_COL32(255, 255, 255, 128 * bgAlpha / 255), 4.0f);

                        //ImGui::PopStyleVar();
                    }
                    NodeEd::EndGroupHint();
                }

                for (auto& link : m_Links)
                    NodeEd::Link(link.ID, link.StartPinID, link.EndPinID, link.Color, 2.0f);

                if (!createNewNode)
                {
                    if (NodeEd::BeginCreate(ImColor(255, 255, 255), 2.0f))
                    {
                        auto showLabel = [](const char* label, ImColor color)
                            {
                                ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
                                auto size = ImGui::CalcTextSize(label);

                                auto padding = ImGui::GetStyle().FramePadding;
                                auto spacing = ImGui::GetStyle().ItemSpacing;

                                ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

                                auto rectMin = ImGui::GetCursorScreenPos() - padding;
                                auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

                                auto drawList = ImGui::GetWindowDrawList();
                                drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
                                ImGui::TextUnformatted(label);
                            };

                        NodeEd::PinId startPinId = 0, endPinId = 0;
                        if (NodeEd::QueryNewLink(&startPinId, &endPinId))
                        {
                            auto startPin = FindPin(startPinId);
                            auto endPin = FindPin(endPinId);

                            newLinkPin = startPin ? startPin : endPin;

                            if (startPin->Kind == PinKind::Input)
                            {
                                std::swap(startPin, endPin);
                                std::swap(startPinId, endPinId);
                            }

                            if (startPin && endPin)
                            {
                                if (endPin == startPin)
                                {
                                    NodeEd::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                                }
                                else if (endPin->Kind == startPin->Kind)
                                {
                                    showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
                                    NodeEd::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                                }
                                //else if (endPin->Node == startPin->Node)
                                //{
                                //    showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
                                //    NodeEd::RejectNewItem(ImColor(255, 0, 0), 1.0f);
                                //}
                                else if (endPin->Type != startPin->Type)
                                {
                                    showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
                                    NodeEd::RejectNewItem(ImColor(255, 128, 128), 1.0f);
                                }
                                else
                                {
                                    showLabel("+ Create Link", ImColor(32, 45, 32, 180));
                                    if (NodeEd::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
                                    {
                                        m_Links.emplace_back(Link(GetNextId(), startPinId, endPinId));
                                        m_Links.back().Color = GetIconColor(startPin->Type);
                                    }
                                }
                            }
                        }

                        NodeEd::PinId pinId = 0;
                        if (NodeEd::QueryNewNode(&pinId))
                        {
                            newLinkPin = FindPin(pinId);
                            if (newLinkPin)
                                showLabel("+ Create Node", ImColor(32, 45, 32, 180));

                            if (NodeEd::AcceptNewItem())
                            {
                                createNewNode = true;
                                newNodeLinkPin = FindPin(pinId);
                                newLinkPin = nullptr;
                                NodeEd::Suspend();
                                ImGui::OpenPopup("Create New Node");
                                NodeEd::Resume();
                            }
                        }
                    }
                    else
                        newLinkPin = nullptr;

                    NodeEd::EndCreate();

                    if (NodeEd::BeginDelete())
                    {
                        NodeEd::NodeId nodeId = 0;
                        while (NodeEd::QueryDeletedNode(&nodeId))
                        {
                            if (NodeEd::AcceptDeletedItem())
                            {
                                auto id = std::find_if(m_Nodes.begin(), m_Nodes.end(), [nodeId](auto& node) { return node.ID == nodeId; });
                                if (id != m_Nodes.end())
                                    m_Nodes.erase(id);
                            }
                        }

                        NodeEd::LinkId linkId = 0;
                        while (NodeEd::QueryDeletedLink(&linkId))
                        {
                            if (NodeEd::AcceptDeletedItem())
                            {
                                auto id = std::find_if(m_Links.begin(), m_Links.end(), [linkId](auto& link) { return link.ID == linkId; });
                                if (id != m_Links.end())
                                    m_Links.erase(id);
                            }
                        }
                    }
                    NodeEd::EndDelete();
                }

                ImGui::SetCursorScreenPos(cursorTopLeft);
            }

# if 1
            auto openPopupPosition = ImGui::GetMousePos();
            NodeEd::Suspend();
            if (NodeEd::ShowNodeContextMenu(&contextNodeId))
                ImGui::OpenPopup("Node Context Menu");
            else if (NodeEd::ShowPinContextMenu(&contextPinId))
                ImGui::OpenPopup("Pin Context Menu");
            else if (NodeEd::ShowLinkContextMenu(&contextLinkId))
                ImGui::OpenPopup("Link Context Menu");
            else if (NodeEd::ShowBackgroundContextMenu())
            {
                ImGui::OpenPopup("Create New Node");
                newNodeLinkPin = nullptr;
            }
            NodeEd::Resume();

            NodeEd::Suspend();
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
            if (ImGui::BeginPopup("Node Context Menu"))
            {
                auto node = FindNode(contextNodeId);

                ImGui::TextUnformatted("Node Context Menu");
                ImGui::Separator();
                if (node)
                {
                    ImGui::Text("ID: %p", node->ID.AsPointer());
                    ImGui::Text("Type: %s", node->Type == NodeType::Blueprint ? "Blueprint" : (node->Type == NodeType::Tree ? "Tree" : "Comment"));
                    ImGui::Text("Inputs: %d", (int)node->Inputs.size());
                    ImGui::Text("Outputs: %d", (int)node->Outputs.size());
                }
                else
                    ImGui::Text("Unknown node: %p", contextNodeId.AsPointer());
                ImGui::Separator();
                if (ImGui::MenuItem("Delete"))
                    NodeEd::DeleteNode(contextNodeId);
                ImGui::EndPopup();
            }

            if (ImGui::BeginPopup("Pin Context Menu"))
            {
                auto pin = FindPin(contextPinId);

                ImGui::TextUnformatted("Pin Context Menu");
                ImGui::Separator();
                if (pin)
                {
                    ImGui::Text("ID: %p", pin->ID.AsPointer());
                    if (pin->Node)
                        ImGui::Text("Node: %p", pin->Node->ID.AsPointer());
                    else
                        ImGui::Text("Node: %s", "<none>");
                }
                else
                    ImGui::Text("Unknown pin: %p", contextPinId.AsPointer());

                ImGui::EndPopup();
            }

            if (ImGui::BeginPopup("Link Context Menu"))
            {
                auto link = FindLink(contextLinkId);

                ImGui::TextUnformatted("Link Context Menu");
                ImGui::Separator();
                if (link)
                {
                    ImGui::Text("ID: %p", link->ID.AsPointer());
                    ImGui::Text("From: %p", link->StartPinID.AsPointer());
                    ImGui::Text("To: %p", link->EndPinID.AsPointer());
                }
                else
                    ImGui::Text("Unknown link: %p", contextLinkId.AsPointer());
                ImGui::Separator();
                if (ImGui::MenuItem("Delete"))
                    NodeEd::DeleteLink(contextLinkId);
                ImGui::EndPopup();
            }

            if (ImGui::BeginPopup("Create New Node"))
            {
                auto newNodePostion = openPopupPosition;
                //ImGui::SetCursorScreenPos(ImGui::GetMousePosOnOpeningCurrentPopup());

                //auto drawList = ImGui::GetWindowDrawList();
                //drawList->AddCircleFilled(ImGui::GetMousePosOnOpeningCurrentPopup(), 10.0f, 0xFFFF00FF);

                Node* node = nullptr;
                if (ImGui::MenuItem("Input Action"))
                    node = SpawnInputActionNode();
                if (ImGui::MenuItem("Output Action"))
                    node = SpawnOutputActionNode();
                if (ImGui::MenuItem("Branch"))
                    node = SpawnBranchNode();
                if (ImGui::MenuItem("Do N"))
                    node = SpawnDoNNode();
                if (ImGui::MenuItem("Set Timer"))
                    node = SpawnSetTimerNode();
                if (ImGui::MenuItem("Less"))
                    node = SpawnLessNode();
                if (ImGui::MenuItem("Weird"))
                    node = SpawnWeirdNode();
                if (ImGui::MenuItem("Trace by Channel"))
                    node = SpawnTraceByChannelNode();
                if (ImGui::MenuItem("Print String"))
                    node = SpawnPrintStringNode();
                ImGui::Separator();
                if (ImGui::MenuItem("Comment"))
                    node = SpawnComment();
                ImGui::Separator();
                if (ImGui::MenuItem("Sequence"))
                    node = SpawnTreeSequenceNode();
                if (ImGui::MenuItem("Move To"))
                    node = SpawnTreeTaskNode();
                if (ImGui::MenuItem("Random Wait"))
                    node = SpawnTreeTask2Node();
                ImGui::Separator();
                if (ImGui::MenuItem("Message"))
                    node = SpawnMessageNode();
                ImGui::Separator();
                if (ImGui::MenuItem("Transform"))
                    node = SpawnHoudiniTransformNode();
                if (ImGui::MenuItem("Group"))
                    node = SpawnHoudiniGroupNode();

                if (node)
                {
                    BuildNodes();

                    createNewNode = false;

                    NodeEd::SetNodePosition(node->ID, newNodePostion);

                    if (auto startPin = newNodeLinkPin)
                    {
                        auto& pins = startPin->Kind == PinKind::Input ? node->Outputs : node->Inputs;

                        for (auto& pin : pins)
                        {
                            if (CanCreateLink(startPin, &pin))
                            {
                                auto endPin = &pin;
                                if (startPin->Kind == PinKind::Input)
                                    std::swap(startPin, endPin);

                                m_Links.emplace_back(Link(GetNextId(), startPin->ID, endPin->ID));
                                m_Links.back().Color = GetIconColor(startPin->Type);

                                break;
                            }
                        }
                    }
                }

                ImGui::EndPopup();
            }
            else
                createNewNode = false;
            ImGui::PopStyleVar();
            NodeEd::Resume();
# endif


            /*
                cubic_bezier_t c;
                c.p0 = pointf(100, 600);
                c.p1 = pointf(300, 1200);
                c.p2 = pointf(500, 100);
                c.p3 = pointf(900, 600);

                auto drawList = ImGui::GetWindowDrawList();
                auto offset_radius = 15.0f;
                auto acceptPoint = [drawList, offset_radius](const bezier_subdivide_result_t& r)
                {
                    drawList->AddCircle(to_imvec(r.point), 4.0f, IM_COL32(255, 0, 255, 255));

                    auto nt = r.tangent.normalized();
                    nt = pointf(-nt.y, nt.x);

                    drawList->AddLine(to_imvec(r.point), to_imvec(r.point + nt * offset_radius), IM_COL32(255, 0, 0, 255), 1.0f);
                };

                drawList->AddBezierCurve(to_imvec(c.p0), to_imvec(c.p1), to_imvec(c.p2), to_imvec(c.p3), IM_COL32(255, 255, 255, 255), 1.0f);
                cubic_bezier_subdivide(acceptPoint, c);
            */

            NodeEd::End();

            auto editorMin = ImGui::GetItemRectMin();
            auto editorMax = ImGui::GetItemRectMax();

            if (m_ShowOrdinals)
            {
                int nodeCount = NodeEd::GetNodeCount();
                std::vector<NodeEd::NodeId> orderedNodeIds;
                orderedNodeIds.resize(static_cast<size_t>(nodeCount));
                NodeEd::GetOrderedNodeIds(orderedNodeIds.data(), nodeCount);


                auto drawList = ImGui::GetWindowDrawList();
                drawList->PushClipRect(editorMin, editorMax);

                int ordinal = 0;
                for (auto& nodeId : orderedNodeIds)
                {
                    auto p0 = NodeEd::GetNodePosition(nodeId);
                    auto p1 = p0 + NodeEd::GetNodeSize(nodeId);
                    p0 = NodeEd::CanvasToScreen(p0);
                    p1 = NodeEd::CanvasToScreen(p1);


                    ImGuiTextBuffer builder;
                    builder.appendf("#%d", ordinal++);

                    auto textSize = ImGui::CalcTextSize(builder.c_str());
                    auto padding = ImVec2(2.0f, 2.0f);
                    auto widgetSize = textSize + padding * 2;

                    auto widgetPosition = ImVec2(p1.x, p0.y) + ImVec2(0.0f, -widgetSize.y);

                    drawList->AddRectFilled(widgetPosition, widgetPosition + widgetSize, IM_COL32(100, 80, 80, 190), 3.0f, ImDrawFlags_RoundCornersAll);
                    drawList->AddRect(widgetPosition, widgetPosition + widgetSize, IM_COL32(200, 160, 160, 190), 3.0f, ImDrawFlags_RoundCornersAll);
                    drawList->AddText(widgetPosition + padding, IM_COL32(255, 255, 255, 255), builder.c_str());
                }

                drawList->PopClipRect();
            }


            //ImGui::ShowTestWindow();
            //ImGui::ShowMetricsWindow();
        }

        int                  m_NextId = 1;
        const int            m_PinIconSize = 24;
        std::vector<Node>    m_Nodes;
        std::vector<Link>    m_Links;
        ImTextureID          m_HeaderBackground = nullptr;
        ImTextureID          m_SaveIcon = nullptr;
        ImTextureID          m_RestoreIcon = nullptr;
        const float          m_TouchTime = 1.0f;
        std::map<NodeEd::NodeId, float, NodeIdLess> m_NodeTouchTime;
        bool                 m_ShowOrdinals = false;
    };

#endif

}
