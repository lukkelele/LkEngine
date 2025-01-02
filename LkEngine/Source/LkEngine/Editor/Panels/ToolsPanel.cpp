#include "LKpch.h"
#include "ToolsPanel.h"

#include "LkEngine/Core/Input/Input.h"

#include "LkEngine/Project/Project.h"

#include "LkEngine/Asset/AssetImporter.h"
#include "LkEngine/Asset/RuntimeAssetManager.h"

#include "LkEngine/Serialization/Serializer.h"


namespace LkEngine {

	bool LToolsPanel::bWindow_ObjectReferences = false;
	bool LToolsPanel::bWindow_AssetRegistry = false;
	bool LToolsPanel::bWindow_InputInfo = false;
	bool LToolsPanel::bWindow_UserInterfaceTools = false;
	bool LToolsPanel::bWindow_RegisteredFonts = false;

	bool LToolsPanel::bUserInterfaceTools_TreeNode_MessageBoxes = true;

	LToolsPanel::LToolsPanel()
	{
		LOBJECT_REGISTER();
	}

	void LToolsPanel::OnRenderUI(bool& IsOpen)
	{
		if (bWindow_ObjectReferences)
		{
			UI_ObjectReferences();
		}

		if (bWindow_AssetRegistry)
		{ 
			UI_AssetRegistry();
		}

		if (bWindow_InputInfo)
		{
			UI_InputInfo();
		}

		if (bWindow_UserInterfaceTools)
		{
			UI_UserInterfaceTools();
		}

		if (bWindow_RegisteredFonts)
		{ 
			UI_RegisteredFonts();
		}

		IsOpen = (bWindow_ObjectReferences 
			|| bWindow_AssetRegistry 
			|| bWindow_InputInfo
			|| bWindow_UserInterfaceTools
			|| bWindow_RegisteredFonts
		);
	}

	void LToolsPanel::SerializeToYaml(YAML::Emitter& Out) const
	{
		LK_CORE_TRACE_TAG("ToolsPanel", "Serializing to YAML");
		Out << YAML::Key << "Windows" << YAML::Value << YAML::BeginMap;
		{
			LK_SERIALIZE_PROPERTY(ObjectReferences, bWindow_ObjectReferences, Out);
			LK_SERIALIZE_PROPERTY(AssetRegistry, bWindow_AssetRegistry, Out);
			LK_SERIALIZE_PROPERTY(InputInfo, bWindow_InputInfo, Out);
			LK_SERIALIZE_PROPERTY(UserInterfaceTools, bWindow_UserInterfaceTools, Out);
			LK_SERIALIZE_PROPERTY(RegisteredFonts, bWindow_RegisteredFonts, Out);
		}
		Out << YAML::EndMap;
	}

	void LToolsPanel::DeserializeFromYaml(const YAML::Node& Data)
	{
		const YAML::Node& WindowsNode = Data["Windows"];
		if (!WindowsNode)
		{
			LK_CORE_ERROR_TAG("ToolsPanel", "Node 'Windows' is not valid");
			return;
		}
		
		LK_DESERIALIZE_PROPERTY(ObjectReferences, bWindow_ObjectReferences, WindowsNode, false);
		LK_DESERIALIZE_PROPERTY(AssetRegistry, bWindow_AssetRegistry, WindowsNode, false);
		LK_DESERIALIZE_PROPERTY(InputInfo, bWindow_InputInfo, WindowsNode, false);
		LK_DESERIALIZE_PROPERTY(UserInterfaceTools, bWindow_UserInterfaceTools, WindowsNode, false);
		LK_DESERIALIZE_PROPERTY(RegisteredFonts, bWindow_RegisteredFonts, WindowsNode, false);
	}

