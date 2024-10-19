#include "LKpch.h"
#include "NodeEditor.h"
#include "NodeEditorUtils.h"

#include <imgui/imgui_internal.h>


namespace LkEngine {
    
    using namespace UI::Nodes;
    using namespace ax::NodeEditor::Detail;
    using namespace ax::NodeEditor::Details;

    namespace NodeEd = ed;

    LNodeEditor::LNodeEditor(std::string_view InName)
        : Name(InName)
    {
        /// FIXME
        EditorContext = new LNodeEditorContext();
        EditorContext->Initialize();
    }

    void LNodeEditor::OnRender()
    {
    }

    void LNodeEditor::OnImGuiRender(const ImVec2& InWindowSize)
    {
        static const int NodePin = 1;
        static const int InputPin = 1;
        static const int OutputPin = 2;

        NE::Begin(Name.c_str(), InWindowSize);
        {
            NE::BeginNode(NodePin);

            ImGui::Text("Node Editor content");

            NE::BeginPin(InputPin, NE::PinKind::Input);
            ImGui::Text("In -->");
            NE::EndPin();

            NE::BeginPin(OutputPin, NE::PinKind::Output);
            ImGui::Text("Out -->");
            NE::EndPin();

            NE::EndNode();
        }
        NE::End();
    }

    void LNodeEditor::Destroy()
    {
        Save(); // Save before deletion begins
    }

    // Serialize
    void LNodeEditor::Save()
    {
    }

    ImColor LNodeEditor::GetIconColor(const PinType type)
    {
        switch (type)
        {
			case PinType::Flow:       return ImColor(255, 255, 255);
			case PinType::Bool:       return ImColor(220, 48, 48);
			case PinType::Int:        return ImColor(68, 201, 156);
			case PinType::Float:      return ImColor(147, 226, 74);
			case PinType::String:     return ImColor(124, 21, 153);
			case PinType::Object:     return ImColor(51, 150, 215);
			case PinType::Function:   return ImColor(218, 0, 183);
			case PinType::Delegate:   return ImColor(255, 48, 48);
			default:
				break;
        }

        return {};
    };

    void LNodeEditor::DrawPinIcon(const Pin& pin, bool connected, int alpha)
    {
        IconType iconType;
        ImColor  color = GetIconColor(pin.Type);
        color.Value.w = alpha / 255.0f;
        switch (pin.Type)
        {
			case PinType::Flow:        
                iconType = IconType::Flow;
                break;

			case PinType::Bool:        
                iconType = IconType::Circle; 
                break;

			case PinType::Int:         
                iconType = IconType::Circle; 
                break;

			case PinType::Float:       
                iconType = IconType::Circle; 
                break;

			case PinType::String:      
                iconType = IconType::Circle; 
                break;

			case PinType::Object:      
                iconType = IconType::Circle; 
                break;

			case PinType::Function:    
                iconType = IconType::Circle; 
                break;

			case PinType::Delegate:    
                iconType = IconType::Square; 
                break;

			default:
				return;
        }

        // FIXME: 
        Icon(ImVec2(static_cast<float>(m_PinIconSize), static_cast<float>(m_PinIconSize)), 
             iconType, 
             connected, 
             color, 
             ImColor(32, 32, 32, alpha)
        );
    };


    void LNodeEditor::BuildNode(Node* node)
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

    Node* LNodeEditor::SpawnInputActionNode()
    {
        m_Nodes.emplace_back(GetNextID(), "InputAction Fire", ImColor(255, 128, 128));
        m_Nodes.back().Outputs.emplace_back(GetNextID(), "", PinType::Delegate);
        m_Nodes.back().Outputs.emplace_back(GetNextID(), "Pressed", PinType::Flow);
        m_Nodes.back().Outputs.emplace_back(GetNextID(), "Released", PinType::Flow);

        BuildNode(&m_Nodes.back());

        return &m_Nodes.back();
    }

    Node* LNodeEditor::SpawnBranchNode()
    {
        m_Nodes.emplace_back(GetNextID(), "Branch");
        m_Nodes.back().Inputs.emplace_back(GetNextID(), "", PinType::Flow);
        m_Nodes.back().Inputs.emplace_back(GetNextID(), "Condition", PinType::Bool);
        m_Nodes.back().Outputs.emplace_back(GetNextID(), "True", PinType::Flow);
        m_Nodes.back().Outputs.emplace_back(GetNextID(), "False", PinType::Flow);

        BuildNode(&m_Nodes.back());

        return &m_Nodes.back();
    }

