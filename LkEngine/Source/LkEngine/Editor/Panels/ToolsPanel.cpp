#include "LKpch.h"
#include "ToolsPanel.h"

#include "LkEngine/Core/SelectionContext.h"
#include "LkEngine/Core/Input/Input.h"

#include "LkEngine/Project/Project.h"

#include "LkEngine/Asset/AssetImporter.h"
#include "LkEngine/Asset/EditorAssetManager.h"

#include "LkEngine/Serialization/Serializer.h"

#include "LkEngine/Editor/Panels/ContentBrowserPanel.h"


namespace LkEngine {

	namespace 
	{
		constexpr std::size_t ASSETHANDLE_MAX_VALUE = std::numeric_limits<LUUID::SizeType>::max();
		const int ASSETHANDLE_CHAR_COUNT = static_cast<int>(std::log10(ASSETHANDLE_MAX_VALUE)) + 1;
		const std::string ASSETHANDLE_STRING(ASSETHANDLE_CHAR_COUNT, '1');
	}

	LToolsPanel::LToolsPanel()
	{
		LPANEL_REGISTER();
	}

	void LToolsPanel::RenderUI(bool& IsOpen)
	{
		if (Window_ObjectReferences.bOpen)
		{
			UI_ObjectReferences();
		}

		if (Window_AssetRegistry.bOpen)
		{ 
			UI_AssetRegistry();
		}

		if (Window_InputInfo.bOpen)
		{
			UI_InputInfo();
		}

		if (Window_UserInterfaceTools.bOpen)
		{
			UI_UserInterfaceTools();
		}

		if (Window_Fonts.bOpen)
		{ 
			UI_Fonts();
		}

		if (Window_Sandbox.bOpen)
		{
			UI_Sandbox();
		}

		IsOpen = (Window_ObjectReferences.bOpen
			|| Window_AssetRegistry.bOpen
			|| Window_InputInfo.bOpen
			|| Window_UserInterfaceTools.bOpen
			|| Window_Fonts.bOpen
			|| Window_Sandbox.bOpen
		);
	}