	void LToolsPanel::UI_ObjectReferences()
	{
		static ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_None;
		ImGui::SetNextWindowSize(ImVec2(720, 960), ImGuiCond_FirstUseEver);
		ImGui::Begin("Object References", &bWindow_ObjectReferences, WindowFlags);
		{
			static std::size_t SelectedIndex = -1;
			static std::size_t SelectedIndexPrev = -1;

			/* Items to show, shows a scrollbar if greater. */
			static int MaxVisibleItems = std::numeric_limits<int>::max();
			static std::vector<bool> Selected(MaxVisibleItems, false);

			/* Show only static class object references. */
			static bool bFilterByStaticClassObjects = true;

			static std::vector<TObjectPtr<LObject>> LiveObjects;
			const int LiveObjectCount = TObjectPtr_Internal::GetLiveObjects(LiveObjects, bFilterByStaticClassObjects);
			/* Resize if needed. */
			if (Selected.size() < LiveObjects.size())
			{
				LK_CORE_INFO("Resizing selected vector");
				Selected.resize(LiveObjects.size(), false);
			}

			/**
			 * Clear selection if selecting indices too large.
			 * Can happen if filtering is toggled whilst having an object
			 * reference selected.
			 */
			if (SelectedIndex >= Selected.size()) 
			{
				SelectedIndex = -1;
				SelectedIndexPrev = -1;
			}

			if (LiveObjects.size() < MaxVisibleItems)
			{
				MaxVisibleItems = static_cast<int>(LiveObjects.size());
			}

			/* Calculate the max height for the window. */
			const float ItemHeight = ImGui::GetTextLineHeightWithSpacing();
			float ObjectsWindowHeight = MaxVisibleItems * ItemHeight;
			if (SelectedIndex == -1)
			{
				float ObjectsWindowHeight = 0;
			}

			ImGui::Text("Total Live References: %d", LiveObjectCount);
			ImGui::Checkbox("Filter by single class only", &bFilterByStaticClassObjects);

			static constexpr ImGuiTableFlags TableFlags = ImGuiTableFlags_Borders 
				| ImGuiTableFlags_RowBg 
				| ImGuiTableFlags_ScrollY 
				| ImGuiTableFlags_Sortable
				| ImGuiTableFlags_SizingStretchProp;

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(2, 2));
			if (ImGui::BeginTable("Live Objects", 2, TableFlags, ImVec2(0, ObjectsWindowHeight)))
			{
				ImGui::TableSetupColumn("Object Index", ImGuiTableColumnFlags_IndentDisable);
				ImGui::TableSetupColumn("Class Name", ImGuiTableColumnFlags_IndentDisable);
				ImGui::TableHeadersRow();

				int Index = 0;
				for (const TObjectPtr<LObject>& LiveObject : LiveObjects)
				{
					LK_CORE_VERIFY(LiveObject->GetClass(), "{} object has invalid LClass reference", LiveObject->ClassName());

					/* Display the object number. */
					ImGui::TableNextColumn();
					ImGui::Text("%d", (Index + 1));

					/* Object classname. */
					char Label[200];
					std::sprintf(Label, "%s ##%lld", LiveObject->GetClass()->GetName().c_str(), reinterpret_cast<uintptr_t>(LiveObject.Get()));

					const bool bIsCurrentlySelected = Selected[Index];
					bool bSelected = false;

					ImGui::TableNextColumn();
					if (ImGui::Selectable(Label, Selected[Index], ImGuiSelectableFlags_SpanAllColumns))
					{
						bSelected = true;
					}

					/* Set to false if item already was selected. */
					if (bSelected && bIsCurrentlySelected)
					{
						Selected[Index] = false;
						SelectedIndex = -1;
					}
					/* Select the item that was not previously selected. */
					else if (bSelected && !bIsCurrentlySelected)
					{
						std::fill(Selected.begin(), Selected.end(), false);
						SelectedIndex = Index;
						Selected[Index] = true;
					}

					Index++;
				}

				ImGui::EndTable();
			}
			ImGui::PopStyleVar(2);


			/* Display information about a selected object, if any is selected. */
			if ((SelectedIndex != -1) && (SelectedIndex < LiveObjects.size()))
			{
				/* Limit visible items. */
				MaxVisibleItems = 20;

				/* 
				 * TODO: Should use a weak pointer here so the viewing of an object
				 *       does not extend its lifetime and thus prohibit its (potential) destruction.
				 */
				TObjectPtr<LObject> SelectedObject = LiveObjects.at(SelectedIndex);

				ImGui::BeginChild("Object Information", ImVec2(0, 0), true, ImGuiWindowFlags_NoNavInputs);
				{
					static constexpr ImGuiTableFlags ObjectInfoTableFlags = ImGuiTableFlags_Borders
						| ImGuiTableFlags_RowBg
						| ImGuiTableFlags_SizingFixedFit;

					const LClass* Class = SelectedObject->GetClass();
					if (ImGui::BeginTable("##ObjectInfo", 2, ObjectInfoTableFlags, ImVec2(0, 0)))
					{
						/* Name. */
						ImGui::TableNextColumn();
						ImGui::Text("Name");
						ImGui::TableNextColumn();
						ImGui::Text("%s", Class->GetName().c_str());

						/* Reference Count. */
						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						ImGui::Text("References");
						ImGui::TableNextColumn();
						ImGui::Text("%d", SelectedObject->GetReferenceCount());

						/* Memory Address. */
						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						ImGui::Text("Address");
						ImGui::TableNextColumn();
						ImGui::Text("0x%p", reinterpret_cast<intptr_t>(SelectedObject.Get()));

						ImGui::EndTable(); /* */
					}
				}
				ImGui::EndChild();
			}
			/* No object selected, expand window. */
			else
			{
				MaxVisibleItems = std::numeric_limits<int>::max();
			}

		}

