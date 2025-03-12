#include "NodeEditorPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <imgui-node-editor/imgui_node_editor.h>
#include <imgui-node-editor/imgui_node_editor_internal.h>


namespace LkEngine {
    
    using namespace ax::NodeEditor::Detail;
    using namespace ax::NodeEditor::Details;

	namespace {
		bool bNodeStyleEditor = false;
	}

    LNodeEditorPanel::LNodeEditorPanel(std::string_view InName)
        : Name(InName)
    {
		LPANEL_REGISTER();

        EditorContext = TObjectPtr<LNodeEditorContext>::Create();
    }

	void LNodeEditorPanel::Initialize()
	{
		LK_CORE_DEBUG_TAG("NodeEditor", "Initializing");

		if (LNode* Node = SpawnInputActionNode(); Node != nullptr)
		{
			LK_CORE_DEBUG_TAG("NodeEditor", "Setting position of branch node {} ({})", Node->Name, Node->ID.Get());
			NodeEd::SetNodePosition(Node->ID, ImVec2(10, 25));
		}
		else
		{
			LK_CORE_WARN_TAG("NodeEditor", "Failed to spawn branch node");
		}

		LK_CORE_INFO_TAG("NodeEditor", "Navigating to content");
		NodeEd::NavigateToContent();

		LK_CORE_ASSERT(EditorContext);
	}