    int LNodeEditor::GetNextID()
    {
        return m_NextID++;
    }

    //NE::NodeId GetNextNodeId()
    //{
    //    return NE::NodeId(GetNextID());
    //}

    NE::LinkId LNodeEditor::GetNextLinkID()
    {
        return NE::LinkId(GetNextID());
    }

    void LNodeEditor::TouchNode(NE::NodeId id)
    {
        m_NodeTouchTime[id] = m_TouchTime;
    }

    float LNodeEditor::GetTouchProgress(NE::NodeId id)
    {
        auto it = m_NodeTouchTime.find(id);
        if (it != m_NodeTouchTime.end() && it->second > 0.0f)
            return (m_TouchTime - it->second) / m_TouchTime;
        else
            return 0.0f;
    }

    void LNodeEditor::UpdateTouch()
    {
        const auto deltaTime = ImGui::GetIO().DeltaTime;
        for (auto& entry : m_NodeTouchTime)
        {
            if (entry.second > 0.0f)
                entry.second -= deltaTime;
        }
    }

    Node* LNodeEditor::FindNode(NE::NodeId id)
    {
        for (auto& node : m_Nodes)
            if (node.ID == id)
                return &node;

        return nullptr;
    }

    Link* LNodeEditor::FindLink(NE::LinkId id)
    {
        for (auto& link : m_Links)
            if (link.ID == id)
                return &link;

        return nullptr;
    }

    Pin* LNodeEditor::FindPin(NE::PinId id)
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

    bool LNodeEditor::IsPinLinked(NE::PinId id)
    {
        if (!id)
            return false;

        for (auto& link : m_Links)
            if (link.StartPinID == id || link.EndPinID == id)
                return true;

        return false;
    }

    bool LNodeEditor::CanCreateLink(Pin* a, Pin* b)
    {
        if (!a || !b || a == b || a->Kind == b->Kind || a->Type != b->Type || a->Node == b->Node)
            return false;

        return true;
    }

    void LNodeEditor::IterateNodes()
    {
        for (Node& node : m_Nodes)
        {
            if (node.Type != NodeType::Comment)
            {
                continue;
            }

            const float commentAlpha = 0.75f;

            // TODO: BeginVertical/BeginHorizontal
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha);
            ed::PushStyleColor(ed::StyleColor_NodeBg, ImColor(255, 255, 255, 64));
            ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(255, 255, 255, 64));
            ed::BeginNode(node.ID);
            ImGui::PushID(node.ID.AsPointer());
            //ImGui::BeginVertical("content");
            //ImGui::BeginHorizontal("horizontal");
            //ImGui::Spring(1);
            ImGui::TextUnformatted(node.Name.c_str());
            //ImGui::Spring(1);
            //ImGui::EndHorizontal();
            ed::Group(node.Size);
            //ImGui::EndVertical();
            ImGui::PopID();
            ed::EndNode();
            ed::PopStyleColor(2);
            ImGui::PopStyleVar();

