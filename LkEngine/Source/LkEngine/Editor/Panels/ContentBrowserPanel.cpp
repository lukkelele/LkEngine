#include "LKpch.h"
#include "ContentBrowserPanel.h"

#include "LkEngine/Core/Window.h"
#include "LkEngine/Core/IO/FileSystem.h"
#include "LkEngine/UI/UICore.h"
#include "LkEngine/UI/ImGui/ImGuiWidgets.h"

#include "LkEngine/Renderer/Image.h"
#include "LkEngine/Renderer/Texture.h"

#include "LkEngine/Editor/EditorLayer.h"

#include "LkEngine/Asset/AssetImporter.h"
#include "LkEngine/Asset/AssetManager.h"
#include "LkEngine/Project/Project.h"
#include "LkEngine/Serialization/Serializer.h"

#define LK_CONTENT_BROWSER_USE_HEIGHT_HACK


namespace LkEngine {

	namespace fs = std::filesystem;

	static bool bActivateSearchWidget = false;

    LContentBrowserPanel::LContentBrowserPanel()
    {
        LOBJECT_REGISTER();
		Instance = this;

		std::memset(SearchBuffer, 0, LContentBrowserItem::INPUT_BUFFER_SIZE);

		/* TODO: Load all file icons here. */
		//AssetIconMap[".lmesh"] = FEditorResources::MeshFileIcon;

		LProject::OnProjectChanged.Add(this, &LContentBrowserPanel::OnProjectChanged);
    }

    void LContentBrowserPanel::Initialize()
    {
		LK_CORE_DEBUG_TAG("ContentBrowser", "Initializing");
    }

	void LContentBrowserPanel::OnRenderUI(bool& IsOpen)
	{
		static constexpr ImGuiTableFlags TableFlags = ImGuiTableFlags_Resizable 
			| ImGuiTableFlags_SizingFixedFit 
			| ImGuiTableFlags_BordersInnerV;

		if (ImGuiWindow* ThisWindow = ImGui::FindWindowByName(LK_UI_CONTENTBROWSER); ThisWindow != nullptr)
		{
			if (ThisWindow->DockNode)
			{
				ThisWindow->DockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton;
			}
		}

		UI::Begin(LK_UI_CONTENTBROWSER, &IsOpen, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.0f, 2.0f));

		if (ImGui::BeginTable(UI::GenerateID(), 2, TableFlags, ImVec2(0.0f, 0.0f)))
		{
			ImGui::TableSetupColumn("Outliner", 0, 300.0f);
			ImGui::TableSetupColumn("Directory", ImGuiTableColumnFlags_WidthStretch);

			ImGui::TableNextRow();

			/* Content Outliner. */
			ImGui::TableSetColumnIndex(0);
			ImGui::BeginChild("##FoldersCommon");
			{
				if (BaseDirectory)
				{
					std::vector<TObjectPtr<FDirectoryInfo>> DirectoryArray;
					DirectoryArray.reserve(BaseDirectory->SubDirectories.size());
					for (auto& [SubDirHandle, SubDir] : BaseDirectory->SubDirectories)
					{
						DirectoryArray.emplace_back(SubDir);
					}

					std::sort(DirectoryArray.begin(), DirectoryArray.end(), [](const auto& a, const auto& b)
					{
						return (a->FilePath.stem().string() < b->FilePath.stem().string());
					});

					for (auto& ChildDirectory : DirectoryArray)
					{
						RenderDirectoryHierarchy(ChildDirectory);
					}
				}
			}
			ImGui::EndChild();

			/* Directory Content. */
			ImGui::TableSetColumnIndex(1);
			static constexpr float TopBarHeight = 26.0f;
			static constexpr float BottomBarHeight = 32.0f;
			ImGui::BeginChild("##DirectoryStructure", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetWindowHeight() - TopBarHeight - BottomBarHeight));
			{
				RenderTopBar();

				ImGui::BeginChild("##ContentBrowser-FileArea");
				{
					static constexpr float CellPadding = 2.0f;
					static constexpr float PaddingForOutline = 2.0f;
					const float ScrollBarOffset = 20.0f + ImGui::GetStyle().ScrollbarSize;
					const float PanelWidth = ImGui::GetContentRegionAvail().x - ScrollBarOffset;
					const float CellSize = FEditorSettings::Get().ContentBrowserThumbnailSize + CellPadding + PaddingForOutline;

					int ColumnCount = static_cast<int>(PanelWidth / CellSize);
					if (ColumnCount < 1)
					{
						ColumnCount = 1;
					}

					/**
					 * Render items. 
					 * 
					 *  Iterates the current directory and renders every entry. 
					 */
					{
						static constexpr float RowSpacing = 12.0f;
						UI::FScopedStyle Spacing(ImGuiStyleVar_ItemSpacing, ImVec2(PaddingForOutline, RowSpacing));
						ImGui::Columns(ColumnCount, 0, false);
						UI::FScopedStyle Border(ImGuiStyleVar_FrameBorderSize, 0.0f);
						UI::FScopedStyle Padding(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
						RenderItems();
					}
				}
				ImGui::EndChild();

				RenderBottomBar();
			}
			ImGui::EndChild(); /* DirectoryStructure. */

			ImGui::EndTable();
		}