	void LNodeEditorPanel::RenderUI(bool& IsOpen)
    {
		if (bNodeStyleEditor)
		{
			ax::NodeEditor::Style& EditorStyle = NodeEd::GetStyle();
			if (ImGui::Begin("Node Editor Style", &bNodeStyleEditor, ImGuiWindowFlags_None))
			{
				ImGui::DragFloat4("Node Padding", &EditorStyle.NodePadding.x, 0.1f, 0.0f, 40.0f);
				ImGui::DragFloat("Node Rounding", &EditorStyle.NodeRounding, 0.1f, 0.0f, 40.0f);
				ImGui::DragFloat("Node Border Width", &EditorStyle.NodeBorderWidth, 0.1f, 0.0f, 15.0f);
				ImGui::DragFloat("Hovered Node Border Width", &EditorStyle.HoveredNodeBorderWidth, 0.1f, 0.0f, 15.0f);
				ImGui::DragFloat("Hovered Node Border Offset", &EditorStyle.HoverNodeBorderOffset, 0.1f, -40.0f, 40.0f);
				ImGui::DragFloat("Selected Node Border Width", &EditorStyle.SelectedNodeBorderWidth, 0.1f, 0.0f, 15.0f);
				ImGui::DragFloat("Selected Node Border Offset", &EditorStyle.SelectedNodeBorderOffset, 0.1f, -40.0f, 40.0f);
				ImGui::DragFloat("Pin Rounding", &EditorStyle.PinRounding, 0.1f, 0.0f, 40.0f);
				ImGui::DragFloat("Pin Border Width", &EditorStyle.PinBorderWidth, 0.1f, 0.0f, 15.0f);
				UI::Draw::DragFloat("Pin Rounding", &EditorStyle.PinRounding, 0.1f, 0.0f, 40.0f);
				UI::Draw::DragFloat("Pin Border Width", &EditorStyle.PinBorderWidth, 0.1f, 0.0f, 15.0f);
				ImGui::DragFloat("Link Strength", &EditorStyle.LinkStrength, 1.0f, 0.0f, 500.0f);
				UI::Draw::DragFloat("Link Strength", &EditorStyle.LinkStrength, 1.0f, 0.0f, 500.0f);
			}

			ImGui::End();
		}

		static constexpr ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_None;
		if (!UI::Begin(LK_FMT_LIB::format("##{}-Window", Name).c_str(), &IsOpen, WindowFlags))
		{
			return;
		}

		if (ImGui::Button("Style Editor"))
		{
			LK_CORE_DEBUG("Toggling node style editor");
			bNodeStyleEditor = !bNodeStyleEditor;
		}
		ImGui::SameLine(0, 12.0f);

		if (ImGui::Button("Spawn Branch Node"))
		{
			SpawnBranchNode();
		}
		ImGui::SameLine(0, 4.0f);
		if (ImGui::Button("Spawn Input Action Node"))
		{
			SpawnInputActionNode();
		}

		if (ImGui::Button("Create Link"))
		{
			//Links.emplace_back(LPinLink(GetNextID(), ))
		}

		UI::ShiftCursor(10.0f, 60.0f);
		{
			ImGui::Text("Editor Name: %s", Name.c_str());
			UI::ShiftCursorX(10.0f);
			ImGui::Text("Nodes: %d", NodeEd::GetNodeCount());
		}
		UI::ShiftCursor(-10.0f, -60.0f);

		EditorContext->SetAsCurrentEditorContext();

		static constexpr float Padding = 12.0f;
		static constexpr float Rounding = 12.0f;

		NodeEd::Begin(Name.c_str());
		{
			for (LNode& Node : Nodes)
			{
				if ((Node.Type != ENodeType::Blueprint) && (Node.Type != ENodeType::Simple))
				{
					continue;
				}

				const bool IsSimple = (Node.Type == ENodeType::Simple);

				std::vector<LPin>& Outputs = Node.Outputs;
				auto FindOutputOfDelegateType = [](const LPin& Output) -> bool
				{ 
					return (Output.Type == EPinType::Delegate); 
				};
				auto OutputIter = std::find_if(Outputs.begin(), Outputs.end(), FindOutputOfDelegateType);
				const bool HasOutputDelegates = (OutputIter != Outputs.end());

				NodeEd::BeginNode(Node.ID);
				{
					ImGui::PushID(Node.ID.AsPointer());

					/* Inputs. */
					if (!Node.Inputs.empty())
					{
						ImRect InputRect;
						for (LPin& Input : Node.Inputs)
						{
							InputRect = UI::GetItemRect();
							ImGui::Dummy(ImVec2(Padding, Padding));
							NodeEd::BeginPin(Input.ID, GetPinKind(Input));
							{
								NodeEd::PinPivotRect(InputRect.GetCenter(), InputRect.GetCenter());
								NodeEd::PinRect(InputRect.GetTL(), InputRect.GetBR());
							}
							NodeEd::EndPin();
						}
					} 

					//ImGui::Text("Node %d", Node.ID.AsPointer());
					ImGui::Text("%s (%d)", Node.Name.c_str(), Node.ID.AsPointer());

					if (!Node.Outputs.empty())
					{
						ImRect OutputRect;
						for (LPin& Output : Node.Outputs)
						{
							OutputRect = UI::GetItemRect();
							ImGui::Dummy(ImVec2(Padding, Padding));
							NodeEd::BeginPin(Output.ID, GetPinKind(Output));
							{
								NodeEd::PinPivotRect(OutputRect.GetCenter(), OutputRect.GetCenter());
								NodeEd::PinRect(OutputRect.GetTL(), OutputRect.GetBR());
							}
							NodeEd::EndPin();
						}
					}

				#if 0
					uint32_t Idx = 0;
					NodeEd::BeginPin(Idx++, NodeEd::PinKind::Input);
					{
						ImGui::Text("-> Input");
					}
					NodeEd::EndPin();
					ImGui::SameLine();
					NodeEd::BeginPin(Idx++, NodeEd::PinKind::Output);
					{
						ImGui::Text("Output ->");
					}
					NodeEd::EndPin();
				#endif

					ImGui::PopID();
				}
				NodeEd::EndNode();
			}
		}

		for (LPinLink& Link : Links)
		{
			NodeEd::Link(Link.ID, Link.StartPinID, Link.EndPinID, Link.Color, 2.0f);
		}

		NodeEd::End();

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


    void LNodeEditorPanel::BuildNode(LNode* Node)
    {
		LK_CORE_ASSERT(Node, "Cannot build node, invalid node reference");
        for (LPin& InputPin : Node->Inputs)
        {
            InputPin.Node = Node;
            InputPin.Kind = EPinKind::Input;
        }

        for (LPin& OutputPin : Node->Outputs)
        {
            OutputPin.Node = Node;
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

    NodeEd::LinkId LNodeEditorPanel::GetNextLinkID()
    {
        return NodeEd::LinkId(GetNextID());
    }

    void LNodeEditorPanel::TouchNode(NodeEd::NodeId id)
    {
        NodeTouchTime[id] = TouchTime;
    }

    float LNodeEditorPanel::GetTouchProgress(NodeEd::NodeId id)
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

    LNode* LNodeEditorPanel::FindNode(NodeEd::NodeId InNodeID)
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

    LPinLink* LNodeEditorPanel::FindLink(NodeEd::LinkId id)
    {
        for (auto& link : Links)
            if (link.ID == id)
                return &link;

        return nullptr;
    }

    LPin* LNodeEditorPanel::FindPin(NodeEd::PinId id)
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

    bool LNodeEditorPanel::IsPinLinked(NodeEd::PinId id)
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
            NodeEd::PushStyleColor(NodeEd::StyleColor_NodeBg, ImColor(255, 255, 255, 64));
            NodeEd::PushStyleColor(NodeEd::StyleColor_NodeBorder, ImColor(255, 255, 255, 64));
            NodeEd::BeginNode(node.ID);
            ImGui::PushID(node.ID.AsPointer());
            //ImGui::BeginVertical("content");
            //ImGui::BeginHorizontal("horizontal");
            //ImGui::Spring(1);
            ImGui::TextUnformatted(node.Name.c_str());
            //ImGui::Spring(1);
            //ImGui::EndHorizontal();
            NodeEd::Group(node.Size);
            //ImGui::EndVertical();
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

			#if 0
                ImGui::SetCursorScreenPos(min - ImVec2(-8, ImGui::GetTextLineHeightWithSpacing() + 4));
                ImGui::BeginGroup();
                ImGui::TextUnformatted(node.Name.c_str());
                ImGui::EndGroup();
			#endif

                auto drawList = NodeEd::GetHintBackgroundDrawList();

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
            NodeEd::EndGroupHint();
        }

		for (LPinLink& Link : Links)
		{
            NodeEd::Link(Link.ID, Link.StartPinID, Link.EndPinID, Link.Color, 2.0f);
		}

		#if 0
        if (bCreateNewNode == false)
        {
            if (NodeEd::BeginCreate(ImColor(255, 255, 255), 2.0f))
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

                NodeEd::PinId startPinId = 0, endPinId = 0;
                if (NodeEd::QueryNewLink(&startPinId, &endPinId))
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
                                Links.emplace_back(LPinLink(GetNextID(), startPinId, endPinId));
                                Links.back().Color = GetIconColor(startPin->Type);
                            }
                        }
                    }
                }

                NodeEd::PinId pinId = 0;
                if (NodeEd::QueryNewNode(&pinId))
                {
                    NewLinkPin = FindPin(pinId);
                    if (NewLinkPin)
                    {
                        showLabel("+ Create Node", ImColor(32, 45, 32, 180));
                    }

                    if (NodeEd::AcceptNewItem())
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

            NodeEd::EndCreate();

            if (NodeEd::BeginDelete())
            {
                NodeEd::NodeId nodeId = 0;
                while (NodeEd::QueryDeletedNode(&nodeId))
                {
                    if (NodeEd::AcceptDeletedItem())
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

                NodeEd::LinkId linkId = 0;
                while (NodeEd::QueryDeletedLink(&linkId))
                {
                    if (NodeEd::AcceptDeletedItem())
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