	void LToolsPanel::SerializeToYaml(YAML::Emitter& Out) const
	{
		LK_CORE_TRACE_TAG("ToolsPanel", "Serializing to YAML");
		Out << YAML::Key << "Windows" << YAML::Value << YAML::BeginMap;
		{
			LK_SERIALIZE_PROPERTY(ObjectReferences, Window_ObjectReferences.bOpen, Out);
			LK_SERIALIZE_PROPERTY(AssetRegistry, Window_AssetRegistry.bOpen, Out);

			/* Window: InputInfo */
			LK_SERIALIZE_PROPERTY(InputInfo, Window_InputInfo.bOpen, Out);
			LK_SERIALIZE_PROPERTY(InputInfo_TreeNode_KeyInfo, Window_InputInfo.bTreeNode_KeyInfo, Out);
			LK_SERIALIZE_PROPERTY(InputInfo_TreeNode_Selection, Window_InputInfo.bTreeNode_Selection, Out);

			LK_SERIALIZE_PROPERTY(UserInterfaceTools, Window_UserInterfaceTools.bOpen, Out);
			LK_SERIALIZE_PROPERTY(Fonts, Window_Fonts.bOpen, Out);
			LK_SERIALIZE_PROPERTY(Sandbox, Window_Sandbox.bOpen, Out);
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
		
		LK_DESERIALIZE_PROPERTY(ObjectReferences, Window_ObjectReferences.bOpen, WindowsNode, false);
		LK_DESERIALIZE_PROPERTY(AssetRegistry, Window_AssetRegistry.bOpen, WindowsNode, false);

		LK_DESERIALIZE_PROPERTY(InputInfo, Window_InputInfo.bOpen, WindowsNode, false);
		LK_DESERIALIZE_PROPERTY(InputInfo_TreeNode_KeyInfo, Window_InputInfo.bTreeNode_KeyInfo, WindowsNode, false);
		LK_DESERIALIZE_PROPERTY(InputInfo_TreeNode_Selection, Window_InputInfo.bTreeNode_Selection, WindowsNode, false);

		LK_DESERIALIZE_PROPERTY(UserInterfaceTools, Window_UserInterfaceTools.bOpen, WindowsNode, false);
		LK_DESERIALIZE_PROPERTY(Fonts, Window_Fonts.bOpen, WindowsNode, false);
		LK_DESERIALIZE_PROPERTY(Sandbox, Window_Sandbox.bOpen, WindowsNode, false);
	}

	void LToolsPanel::UI_ObjectReferences()
	{
		static ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_None;
		ImGui::SetNextWindowSize(ImVec2(720, 960), ImGuiCond_FirstUseEver);
		ImGui::Begin("Object References", &Window_ObjectReferences.bOpen, WindowFlags);
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

		LK_UI_DEBUG_WINDOW_ON_HOVER();
		ImGui::End();
	}

	void LToolsPanel::UI_AssetRegistry()
	{
		static int CurrentSortColumn = 0;
		static ImGuiSortDirection CurrentSortDirection = ImGuiSortDirection_Ascending;

		auto SortAssets = [](const std::pair<LUUID, FAssetMetadata>& A, const std::pair<LUUID, FAssetMetadata>& B) -> bool
		{
			/* Column 0: AssetHandle */
			if (CurrentSortColumn == 0)
			{
				if (CurrentSortDirection == ImGuiSortDirection_Ascending)
				{
					return (A.first < B.first);
				}
				else
				{
					return (A.first > B.first);
				}
			}
			/* Column 1: FilePath */
			else if (CurrentSortColumn == 1)
			{
				if (CurrentSortDirection == ImGuiSortDirection_Ascending)
				{
					return (A.second.FilePath < B.second.FilePath);
				}
				else
				{
					return (A.second.FilePath > B.second.FilePath);
				}
			}

			return false;
		};

		const ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_None;
		ImGui::SetNextWindowSize(ImVec2(500, 560), ImGuiCond_FirstUseEver);
		ImGui::Begin("Asset Registry", &Window_AssetRegistry.bOpen, WindowFlags);

		if (TObjectPtr<LProject> Project = LProject::Current(); Project != nullptr)
		{
			TObjectPtr<LRuntimeAssetManager> AssetManager = Project->GetRuntimeAssetManager();
			LK_CORE_VERIFY(AssetManager, "Invalid asset manager reference");
			const LAssetRegistry& AssetRegistry = AssetManager->GetAssetRegistry();

			/* Copy the asset registry for the sake of sorting it. */
			std::vector<std::pair<FAssetHandle, FAssetMetadata>> AssetRegistrySorted(AssetRegistry.begin(), AssetRegistry.end());
			std::sort(AssetRegistrySorted.begin(), AssetRegistrySorted.end(), SortAssets);

			static constexpr ImGuiTableFlags TableFlags = ImGuiTableFlags_Borders 
				| ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp 
				| ImGuiTableFlags_Sortable;
			if (ImGui::BeginTable("##Table-AssetRegistry", 2, TableFlags))
			{
				ImGui::TableSetupColumn("Handle", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize(ASSETHANDLE_STRING.c_str()).x);
				ImGui::TableSetupColumn("Filepath");
				ImGui::TableHeadersRow();

				ImGuiTableSortSpecs* SortSpecs = ImGui::TableGetSortSpecs();
				if (SortSpecs && SortSpecs->SpecsDirty)
				{
					const ImGuiTableColumnSortSpecs* SortSpec = &SortSpecs->Specs[0];
					CurrentSortColumn = SortSpec->ColumnIndex;
					CurrentSortDirection = SortSpec->SortDirection;
					SortSpecs->SpecsDirty = false;
				}

				for (const auto& [AssetHandle, Metadata] : AssetRegistrySorted) 
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					const std::string AssetHandleStr = std::format("{}", AssetHandle);
					ImGui::Text("%s", AssetHandleStr.c_str());

					ImGui::TableSetColumnIndex(1);
					ImGui::Text("%s", Metadata.FilePath.string().c_str());
				}

				ImGui::EndTable();
			}
		}

		LK_UI_DEBUG_WINDOW_ON_HOVER();
		ImGui::End();
	}