		LK_UI_DEBUG_ON_HOVER();
		ImGui::End();
	}

	void LToolsPanel::UI_AssetRegistry()
	{
		const ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_None;
		ImGui::SetNextWindowSize(ImVec2(500, 560), ImGuiCond_FirstUseEver);
		ImGui::Begin("Asset Registry", &bWindow_AssetRegistry, WindowFlags);

		if (TObjectPtr<LProject> Project = LProject::Current(); Project != nullptr)
		{
			TObjectPtr<LRuntimeAssetManager> AssetManager = Project->GetRuntimeAssetManager();
			LK_CORE_VERIFY(AssetManager, "Invalid asset manager reference");
			const LAssetRegistry& AssetRegistry = AssetManager->GetAssetRegistry();

			if (ImGui::BeginTable("##Table-AssetRegistry", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp))
			{
				ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("Index").x);
				ImGui::TableSetupColumn("Handle");
				ImGui::TableSetupColumn("Filepath", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableHeadersRow();

				int Index = 1;
				for (const auto& [AssetHandle, Metadata] : AssetRegistry) 
				{
					ImGui::TableNextRow();
					/* Column 1: Index */
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%d", Index);

					/* Column 2: Font Size */
					ImGui::TableSetColumnIndex(1);
					ImGui::Text("%lld", AssetHandle);

					/* Column 3: Filepath */
					ImGui::TableSetColumnIndex(2);
					ImGui::Text("%s", Metadata.FilePath.string().c_str());

					Index++;
				}

				ImGui::EndTable();
			}
		}

		LK_UI_DEBUG_ON_HOVER();
		ImGui::End();
	}

	void LToolsPanel::UI_RegisteredFonts()
	{
		static std::vector<FFontEntry> RegisteredFonts;
		const std::size_t RegisteredFontsCount = UI::Font::GetRegistered(RegisteredFonts);

		/* Place the default font named 'Default' first. */
		std::sort(RegisteredFonts.begin(), RegisteredFonts.end(), [](const FFontEntry& A, const FFontEntry& B)
		{
			return (A.Name == "Default") || (B.Name != "Default" && A.Name < B.Name);
		});

		/* Determine the maximum width based on the longest font name. */
		auto FontEntryComparitor = [](const FFontEntry& A, const FFontEntry& B)
		{
			return A.Name.length() < B.Name.length();
		};

		/* Calculate the width dynamically based on the longest name, with some padding. */
		static constexpr float NamePadding = 12.0f;
		const auto LongestNameIt = std::max_element(RegisteredFonts.begin(), RegisteredFonts.end(), FontEntryComparitor);
		const float NameColumnWidth = ImGui::CalcTextSize(LongestNameIt->Name.c_str()).x + NamePadding;

		const std::string WindowName = std::format("Registered Fonts: {}", RegisteredFontsCount);
		ImGui::Begin(WindowName.c_str(), &bWindow_RegisteredFonts);
		if (ImGui::BeginTable("RegisteredFontsTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
		{
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, NameColumnWidth);
			ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 34.0f);
			ImGui::TableSetupColumn("Filepath", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableHeadersRow();
			for (const auto& Font : RegisteredFonts)
			{
				ImGui::TableNextRow();
				/* Column 1: Font Name */
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s", Font.Name.c_str());

				/* Column 2: Font Size */
				ImGui::TableSetColumnIndex(1);
				ImGui::Text("%.1f", Font.Size);

				/* Column 3: Filepath */
				ImGui::TableSetColumnIndex(2);
				ImGui::Text("%s", Font.FilePath.c_str());
			}

			ImGui::EndTable();
		}

		LK_UI_DEBUG_ON_HOVER();

		ImGui::End();
	}

	void LToolsPanel::UI_InputInfo()
	{
		ImGui::Begin("Input Info", &bWindow_InputInfo);

		/* Keyboard Inputs. */
		{
			static std::vector<EKey> PressedKeys;
			const std::size_t PressedKeyCount = LInput::GetPressedKeys(PressedKeys);
			const std::string Title = std::format("Pressed Keys: {}", PressedKeyCount);
			if (ImGui::BeginTable(Title.c_str(), 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit))
			{
				/* Set up the columns for the table. */
				ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("Key ------").x);
				ImGui::TableSetupColumn("Repeat Count", ImGuiTableColumnFlags_WidthFixed, 100.0f);
				ImGui::TableSetupColumn("Time Held (ms)", ImGuiTableColumnFlags_WidthFixed, 120.0f);
				ImGui::TableHeadersRow();

				for (const EKey PressedKey : PressedKeys)
				{
					using namespace std::chrono;

					const FKeyData& KeyData = LInput::GetKeyData(PressedKey);
					const auto TimeHeld = LInput::GetKeyHeldTime<milliseconds>(PressedKey);

					ImGui::TableNextRow();

					/* Column 1: Key Name */
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s", Enum::ToString(PressedKey));

					/* Column 2: Repeat Count */
					ImGui::TableSetColumnIndex(1);
					ImGui::Text("%d", KeyData.RepeatCount);

					/* Column 3: Time Held in milliseconds */
					ImGui::TableSetColumnIndex(2);
					ImGui::Text("%d", static_cast<int>(TimeHeld.count()));
				}

				ImGui::EndTable();
			}
		}

		ImGui::End();
	}

	void LToolsPanel::UI_UserInterfaceTools()
	{
		ImGui::Begin("UI Tools", &bWindow_UserInterfaceTools);

		/* TODO: Serialize the 'Open-by-default'-logic to load the editor in the same state 
		 *       it was closed in. */
		if (UI::BeginTreeNode("Message Boxes", bUserInterfaceTools_TreeNode_MessageBoxes))
		{
			static constexpr float TitleWidth = 148.0f;

			ImGui::Text("Message Boxes: %d", UI::Internal::MessageBoxes.size());

		#if 0
			UI::Font::Push("Large");
			ImGui::Text("Message Box");
			UI::Font::Pop();
		#endif
			/* Message Box. */
			{
				static char InputBuffer_BoxTitle[128] = { "Message Box" };
				static char InputBuffer_BoxText[128] = { "Template Box Text" };
				ImGui::Text("Title");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(TitleWidth);
				ImGui::InputText("##MessageBoxTitle", InputBuffer_BoxTitle, LK_ARRAYSIZE(InputBuffer_BoxTitle));

				ImGui::Text("Box Text");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(TitleWidth);
				ImGui::InputText("##MessageBoxText", InputBuffer_BoxText, LK_ARRAYSIZE(InputBuffer_BoxText));

				static bool bOkButton = false;
				static bool bCancelButton = false;
				static bool bAutoResize = false;
				ImGui::Text("Flags");
				ImGui::Indent();
				{
					ImGui::Checkbox("Ok Button", &bOkButton);
					//ImGui::SameLine();
					ImGui::Checkbox("Cancel Button", &bCancelButton);
					//ImGui::SameLine();
					ImGui::Checkbox("Auto Resize", &bAutoResize);
				}
				ImGui::Unindent();

				ImGui::Dummy(ImVec2(0, 4.0f));
				ImGui::Separator();
				ImGui::Dummy(ImVec2(0, 4.0f));

				if (ImGui::Button("Create##MessageBox"))
				{
					const std::string BoxTitle(InputBuffer_BoxTitle);
					
					uint32_t BoxFlags = 0;
					if (bOkButton)
					{
						BoxFlags |= (uint32_t)EMessageBoxFlag::OkButton;
					}
					if (bCancelButton)
					{
						BoxFlags |= (uint32_t)EMessageBoxFlag::CancelButton;
					}
					if (bAutoResize)
					{
						BoxFlags |= (uint32_t)EMessageBoxFlag::AutoSize;
					}

					UI::ShowMessageBox(BoxTitle.c_str(), []()
					{
						ImGui::Text("Message Box Text");
					}, BoxFlags);
				}
			}

			UI::VSeparator(10.0f);

			UI::EndTreeNode();
		}

		if (UI::BeginTreeNode("UI Properties", false))
		{
			/* Property 1. */
			{
				static bool bProperty1 = false;
				UI::Property("Property 1", bProperty1, "Help text for property 1");
			}

			/* Vector3 Control. */
			{
				static constexpr float ResetValue = 0.0f;
				static constexpr float ColumnWidth = 100.0f;
				static bool bManuallyEdited1 = false;
				static bool bManuallyEdited2 = false;

				static glm::vec3 VectorData1;
				static glm::vec3 VectorData2;
				static uint32_t Axes = (uint32_t)(EVectorAxis::X);
				if (ImGui::BeginTable("VectorControl", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_NoClip))
				{
					ImGui::TableSetupColumn("LabelColumn", 0, 100.0f);
					ImGui::TableSetupColumn("ValueColumn", ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoClip, ImGui::GetContentRegionAvail().x - 100.0f);

					ImGui::TableNextRow();
					UI::Draw::Vec3Control("Info 1", VectorData1, bManuallyEdited1, ResetValue, 0.10f, 0.0f, 0.0f, ColumnWidth, Axes);

					ImGui::TableNextRow();
					UI::Draw::Vec3Control("Info 2", VectorData2, bManuallyEdited2, ResetValue, 1.50f, 10.0f, 100.0f, ColumnWidth, Axes);

					ImGui::EndTable();
				}

			}

			UI::EndTreeNode();
		}

		
		ImGui::End();
	}

}
