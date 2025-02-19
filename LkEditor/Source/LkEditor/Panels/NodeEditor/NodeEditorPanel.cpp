#include "NodeEditorPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui-node-editor/imgui_node_editor_internal.h>


namespace LkEngine {
    
    using namespace ax::NodeEditor::Detail;
    using namespace ax::NodeEditor::Details;

    namespace NodeEd = ed;

    LNodeEditorPanel::LNodeEditorPanel(std::string_view InName)
        : Name(InName)
    {
		LPANEL_REGISTER();

        EditorContext = TObjectPtr<LNodeEditorContext>::Create();
    }

	void LNodeEditorPanel::Initialize()
	{
		LK_CORE_ERROR_TAG("LNodeEditorPanel", "Initialize");

		LNode* Node = nullptr;
		if (Node = SpawnInputActionNode(); Node != nullptr)
		{
			NodeEd::SetNodePosition(Node->ID, ImVec2(4, 4));
		}
		if (Node = SpawnBranchNode(); Node != nullptr)
		{
			NodeEd::SetNodePosition(Node->ID, ImVec2(10, 25));
		}
	}

	void LNodeEditorPanel::RenderUI(bool& IsOpen)
    {
        static constexpr int NodePin = 0;
        static constexpr int InputPin = 1;
        static constexpr int OutputPin = 2;

		static constexpr ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_None;
		if (!UI::Begin("Node Editor", &IsOpen, WindowFlags))
		{
			return;
		}

		if (ImGui::Button("Spawn Branch Node"))
		{
			SpawnBranchNode();
		}

		NE::Begin(Name.c_str());
		NE::BeginNode(NodePin);

		NE::BeginPin(InputPin, NE::PinKind::Input);
		ImGui::Text("In -->");
		NE::EndPin();

		NE::BeginPin(OutputPin, NE::PinKind::Output);
		ImGui::Text("Out -->");
		NE::EndPin();

		NE::EndNode();
		NE::End();

		//ImGui::End();
		UI::End();
    }

	void LNodeEditorPanel::SerializeToYaml(YAML::Emitter& Out) const
	{
		/* TODO */
		LK_UNUSED(Out);
	}

	void LNodeEditorPanel::DeserializeFromYaml(const YAML::Node& Data)
	{
		/* TODO */
		LK_UNUSED(Data);
	}

    void LNodeEditorPanel::Save()
    {
    }

    ImColor LNodeEditorPanel::GetIconColor(const EPinType InPinType)
    {
        switch (InPinType)
        {
			case EPinType::Flow:       return ImColor(255, 255, 255);
			case EPinType::Bool:       return ImColor(220, 48, 48);
			case EPinType::Int:        return ImColor(68, 201, 156);
			case EPinType::Float:      return ImColor(147, 226, 74);
			case EPinType::String:     return ImColor(124, 21, 153);
			case EPinType::Object:     return ImColor(51, 150, 215);
			case EPinType::Function:   return ImColor(218, 0, 183);
			case EPinType::Delegate:   return ImColor(255, 48, 48);
			default:
				break;
        }

        return ImColor();
    };

    void LNodeEditorPanel::DrawPinIcon(const LPin& pin, bool connected, int alpha)
    {
        ENodeIconType iconType;
        ImColor  color = GetIconColor(pin.Type);
        color.Value.w = alpha / 255.0f;
        switch (pin.Type)
        {
			case EPinType::Flow:        
                iconType = ENodeIconType::Flow;
                break;

			case EPinType::Bool:        
                iconType = ENodeIconType::Circle; 
                break;

			case EPinType::Int:         
                iconType = ENodeIconType::Circle; 
                break;

			case EPinType::Float:       
                iconType = ENodeIconType::Circle; 
                break;

			case EPinType::String:      
                iconType = ENodeIconType::Circle; 
                break;

			case EPinType::Object:      
                iconType = ENodeIconType::Circle; 
                break;

			case EPinType::Function:    
                iconType = ENodeIconType::Circle; 
                break;

			case EPinType::Delegate:    
                iconType = ENodeIconType::Square; 
                break;

			default:
				return;
        }

        // FIXME: 
	#if 0
        Icon(ImVec2(static_cast<float>(PinIconSize), static_cast<float>(PinIconSize)), 
             iconType, 
             connected, 
             color, 
             ImColor(32, 32, 32, alpha)
        );
	#endif
    };


    void LNodeEditorPanel::BuildNode(LNode* node)
    {
        for (LPin& InputPin : node->Inputs)
        {
            InputPin.Node = node;
            InputPin.Kind = EPinKind::Input;
        }

        for (LPin& OutputPin : node->Outputs)
        {
            OutputPin.Node = node;
            OutputPin.Kind = EPinKind::Output;
        }
    }

    LNode* LNodeEditorPanel::SpawnInputActionNode()
    {
        Nodes.emplace_back(GetNextID(), "InputAction Fire", ImColor(255, 128, 128));
        Nodes.back().Outputs.emplace_back(GetNextID(), "", EPinType::Delegate);
        Nodes.back().Outputs.emplace_back(GetNextID(), "Pressed", EPinType::Flow);
        Nodes.back().Outputs.emplace_back(GetNextID(), "Released", EPinType::Flow);

        BuildNode(&Nodes.back());

        return &Nodes.back();
    }