	void LToolsPanel::UI_InputInfo()
	{
		ImGui::SetNextWindowSize(ImVec2(400, 680), ImGuiCond_FirstUseEver);
		ImGui::Begin("Input Info", &Window_InputInfo.bOpen, ImGuiWindowFlags_NoCollapse);

		/* TreeNode: Selection */
		ImGui::SetNextItemOpen(Window_InputInfo.bTreeNode_Selection, ImGuiCond_Once);
		Window_InputInfo.bTreeNode_Selection = ImGui::TreeNodeEx("Selection", ImGuiTreeNodeFlags_None);
		if (Window_InputInfo.bTreeNode_Selection)
		{
			ImGuiStyle& Style = ImGui::GetStyle();
			const float RowPaddingX = Style.FramePadding.x * 2.0f;

			const ImVec2 WindowSize = ImGui::GetContentRegionAvail();
			const float RowLength = std::max(ImGui::CalcTextSize(ASSETHANDLE_STRING.c_str()).x + RowPaddingX, 
											 (WindowSize.x * 0.50f - (2 * Style.FramePadding.x)));
			const LVector2 TableSize(RowLength, 0);

			UI_InputInfo_SceneSelection(TableSize);
			ImGui::SameLine();
			UI_InputInfo_ContentBrowserSelection(TableSize);

			ImGui::TreePop();
		}

		UI::VSeparator(10.0f);

		/* TreeNode: Key Info */
		ImGui::SetNextItemOpen(Window_InputInfo.bTreeNode_KeyInfo, ImGuiCond_Once);
		Window_InputInfo.bTreeNode_KeyInfo = ImGui::TreeNodeEx("Key Info", ImGuiTreeNodeFlags_None);
		if (Window_InputInfo.bTreeNode_KeyInfo)
		{
			static std::vector<EKey> PressedKeys;
			const std::size_t PressedKeyCount = LInput::GetPressedKeys(PressedKeys);
			const std::string Title = std::format("Pressed Keys: {}", PressedKeyCount);
			if (ImGui::BeginTable(Title.c_str(), 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit))
			{
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

			ImGui::TreePop();
		}

		ImGui::End();
	}

	void LToolsPanel::UI_UserInterfaceTools()
	{
		ImGui::Begin("UI Tools", &Window_UserInterfaceTools.bOpen);

		/* TODO: Serialize the 'Open-by-default'-logic to load the editor in the same state 
		 *       it was closed in. */
		if (UI::BeginTreeNode("Message Boxes", Window_UserInterfaceTools.bTreeNode_MessageBoxes))
		{
			static constexpr float TitleWidth = 148.0f;

			ImGui::Text("Message Boxes: %d", UI::Internal::MessageBoxes.size());

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
					ImGui::Checkbox("Cancel Button", &bCancelButton);
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
						BoxFlags |= EMessageBoxFlag::OkButton;
					}
					if (bCancelButton)
					{
						BoxFlags |= EMessageBoxFlag::CancelButton;
					}
					if (bAutoResize)
					{
						BoxFlags |= EMessageBoxFlag::AutoSize;
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


	void LToolsPanel::UI_Fonts()
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
		ImGui::Begin(WindowName.c_str(), &Window_Fonts.bOpen);
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

		LK_UI_DEBUG_WINDOW_ON_HOVER();
		ImGui::End();
	}

	void LToolsPanel::UI_InputInfo_SceneSelection(const LVector2& Size)
	{
		static constexpr ImGuiTableFlags TableFlags = ImGuiTableFlags_Borders 
			| ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit;

		static const std::string Title = "Scene Selection";
		if (ImGui::BeginTable(Title.c_str(), 1, TableFlags, Size.As<ImVec2>()))
		{
			const auto& Selection = LSelectionContext::GetSelected(ESelectionContext::Scene);

			ImGui::TableSetupColumn("Scene", ImGuiTableColumnFlags_WidthStretch, 120.0f);
			ImGui::TableHeadersRow();

			const int TableRows = static_cast<int>(Selection.size());
			if (TableRows > 0)
			{
				for (int SelectionIdx = 0; SelectionIdx < (int)Selection.size(); SelectionIdx++)
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);

					const std::string UuidString = std::format("{}", Selection.at(SelectionIdx));
					ImGui::Text("%s", UuidString.c_str());
				}
			}
			else
			{
				/* Add an empty row if no selections exist. */
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("");
			}

			ImGui::EndTable();
		}
	}