		UI::VSeparator(6.0f);
		ImGui::PopStyleVar(3); /* ItemSpacing, FramePadding, CellPadding. */
		UI::End(); /* LK_UI_CONTENTBROWSER */
	}

	void LContentBrowserPanel::SetSceneContext(const TObjectPtr<LScene> InSceneContext)
	{
		SceneContext = InSceneContext;
	}

	void LContentBrowserPanel::OnProjectChanged(TObjectPtr<LProject> InProject)
	{
		LK_CORE_DEBUG_TAG("ContentBrowserPanel", "OnProjectChanged: {}", InProject->GetName());

		ItemList.Clear();
		Directories.clear();
		BaseDirectory = nullptr;
		CurrentDirectory = nullptr;
		NextDirectory = nullptr;
		PreviousDirectory = nullptr;
		BreadCrumbData.clear();
		LSelectionContext::DeselectAll();

		Project = InProject;

		FAssetHandle BaseDirectoryHandle = ProcessDirectory(Project->GetAssetDirectory().string(), nullptr);
		BaseDirectory = Directories[BaseDirectoryHandle];
		ChangeDirectory(BaseDirectory);

		std::memset(SearchBuffer, 0, LContentBrowserItem::INPUT_BUFFER_SIZE);
	}

	TObjectPtr<FDirectoryInfo> LContentBrowserPanel::GetDirectory(const std::filesystem::path& Filepath) const
	{
		if (Filepath.empty() || (Filepath.string() == "."))
		{
			return BaseDirectory;
		}

		/* TODO: Do not loop through like this. */
		for (const auto& [Handle, Directory] : Directories)
		{
			if (Directory->FilePath == Filepath)
			{
				return Directory;
			}
		}

		return nullptr;
	}

	void LContentBrowserPanel::SerializeToYaml(YAML::Emitter& Out) const
	{
		LK_UNUSED(Out);
	}

	void LContentBrowserPanel::DeserializeFromYaml(const YAML::Node& Data)
	{
		LK_UNUSED(Data);
	}

	void LContentBrowserPanel::Refresh()
	{
		LK_CORE_INFO_TAG("ContentBrowserPanel", "Refresh");
		ItemList.Clear();
		Directories.clear();

		TObjectPtr<FDirectoryInfo> CurrentDirectory = CurrentDirectory;
		const FAssetHandle BaseDirectoryHandle = ProcessDirectory(Project->GetAssetDirectory().string(), nullptr);
		BaseDirectory = Directories[BaseDirectoryHandle];
		CurrentDirectory = GetDirectory(CurrentDirectory->FilePath);

		if (!CurrentDirectory)
		{
			CurrentDirectory = BaseDirectory;
		}

		ChangeDirectory(CurrentDirectory);
	}

	void LContentBrowserPanel::SortItemList()
	{
		auto SortFunc = [](const TObjectPtr<LContentBrowserItem>& Item1, const TObjectPtr<LContentBrowserItem>& Item2)
		{
			if (Item1->GetType() == Item2->GetType())
			{
				return StringUtils::ToLower(Item1->GetName()) < StringUtils::ToLower(Item2->GetName());
			}

			return (Item1->GetType() < Item2->GetType());
		};

		std::sort(ItemList.begin(), ItemList.end(), SortFunc);
	}

	void LContentBrowserPanel::ChangeDirectory(TObjectPtr<FDirectoryInfo> Directory)
	{
		if (!Directory)
		{
			return;
		}
		
		ItemList.Items.clear();

		/* Update breadcrumbs. */
		bUpdateNavigationPath = true;
		
		if (strlen(SearchBuffer) == 0)
		{
			for (auto& [SubDirHandle, SubDir] : Directory->SubDirectories)
			{
				ItemList.Items.push_back(TObjectPtr<LContentBrowserDirectory>::Create(SubDir));
			}

			std::vector<FAssetHandle> InvalidAssets;
			for (FAssetHandle AssetHandle : Directory->Assets)
			{
				FAssetMetadata Metadata = LProject::Current()->GetRuntimeAssetManager()->GetMetadata(AssetHandle);
				
				if (!Metadata.IsValid())
				{
					continue;
				}

				TObjectPtr<LTexture2D> ItemIcon = FEditorResources::FileIcon;
				LK_CORE_ASSERT(ItemIcon, "ItemIcon is nullptr");
			#if 0
				const std::string Extension = Metadata.FilePath.extension().string();
				if (AssetIconMap.find(Extension) != AssetIconMap.end())
				{
					ItemIcon = AssetIconMap[Extension];
				}
			#endif

				ItemList.Items.push_back(TObjectPtr<LContentBrowserAsset>::Create(Metadata, ItemIcon));
			}
		}
		else
		{
			Search(SearchBuffer, Directory, ItemList);
		}

		SortItemList();

		PreviousDirectory = Directory;
		CurrentDirectory = Directory;

		ClearSelectedObjects();
	}

	FAssetHandle LContentBrowserPanel::ProcessDirectory(const fs::path& DirectoryPath, const TObjectPtr<FDirectoryInfo>& ParentDir)
	{
		const auto& Directory = GetDirectory(DirectoryPath);
		if (Directory)
		{
			return Directory->Handle;
		}

		TObjectPtr<FDirectoryInfo> DirectoryInfo = TObjectPtr<FDirectoryInfo>::Create();
		DirectoryInfo->Handle = FAssetHandle();
		DirectoryInfo->Parent = ParentDir;

		if (DirectoryPath == Project->GetAssetDirectory())
		{
			DirectoryInfo->FilePath = "";
		}
		else
		{
			DirectoryInfo->FilePath = fs::relative(DirectoryPath, Project->GetAssetDirectory());
		}

		for (fs::directory_entry Entry : fs::directory_iterator(DirectoryPath))
		{
			if (Entry.is_directory())
			{
				const FAssetHandle SubdirHandle = ProcessDirectory(Entry.path(), DirectoryInfo);
				DirectoryInfo->SubDirectories[SubdirHandle] = Directories[SubdirHandle];
				continue;
			}

			auto AssetManager = LProject::Current()->GetRuntimeAssetManager();
			FAssetMetadata Metadata = AssetManager->GetMetadata(fs::relative(Entry.path(), Project->GetAssetDirectory()));
			if (!Metadata.IsValid())
			{
				const EAssetType AssetType = AssetManager->GetAssetTypeFromPath(Entry.path());
				if (AssetType == EAssetType::None)
				{
					continue;
				}

				Metadata.Handle = AssetManager->ImportAsset(Entry.path());
			}

			/* Asset import failed, continue. */
			if (!Metadata.IsValid())
			{
				LK_CORE_WARN_TAG("ContentBrowserPanel", "Asset import failed for: {}", Entry.path().string());
				continue;
			}

			DirectoryInfo->Assets.push_back(Metadata.Handle);
		}

		Directories[DirectoryInfo->Handle] = DirectoryInfo;
		return DirectoryInfo->Handle;
	}

	void LContentBrowserPanel::Search(const std::string& Query,
									  const TObjectPtr<FDirectoryInfo>& DirectoryInfo, 
									  FContentBrowserItemList& ItemList)
	{
		const std::string QueryLowerCase = StringUtils::ToLower(Query);
		for (auto& [Handle, Subdir] : DirectoryInfo->SubDirectories)
		{
			const std::string SubdirName = Subdir->FilePath.filename().string();
			if (SubdirName.find(QueryLowerCase) != std::string::npos)
			{
				ItemList.Items.push_back(TObjectPtr<LContentBrowserDirectory>::Create(Subdir));
			}

			/* Search recursively. */
			FContentBrowserItemList List;
			Search(Query, Subdir, List);
			ItemList.Items.insert(ItemList.Items.end(), List.Items.begin(), List.Items.end());
		}

		for (auto& AssetHandle : DirectoryInfo->Assets)
		{
			const FAssetMetadata& Asset = LProject::Current()->GetRuntimeAssetManager()->GetMetadata(AssetHandle);
			std::string Filename = StringUtils::ToLower(Asset.FilePath.filename().string());

			if (Filename.find(QueryLowerCase) != std::string::npos)
			{
				TObjectPtr<LTexture2D> AssetTexture = (AssetIconMap.find(Asset.FilePath.extension().string()) != AssetIconMap.end()) 
					? AssetIconMap[Asset.FilePath.extension().string()] 
					: FEditorResources::FileIcon;
				ItemList.Items.push_back(TObjectPtr<LContentBrowserAsset>::Create(Asset, AssetTexture));
			}
		}
		LK_CORE_ERROR_TAG("ContentBrowser", "After Search | Items: {}", ItemList.Items.size());
	}

	void LContentBrowserPanel::OnBrowseForward()
	{
		LK_CORE_DEBUG_TAG("ContentBrowser", "OnBrowseForward");
		LK_CORE_VERIFY(NextDirectory);
		ChangeDirectory(NextDirectory);
	}

	void LContentBrowserPanel::OnBrowseBackward()
	{
		LK_CORE_DEBUG_TAG("ContentBrowser", "OnBrowseBackward");
		LK_CORE_VERIFY(CurrentDirectory && NextDirectory);
		NextDirectory = CurrentDirectory;
		PreviousDirectory = CurrentDirectory->Parent;
		ChangeDirectory(PreviousDirectory);
	}

	void LContentBrowserPanel::ClearSelectedObjects()
	{
		std::vector<LUUID> SelectedItems = LSelectionContext::GetSelected(ESelectionContext::ContentBrowser);
		for (LUUID ItemHandle : SelectedItems)
		{
			const std::size_t Index = ItemList.FindItem(ItemHandle);
			if (Index == FContentBrowserItemList::InvalidItem)
			{
				continue;
			}

			LSelectionContext::Deselect(ESelectionContext::ContentBrowser, ItemHandle);
			if (ItemList[Index]->IsRenaming())
			{
				ItemList[Index]->StopRenaming();
			}
		}
	}

	void LContentBrowserPanel::UpdateDropArea(const TObjectPtr<FDirectoryInfo>& DirectoryInfo)
	{
		LK_CORE_ASSERT(CurrentDirectory, "Failed to update drop area, CurrentDirectory is nullptr");
		if (DirectoryInfo->Handle != CurrentDirectory->Handle && ImGui::BeginDragDropTarget())
		{
			/* TODO: Drag'n'Drop payload names/enums instead of raw strings. */
			const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("AssetPayload");
			if (Payload)
			{
				const uint32_t Count = Payload->DataSize / sizeof(FAssetHandle);
				for (uint32_t i = 0; i < Count; i++)
				{
					const FAssetHandle AssetHandle = *(((FAssetHandle*)Payload->Data) + i);
					const std::size_t Index = ItemList.FindItem(AssetHandle);
					if (Index != FContentBrowserItemList::InvalidItem)
					{
						ItemList[Index]->Move(DirectoryInfo->FilePath);
						ItemList.erase(AssetHandle);
					}
				}
			}

			ImGui::EndDragDropTarget();
		}
	}

	void LContentBrowserPanel::RenderTopBar(const float Height)
	{
		/* Begin child window for the top bar. */
		static constexpr ImGuiChildFlags ChildFlags = ImGuiChildFlags_None;
		static constexpr ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoScrollbar;
		ImGui::BeginChild("##ContentBrowser-TopBar", ImVec2(0, Height), ChildFlags, WindowFlags);

		/* Begin horizontal layout for the top bar. */
		ImGui::BeginGroup();
		{
			const float EdgeOffset = 4.0f;

			/* Navigation buttons. */
			{
				const ImVec2 Spacing(2.0f, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, Spacing);

				auto ContentBrowserButton = [Height](const char* LabelId, const TObjectPtr<LTexture2D>& InIcon) -> bool
				{
					const ImU32 ButtonCol = RGBA32::BackgroundDark;
					const ImU32 ButtonColActive = UI::ColorWithMultipliedValue(RGBA32::BackgroundDark, 0.8f);

					ImGui::PushStyleColor(ImGuiCol_Button, ButtonCol);
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ButtonCol);
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ButtonColActive);

					const float IconSize = std::min(24.0f, Height);
					const float IconPadding = 3.0f;
					const bool Clicked = ImGui::Button(LabelId, ImVec2(IconSize, IconSize));

					/* Draw button image manually. */
					ImGui::SameLine();
					UI::DrawButtonImage(
						InIcon,
						RGBA32::Text::Darker,
						UI::ColorWithMultipliedValue(RGBA32::Text::Darker, 1.20f), /* TintHovered. */
						UI::ColorWithMultipliedValue(RGBA32::Text::Darker, 0.80f), /* TintPressed. */
						UI::RectExpanded(UI::GetItemRect(), -IconPadding, -IconPadding)
					);

					ImGui::PopStyleColor(3);
					return Clicked;
				};

				ImGui::SameLine();
				if (ContentBrowserButton("##Backward", FEditorResources::BackwardIcon))
				{
					OnBrowseBackward();
				}
				UI::SetTooltip("Previous Directory");

				ImGui::Dummy(ImVec2(EdgeOffset, 0.0f));

				ImGui::SameLine();
				if (ContentBrowserButton("##Forward", FEditorResources::ForwardIcon))
				{
					OnBrowseForward();
				}
				UI::SetTooltip("Next Directory");

				ImGui::Dummy(ImVec2(EdgeOffset * 2.0f, 0.0f));

				ImGui::SameLine();
				if (ContentBrowserButton("##Refresh", FEditorResources::RefreshIcon))
				{
					Refresh();
				}
				UI::SetTooltip("Refresh");

				ImGui::Dummy(ImVec2(EdgeOffset * 2.0f, 0.0f));
				ImGui::PopStyleVar();
			}

			/* Search widget. */
			static constexpr float SearchBarSize = 220.0f;
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);
				if (bActivateSearchWidget)
				{
					ImGui::SetKeyboardFocusHere();
					bActivateSearchWidget = false;
				}

				ImGui::SameLine();
				if (UI::Widget::SearchWidget<LContentBrowserItem::INPUT_BUFFER_SIZE, SearchBarSize>(SearchBuffer))
				{
					LK_CORE_INFO_TAG("ContentBrowser", "SearchWidget");
					if (strlen(SearchBuffer) == 0)
					{
						ChangeDirectory(CurrentDirectory);
					}
					else
					{
						Search(SearchBuffer, CurrentDirectory, ItemList);
						SortItemList();
					}
				}
				LK_UI_DEBUG_BOUNDING_BOX(IM_COL32(255, 50, 255, 255));

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2.0f);
			}

			/* Breadcrumbs. */
			if (bUpdateNavigationPath)
			{
				BreadCrumbData.clear();

				TObjectPtr<FDirectoryInfo> Current = CurrentDirectory;
				while (Current && Current->Parent != nullptr)
				{
					BreadCrumbData.push_back(Current);
					Current = Current->Parent;
				}

				std::reverse(BreadCrumbData.begin(), BreadCrumbData.end());
				bUpdateNavigationPath = false;
			}

			/***********************************************************
			 * Pathing / Breadcrumbs
			 **********************************************************/
			{
				ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
				ImGui::PushStyleColor(ImGuiCol_Text, RGBA32::Text::Darker);

				LK_CORE_ASSERT(Project);
				const std::string& AssetsDirectoryName = fs::relative(Project->GetConfiguration().AssetDirectory).string();
				const ImVec2 TextSize = ImGui::CalcTextSize(AssetsDirectoryName.c_str());
				const float TextPadding = ImGui::GetStyle().FramePadding.y;

				ImGui::SameLine();
				if (ImGui::Selectable(AssetsDirectoryName.c_str(), false, 0, ImVec2(TextSize.x, TextSize.y + TextPadding)))
				{
					LK_CORE_DEBUG_TAG("ContentBrowser", "Clicked browser path");
					LSelectionContext::DeselectAll(ESelectionContext::ContentBrowser);
					ChangeDirectory(BaseDirectory);
				}

				UpdateDropArea(BaseDirectory);

				for (const auto& Directory : BreadCrumbData)
				{
					ImGui::SameLine();
					ImGui::Text("/");

					const std::string DirectoryName = Directory->FilePath.filename().string();
					const ImVec2 TextSize = ImGui::CalcTextSize(DirectoryName.c_str());
					ImGui::SameLine();
					if (ImGui::Selectable(DirectoryName.c_str(), false, 0, ImVec2(TextSize.x, TextSize.y + TextPadding)))
					{
						LSelectionContext::DeselectAll(ESelectionContext::ContentBrowser);
						ChangeDirectory(Directory);
					}

					UpdateDropArea(Directory);
				}

				ImGui::PopStyleColor();
				ImGui::PopFont();
			}

			/* Settings button, right aligned. */
			ImGui::SameLine();
			ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x - 40.0f, 0.0f));
			ImGui::SameLine();
			if (UI::Widget::OptionsButton())
			{
				ImGui::OpenPopup("ContentBrowserSettings");
			}

			UI::SetTooltip("Content Browser Settings");
			if (ImGui::BeginPopup("ContentBrowserSettings"))
			{
				auto& EditorSettings = FEditorSettings::Get();
				int* ContentBrowserThumbnailSizePtr = reinterpret_cast<int*>(&EditorSettings.ContentBrowserThumbnailSize);

				bool SaveSettings = ImGui::MenuItem("Show Asset Types", nullptr, &EditorSettings.bContentBrowserShowAssetTypes);
				SaveSettings |= ImGui::SliderInt("##ThumbnailSize", ContentBrowserThumbnailSizePtr, 96, 512);
				UI::SetTooltip("Thumbnail Size");

				if (SaveSettings)
				{
					FEditorSettings::Get().Save();
				}

				ImGui::EndPopup();
			}
		}
		ImGui::EndGroup();

		ImGui::EndChild(); /* ContentBrowser-TopBar */
	}

	void LContentBrowserPanel::RenderBottomBar(const float Height)
	{
	}

	void LContentBrowserPanel::RenderItems()
	{
		bIsAnyItemHovered = false;

		for (TObjectPtr<LContentBrowserItem>& Item : ItemList)
		{
			Item->BeginRender();

			/* Determine action. */
			const FContentBrowserItemAction ItemAction = Item->Render();

			/* Action: Select. */
			if (ItemAction.Field & EContentBrowserAction::Selected)
			{
				LK_CORE_DEBUG_TAG("ContentBrowser", "Selected item: {}", Item->GetName());
			}
			/* Action: Deselect. */
			if (ItemAction.Field & EContentBrowserAction::Deselected)
			{
				LK_CORE_DEBUG_TAG("ContentBrowser", "Deselected item: {}", Item->GetName());
			}
			/* Action: Show In Explorer. */
			if (ItemAction.Field & EContentBrowserAction::ShowInExplorer)
			{
				LK_CORE_DEBUG_TAG("ContentBrowser", "Show in explorer: {}", Item->GetName());
				LK_CORE_ASSERT(Project && CurrentDirectory);
				LFileSystem::ShowFileInExplorer(Project->GetAssetDirectory() / CurrentDirectory->FilePath / Item->GetName());
			}
			/* Action: Start Renaming. */
			if (ItemAction.Field & EContentBrowserAction::StartRenaming)
			{
				LK_CORE_DEBUG_TAG("ContentBrowser", "Start renaming: {}", Item->GetName());
			}
			/* Action: Reload. */
			if (ItemAction.Field & EContentBrowserAction::Reload)
			{
				LK_CORE_DEBUG_TAG("ContentBrowser", "Reload");
			}
			/* Action: Hovered. */
			if (ItemAction.Field & EContentBrowserAction::Hovered)
			{
				//LK_CORE_DEBUG_TAG("ContentBrowser", "Hovered: {}", Item->GetName());
			}

			Item->EndRender();

			/* Action: Renamed. */
			if (ItemAction.Field & EContentBrowserAction::Renamed)
			{
				LK_CORE_DEBUG_TAG("ContentBrowser", "Renamed: {}", Item->GetName());
				break;
			}

			/* Action: Activated. */
			if (ItemAction.Field & EContentBrowserAction::Activated)
			{
				LK_CORE_DEBUG_TAG("ContentBrowser", "Activated: {}", Item->GetName());
				if (Item->GetType() == LContentBrowserItem::EItemType::Directory)
				{
					LK_CORE_DEBUG_TAG("ContentBrowser", "Entering directory: {}", Item->GetName());
					LSelectionContext::DeselectAll(ESelectionContext::ContentBrowser);
					ChangeDirectory(Item.As<LContentBrowserDirectory>()->GetDirectoryInfo());
					break;
				}
			}

			/// TODO: Finish the rest of the browser actions.
		}
	}

	void LContentBrowserPanel::RenderDirectoryHierarchy(TObjectPtr<FDirectoryInfo> Directory)
	{
		const std::string Name = Directory->FilePath.filename().string();
		const std::string ID = Name + "_TreeNode";
		const bool PreviousState = ImGui::TreeNodeBehaviorIsOpen(ImGui::GetID(ID.c_str()));

		ImGuiWindow* Window = ImGui::GetCurrentWindow();

	#if defined(LK_CONTENT_BROWSER_USE_HEIGHT_HACK)
		/* ImGui height hack. */
		Window->DC.CurrLineSize.y = 20.0f;
		Window->DC.CurrLineTextBaseOffset = 3.0f;
	#endif
		const ImRect ItemRect = { 
			Window->WorkRect.Min.x, Window->DC.CursorPos.y, 
			Window->WorkRect.Max.x, Window->DC.CursorPos.y + Window->DC.CurrLineSize.y 
		};

		const bool IsItemClicked = [&ItemRect, &ID]
		{
			if (ImGui::ItemHoverable(ItemRect, ImGui::GetID(ID.c_str()), ImGuiItemFlags_None))
			{
				return ImGui::IsMouseDown(ImGuiMouseButton_Left) || ImGui::IsMouseReleased(ImGuiMouseButton_Left);
			}
			return false;
		}();

		const bool IsWindowFocused = ImGui::IsWindowFocused();

		/* Fill with selection color if any of the child entities are selected. */
		auto CheckIfAnyDescendantSelected = [&](TObjectPtr<FDirectoryInfo>& InDirectory, auto InIsAnyDescendantSelected) -> bool
		{
			if (InDirectory->Handle == CurrentDirectory->Handle)
			{
				return true;
			}

			for (auto& [ChildHandle, ChildDir] : InDirectory->SubDirectories)
			{
				if (InIsAnyDescendantSelected(ChildDir, InIsAnyDescendantSelected))
				{
					return true;
				}
			}

			return false;
		};

		const bool IsAnyDescendantSelected = CheckIfAnyDescendantSelected(Directory, CheckIfAnyDescendantSelected);
		const bool IsActiveDirectory = (Directory->Handle == CurrentDirectory->Handle);

		auto FillWithColor = [&](const ImColor& color)
		{
			const ImU32 BgColor = ImGui::ColorConvertFloat4ToU32(color);
			ImGui::GetWindowDrawList()->AddRectFilled(ItemRect.Min, ItemRect.Max, BgColor);
		};

		ImGuiTreeNodeFlags Flags = (IsActiveDirectory ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_SpanFullWidth;

		/* Fill the background. */
		if (IsActiveDirectory || IsItemClicked)
		{
			if (IsWindowFocused)
			{
				FillWithColor(RGBA32::Selection);
			}
			else
			{
				const ImColor Color = UI::ColorWithMultipliedValue(RGBA32::Selection, 0.80f);
				FillWithColor(UI::ColorWithMultipliedSaturation(Color, 0.70f));
			}

			ImGui::PushStyleColor(ImGuiCol_Text, RGBA32::BackgroundDark);
		}
		else if (IsAnyDescendantSelected)
		{
			FillWithColor(RGBA32::SelectionMuted);
		}

		/* Tree Node. */
		const bool IsOpen = UI::TreeNode(ID, Name, Flags, FEditorResources::FolderIcon);
		if (IsActiveDirectory || IsItemClicked)
		{
			ImGui::PopStyleColor();
		}

		/* Fix for slight overlap. */
		UI::ShiftCursorY(3.0f);

		/* Context Menu. */
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 4.0f));
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::BeginMenu("New"))
			{
				if (ImGui::MenuItem("Folder"))
				{
					const bool Created = LFileSystem::CreateDirectory(LFileSystem::GetAssetsDir() / Directory->FilePath / "New Folder");
					if (Created)
					{
						Refresh();
					}
				}

				if (ImGui::MenuItem("Scene"))
				{
					LK_CORE_FATAL("CreateAssetInDirectory -> Scene");
					//CreateAssetInDirectory<Scene>("New Scene.lkscene", Directory);
				}

				if (ImGui::MenuItem("Material"))
				{
					LK_CORE_FATAL("CreateAssetInDirectory -> Material");
					//CreateAssetInDirectory<LMaterialAsset>("New Material.lmat", Directory);
				}

				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Show in Explorer"))
			{
				LFileSystem::OpenDirectoryInExplorer(LFileSystem::GetAssetsDir() / Directory->FilePath);
			}

			ImGui::EndPopup();
		}
		ImGui::PopStyleVar(); /* ItemSpacing */

		/* Draw children directories. */
		if (IsOpen)
		{
			std::vector<TObjectPtr<FDirectoryInfo>> DirectoryArray;
			DirectoryArray.reserve(BaseDirectory->SubDirectories.size());
			for (auto& [SubDirHandle, SubDir] : Directory->SubDirectories)
			{
				DirectoryArray.emplace_back(SubDir);
			}

			std::sort(DirectoryArray.begin(), DirectoryArray.end(), [](const auto& a, const auto& b)
			{
				return (a->FilePath.stem().string() < b->FilePath.stem().string());
			});

			for (auto& ChildDirectory : DirectoryArray)
			{
				RenderDirectoryHierarchy(ChildDirectory);
			}
		}

		UpdateDropArea(Directory);

		if (IsOpen != (PreviousState && !IsActiveDirectory))
		{
			if (!ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.01f))
			{
				ChangeDirectory(Directory);
			}
		}

		if (IsOpen)
		{
			ImGui::TreePop();
		}
	}

}