    LNode* LNodeEditorPanel::SpawnBranchNode()
    {
        Nodes.emplace_back(GetNextID(), "Branch");
        Nodes.back().Inputs.emplace_back(GetNextID(), "", EPinType::Flow);
        Nodes.back().Inputs.emplace_back(GetNextID(), "Condition", EPinType::Bool);
        Nodes.back().Outputs.emplace_back(GetNextID(), "True", EPinType::Flow);
        Nodes.back().Outputs.emplace_back(GetNextID(), "False", EPinType::Flow);

        BuildNode(&Nodes.back());

        return &Nodes.back();
    }

    int LNodeEditorPanel::GetNextID()
    {
        return NextID++;
    }

    NE::LinkId LNodeEditorPanel::GetNextLinkID()
    {
        return NE::LinkId(GetNextID());
    }

    void LNodeEditorPanel::TouchNode(NE::NodeId id)
    {
        NodeTouchTime[id] = TouchTime;
    }

    float LNodeEditorPanel::GetTouchProgress(NE::NodeId id)
    {
        auto it = NodeTouchTime.find(id);
        if (it != NodeTouchTime.end() && it->second > 0.0f)
            return (TouchTime - it->second) / TouchTime;
        else
            return 0.0f;
    }

    void LNodeEditorPanel::UpdateTouch()
    {
        const auto deltaTime = ImGui::GetIO().DeltaTime;
        for (auto& entry : NodeTouchTime)
        {
            if (entry.second > 0.0f)
                entry.second -= deltaTime;
        }
    }

    LNode* LNodeEditorPanel::FindNode(NE::NodeId InNodeID)
    {
		auto FindNode = [InNodeID](const LNode& Node) -> bool
		{
			return (InNodeID == Node.ID);
		};
		if (auto Iter = std::find_if(Nodes.begin(), Nodes.end(), FindNode); Iter != Nodes.end())
		{
			return &(*Iter);
		}
	#if 0
		for (LNode& Node : Nodes)
		{
			if (Node.ID == InNodeID)
			{
                return &Node;
			}
		}
	#endif

        return nullptr;
    }

    LPinLink* LNodeEditorPanel::FindLink(NE::LinkId id)
    {
        for (auto& link : Links)
            if (link.ID == id)
                return &link;

        return nullptr;
    }

    LPin* LNodeEditorPanel::FindPin(NE::PinId id)
    {
        if (!id)
            return nullptr;

        for (auto& node : Nodes)
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

    bool LNodeEditorPanel::IsPinLinked(NE::PinId id)
    {
        if (!id)
            return false;

        for (auto& link : Links)
            if (link.StartPinID == id || link.EndPinID == id)
                return true;

        return false;
    }

    bool LNodeEditorPanel::CanCreateLink(LPin* a, LPin* b)
    {
        if (!a || !b || a == b || a->Kind == b->Kind || a->Type != b->Type || a->Node == b->Node)
            return false;

        return true;
    }

    void LNodeEditorPanel::IterateNodes()
    {
        for (LNode& node : Nodes)
        {
            if (node.Type != ENodeType::Comment)
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

			#if 0
                ImGui::SetCursorScreenPos(min - ImVec2(-8, ImGui::GetTextLineHeightWithSpacing() + 4));
                ImGui::BeginGroup();
                ImGui::TextUnformatted(node.Name.c_str());
                ImGui::EndGroup();
			#endif

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

		for (LPinLink& Link : Links)
		{
            ed::Link(Link.ID, Link.StartPinID, Link.EndPinID, Link.Color, 2.0f);
		}

		#if 0
        if (bCreateNewNode == false)
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

                    if (startPin->Kind == EPinKind::Input)
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
                                Links.emplace_back(LPinLink(GetNextID(), startPinId, endPinId));
                                Links.back().Color = GetIconColor(startPin->Type);
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
                        bCreateNewNode = true;
                        NewNodeLinkPin = FindPin(pinId);
                        NewLinkPin = nullptr;

                        NodeEd::Suspend();
                        ImGui::OpenPopup("Create New Node");
                        NodeEd::Resume();
                    }
                }
            }
			else
			{
                NewLinkPin = nullptr;
			}

            ed::EndCreate();

            if (ed::BeginDelete())
            {
                NodeEd::NodeId nodeId = 0;
                while (ed::QueryDeletedNode(&nodeId))
                {
                    if (ed::AcceptDeletedItem())
                    {
                        auto id = std::find_if(Nodes.begin(), Nodes.end(), [nodeId](auto& node) 
                        { 
                            return node.ID == nodeId; 
                        });
                        if (id != Nodes.end())
                        {
                            Nodes.erase(id);
                        }
                    }
                }

                ed::LinkId linkId = 0;
                while (ed::QueryDeletedLink(&linkId))
                {
                    if (ed::AcceptDeletedItem())
                    {
                        auto id = std::find_if(Links.begin(), Links.end(), [linkId](auto& link) 
                        { 
                            return link.ID == linkId; 
                        });
                        if (id != Links.end())
                        {
                            Links.erase(id);
                        }
                    }
                }
            }

            NodeEd::EndDelete();
        }
		#endif

        //ImGui::SetCursorScreenPos(cursorTopLeft);
    }

    void LNodeEditorPanel::ActivateContext()
    {
        EditorContext->SetAsCurrentEditorContext();
    }

}