	void LToolsPanel::UI_InputInfo_ContentBrowserSelection(const LVector2& Size)
	{
		static constexpr ImGuiTableFlags TableFlags = ImGuiTableFlags_Borders 
			| ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit;

		static const std::string Title = "ContentBrowser Selection";
		if (ImGui::BeginTable(Title.c_str(), 1, TableFlags, Size.As<ImVec2>()))
		{
			const auto& Selection = LSelectionContext::GetSelected(ESelectionContext::ContentBrowser);

			LContentBrowser& ContentBrowser = LContentBrowser::Get();
			FContentBrowserItemList& CurrentBrowserItems = ContentBrowser.GetCurrentItems();

			ImGui::TableSetupColumn("Content Browser", ImGuiTableColumnFlags_WidthStretch, 120.0f);
			ImGui::TableHeadersRow();

			const int TableRows = static_cast<int>(Selection.size());
			if (TableRows > 0)
			{
				for (int SelectionIdx = 0; SelectionIdx < (int)Selection.size(); SelectionIdx++)
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);

					const LUUID& SelectedUuid = Selection.at(SelectionIdx);
					const std::size_t ItemIdx = CurrentBrowserItems.Find(SelectedUuid);
					LK_CORE_ASSERT(ItemIdx != FContentBrowserItemList::InvalidItem, "Invalid item index of UUID: '{}'", SelectedUuid);
					TObjectPtr<LContentBrowserItem> BrowserItem = CurrentBrowserItems[ItemIdx];
					LK_CORE_ASSERT(BrowserItem);
					ImGui::Text("%s", BrowserItem->GetName().c_str());
				}
			}
			else
			{
				/* Add an empty row if no selections exist. */
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("");
			}