            if (ed::BeginGroupHint(node.ID))
            {
                //auto alpha   = static_cast<int>(commentAlpha * ImGui::GetStyle().Alpha * 255);
                auto bgAlpha = static_cast<int>(ImGui::GetStyle().Alpha * 255);

                //ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha * ImGui::GetStyle().Alpha);

                auto min = ed::GetGroupMin();
                //auto max = ed::GetGroupMax();

                ImGui::SetCursorScreenPos(min - ImVec2(-8, ImGui::GetTextLineHeightWithSpacing() + 4));
                ImGui::BeginGroup();
                ImGui::TextUnformatted(node.Name.c_str());
                ImGui::EndGroup();

                auto drawList = ed::GetHintBackgroundDrawList();

                auto hintBounds = ImGui_GetItemRect();
                //auto hintFrameBounds = ImRect_Expanded(hintBounds, 8, 4);

                //drawList->AddRectFilled(
                //    hintFrameBounds.GetTL(),
                //    hintFrameBounds.GetBR(),
                //    IM_COL32(255, 255, 255, 64 * bgAlpha / 255), 4.0f);

                //drawList->AddRect(
                //    hintFrameBounds.GetTL(),
                //    hintFrameBounds.GetBR(),
                //    IM_COL32(255, 255, 255, 128 * bgAlpha / 255), 4.0f);

                //ImGui::PopStyleVar();
            }
            ed::EndGroupHint();
        }

        for (auto& link : m_Links)
            ed::Link(link.ID, link.StartPinID, link.EndPinID, link.Color, 2.0f);

        if (CreateNewNode == false)
        {
            if (ed::BeginCreate(ImColor(255, 255, 255), 2.0f))
            {
                auto showLabel = [](const char* label, ImColor color)
                    {
                        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
                        const ImVec2 size = ImGui::CalcTextSize(label);

                        const ImVec2 padding = ImGui::GetStyle().FramePadding;
                        const ImVec2 spacing = ImGui::GetStyle().ItemSpacing;

                        ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

                        auto rectMin = ImGui::GetCursorScreenPos() - padding;
                        auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

                        auto drawList = ImGui::GetWindowDrawList();
                        drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
                        ImGui::TextUnformatted(label);
                    };

                ed::PinId startPinId = 0, endPinId = 0;
                if (ed::QueryNewLink(&startPinId, &endPinId))
                {
                    auto startPin = FindPin(startPinId);
                    auto endPin = FindPin(endPinId);

                    NewLinkPin = startPin ? startPin : endPin;

                    if (startPin->Kind == PinKind::Input)
                    {
                        std::swap(startPin, endPin);
                        std::swap(startPinId, endPinId);
                    }

                    if (startPin && endPin)
                    {
                        if (endPin == startPin)
                        {
                            ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                        }
                        else if (endPin->Kind == startPin->Kind)
                        {
                            showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                        }
                        //else if (endPin->Node == startPin->Node)
                        //{
                        //    showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
                        //    ed::RejectNewItem(ImColor(255, 0, 0), 1.0f);
                        //}
                        else if (endPin->Type != startPin->Type)
                        {
                            showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 128, 128), 1.0f);
                        }
                        else
                        {
                            showLabel("+ Create Link", ImColor(32, 45, 32, 180));
                            if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
                            {
                                m_Links.emplace_back(Link(GetNextID(), startPinId, endPinId));
                                m_Links.back().Color = GetIconColor(startPin->Type);
                            }
                        }
                    }
                }

                ed::PinId pinId = 0;
                if (ed::QueryNewNode(&pinId))
                {
                    NewLinkPin = FindPin(pinId);
                    if (NewLinkPin)
                    {
                        showLabel("+ Create Node", ImColor(32, 45, 32, 180));
                    }

                    if (ed::AcceptNewItem())
                    {
                        CreateNewNode = true;
                        NewNodeLinkPin = FindPin(pinId);
                        NewLinkPin = nullptr;

                        NodeEd::Suspend();
                        ImGui::OpenPopup("Create New Node");
                        NodeEd::Resume();
                    }
                }
            }
            else
                NewLinkPin = nullptr;

            ed::EndCreate();

            if (ed::BeginDelete())
            {
                NodeEd::NodeId nodeId = 0;
                while (ed::QueryDeletedNode(&nodeId))
                {
                    if (ed::AcceptDeletedItem())
                    {
                        auto id = std::find_if(m_Nodes.begin(), m_Nodes.end(), [nodeId](auto& node) 
                        { 
                            return node.ID == nodeId; 
                        });
                        if (id != m_Nodes.end())
                        {
                            m_Nodes.erase(id);
                        }
                    }
                }

                ed::LinkId linkId = 0;
                while (ed::QueryDeletedLink(&linkId))
                {
                    if (ed::AcceptDeletedItem())
                    {
                        auto id = std::find_if(m_Links.begin(), m_Links.end(), [linkId](auto& link) 
                        { 
                            return link.ID == linkId; 
                        });
                        if (id != m_Links.end())
                        {
                            m_Links.erase(id);
                        }
                    }
                }
            }

            NodeEd::EndDelete();
        }

        //ImGui::SetCursorScreenPos(cursorTopLeft);
    }

    void LNodeEditor::ActivateContext()
    {
        EditorContext->SetAsCurrentEditorContext();
    }

}