			ImGui::EndTable();
		}
	}

	void LToolsPanel::UI_Sandbox()
	{
		static constexpr ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_None;
		ImGui::Begin("Sandbox", &Window_Sandbox.bOpen, WindowFlags);

		ImGuiStyle& Style = ImGui::GetStyle();

		UI::Font::Push("Title");
		ImGui::Text("Sandbox");
		UI::Font::Pop();

		static std::array<const char*, 3> Options = { "Payload1", "Payload2", "Payload3" };
		static int32_t Selected = 0;

		if (UI::PropertyDropdown("Payload", Options.data(), Options.size(), &Selected, "Type of payload"))
		{
			LK_CORE_INFO("Dropdown->Payload: {}", (int)Selected);
		}

		static float ButtonSizeX = 84.0f;
		static float ButtonSizeY = 52.0f;
		static float ButtonOffsetY = 0.0f;
		{
			ImGui::Text("Button Size X");
			ImGui::SameLine(0, 10.0f);
			ImGui::SetNextItemWidth(90.0f);
			UI::Draw::DragFloat("##ButtonSizeX", &ButtonSizeX, 1.0f, 10.0f, 300.0f);

			ImGui::Text("Button Size Y");
			ImGui::SameLine(0, 10.0f);
			ImGui::SetNextItemWidth(90.0f);
			UI::Draw::DragFloat("##ButtonSizeY", &ButtonSizeY, 1.0f, 10.0f, 300.0f);

			ImGui::Text("Button Offset Y");
			ImGui::SameLine(0, 10.0f);
			ImGui::SetNextItemWidth(90.0f);
			UI::Draw::DragFloat("##ButtonOffsetY", &ButtonOffsetY, 0.10f, -50.0f, 50.0f);
		}

		/* Configuration. */
		{
			static bool bPortOpen = false;

			UI::Font::Push("Large");
			ImGui::Text("Configuration");
			UI::Font::Pop();
			UI::Draw::Underline();

			UI::BeginPropertyGrid(nullptr, ImVec2(230.0f, 0.0f), ImGuiTableFlags_SizingStretchProp);

			UI::FScopedStyle FrameRounding(ImGuiStyleVar_FrameRounding, 7);
			{
				if (UI::Property("Enabled", bPortOpen))
				{
					LK_CORE_CONSOLE_INFO("Port is {}", (bPortOpen ? "open" : "closed"));
				}
			}

			static std::array<const char*, 3> PortOptions = { "/dev/ttyPORT1", "/dev/ttyPORT2", "/dev/ttyPORT3" };
			static int32_t PortSelection = 0;
			UI::FScopedColor FrameBg(ImGuiCol_FrameBg, RGBA32::Compliment);
			if (UI::PropertyDropdown("Port", PortOptions.data(), PortOptions.size(), &PortSelection, "Target port"))
			{
				LK_CORE_INFO("Dropdown->Port: {}", (int)PortSelection);
			}

			UI::EndPropertyGrid();
		}

		/* Console. */
		{
			ImVec2 ContentAvail = ImGui::GetContentRegionAvail();
			//ContentAvail.y -= 200.0f;

			static constexpr float RowHeight = 26.0f;
			static constexpr float OffsetDistY = 6.0f;
			static constexpr float TextPadX = 6.0f;

			static const ImVec4 Color1 = ImVec4(0.961f, 0.173f, 0.659f, 1.0f);
			static const ImVec4 Color2 = ImVec4(0.00f, 0.45f, 1.0f, 1.0f);
			static const ImVec4 Color3 = ImVec4(1.00f, 0.90f, 0.060f, 1.0f);
			static const ImVec4 Color4 = ImVec4(1.00f, 0.32f, 0.30f, 1.0f);

			static std::array<const char*, 4> Columns = { "Payload", "Type", "Timestamp", "Data" };
			UI::Table("Console", Columns.data(), Columns.size(), ImVec2(ContentAvail.x, 280.0f), [&]()
			{
				const bool RowClicked = UI::TableRowClickable(Options.at(Selected), 26.0f);

				/* Column: Payload */
				UI::ShiftCursorY(OffsetDistY);
				UI::Separator(ImVec2(TextPadX, RowHeight - 2 * OffsetDistY), Color1);
				ImGui::SameLine();
				UI::ShiftCursorY(-OffsetDistY);
				ImGui::Text("%s", Options.at(Selected));

				/* Column: Type */
				ImGui::TableNextColumn();
				UI::ShiftCursorX(TextPadX);
				ImGui::Text("%d", (int)Selected);

				/* Column: Timestamp */
				ImGui::TableNextColumn();
				UI::ShiftCursorX(TextPadX);
				static constexpr const char* TimeFormat = "%H:%M:%S";
				std::time_t TimeNow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
				std::stringstream TimeString;
				TimeString << std::put_time(std::localtime(&TimeNow), TimeFormat);
				ImGui::Text(TimeString.str().c_str());

				/* Column: Data */
				ImGui::TableNextColumn();
				UI::ShiftCursorX(TextPadX);
				ImGui::Text("%0X", 16);
			});
		}

		{
			static bool bRadioButton = false;
			static constexpr float SpacingX = 5.0f;

			if (ImGui::RadioButton("Radio Button", bRadioButton))
			{
				bRadioButton = !bRadioButton;
			}

			static std::array<const char*, 5> Types = { "Type1", "Type2", "Type3", "Type4", "Type5" };
			static int32_t SelectedType = 0;
			static int32_t SelectedType2 = 0;

			ImGui::Text("Creator");
			/* TODO: Need to fix so PushAligned counts the performed alignments. */
			ImGui::Dummy(ImVec2(0, 0));
			UI::PushAligned(SpacingX);
			if (UI::PropertyDropdown("##Types", Types.data(), Types.size(), &SelectedType, "", 160))
			{
				LK_CORE_INFO("Dropdown->Types: {}", SelectedType);
			}
			if (UI::PropertyDropdown("##Types2", Types.data(), Types.size(), &SelectedType2, "", 160))
			{
				LK_CORE_INFO("Dropdown->Types2: {}", SelectedType2);
			}
			UI::PopAligned();
		}

		{
			const ImVec2 ContentAvail = ImGui::GetContentRegionAvail();
			ImGui::Text("Content: (%.2f, %.2f)", ContentAvail.x, ContentAvail.y);
			const ImVec2 ButtonSize(static_cast<int>(ButtonSizeX), static_cast<int>(ButtonSizeY));

			static constexpr int Buttons = 2;
			static constexpr float ButtonPadding = 10.0f;

			UI::ShiftCursor((ContentAvail.x - (Buttons * ButtonSize.x) - Style.FramePadding.x - (ButtonPadding)), 
							(ContentAvail.y - (1.75f * ButtonSize.y) - Style.FramePadding.y - ButtonOffsetY));

			/* Cancel button. */
			{
				UI::FScopedColor ButtonActive(ImGuiCol_ButtonActive, RGBA32::Text::Error);
				UI::FScopedColor ButtonHovered(ImGuiCol_ButtonHovered, RGBA32::SelectionMuted);
				UI::FScopedStyle FrameRounding(ImGuiStyleVar_FrameRounding, 10);
				if (ImGui::Button("Cancel", ButtonSize))
				{
					LK_CORE_CONSOLE_WARN("Cancel: {}", Options.at(Selected));
				}
			}

			ImGui::SameLine(0, ButtonPadding);

			/* Send button. */
			{
				UI::FScopedColor ButtonColor(ImGuiCol_Button, RGBA32::Compliment);
				UI::FScopedColor ButtonActive(ImGuiCol_ButtonActive, RGBA32::BrightGreen);
				UI::FScopedColor ButtonHovered(ImGuiCol_ButtonHovered, RGBA32::Highlight);
				UI::FScopedStyle FrameRounding(ImGuiStyleVar_FrameRounding, 10);
				if (ImGui::Button("Send", ButtonSize))
				{
					LK_CORE_CONSOLE_INFO("Send: {}", Options.at(Selected));
				}
			}
		}

		ImGui::End();
	}

}
