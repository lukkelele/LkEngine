#include "ContentBrowserPanel.h"

#include <LkEngine/Core/Window.h>
#include <LkEngine/Core/Input/Input.h>
#include <LkEngine/Core/IO/FileSystem.h>
#include <LkEngine/Renderer/Image.h>
#include <LkEngine/Renderer/Texture.h>
#include <LkEngine/Renderer/UI/UICore.h>
#include <LkEngine/Renderer/UI/Widgets.h>
#include <LkEngine/Asset/AssetImporter.h>
#include <LkEngine/Asset/AssetManager.h>

#include "LkEditor/AssetEditor/AssetEditorManager.h"

#include "LkEngine/Project/Project.h"
#include "LkEngine/Serialization/Serializer.h"

#include "LkEditor/EditorLayer.h"

#define LK_CONTENT_BROWSER_USE_HEIGHT_HACK 1

namespace LkEngine {

	namespace fs = std::filesystem;

	static LContentBrowserPanel* Instance = nullptr;

	static bool bActivateSearchWidget = false;

    LContentBrowserPanel::LContentBrowserPanel()
    {
        LOBJECT_REGISTER();

		/**
		 * The reason for using this type of singleton instead of Meyer's
		 * is because the LPanelManager is the creator of the LContentBrowser instance.
		 */
		Instance = this;

		std::memset(SearchBuffer, 0, LContentBrowserItem::INPUT_BUFFER_SIZE);

		LProject::OnProjectChanged.Add(this, &LContentBrowserPanel::OnProjectChanged);
    }

    void LContentBrowserPanel::Initialize()
    {
		LK_CORE_TRACE_TAG("ContentBrowser", "Initializing");
    }

	LContentBrowserPanel& LContentBrowserPanel::Get()
	{ 
		return *Instance; 
	}

	void LContentBrowserPanel::RenderUI(bool& IsOpen)
	{
		/* Handle dock window. */
		if (ImGuiWindow* ThisWindow = ImGui::FindWindowByName(PanelID::ContentBrowser); ThisWindow != nullptr)
		{
			if (ThisWindow->DockNode)
			{
				ThisWindow->DockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton;
			}
		}

		static constexpr ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoScrollbar 
			| ImGuiWindowFlags_NoScrollWithMouse
			| ImGuiWindowFlags_NoNavFocus;

		ImGui::Begin(PanelID::ContentBrowser, &IsOpen, WindowFlags);
		bIsContentBrowserHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.0f, 6.0f));

		static constexpr ImGuiTableFlags TableFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV;
		//UI::PushID();
		if (ImGui::BeginTable(UI::GenerateID(), 2, TableFlags, ImVec2(0.0f, 0.0f)))
		{
			ImGui::TableSetupColumn("Outliner", 0, 300.0f);
			ImGui::TableSetupColumn("Directory Structure", ImGuiTableColumnFlags_WidthStretch);

			ImGui::TableNextRow();

			/* Content Outliner. */
			ImGui::TableSetColumnIndex(0);
			ImGui::BeginChild("##OutlinerFolders");
			{
				if (BaseDirectory)
				{
					std::vector<TObjectPtr<FDirectoryInfo>> DirectoryArray; // TODO: Should not be needed to be recreated every tick.
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
						RenderOutlinerHierarchy(ChildDirectory);
					}
				}
			}
			ImGui::EndChild();

			/* Directory Content. */
			ImGui::TableSetColumnIndex(1);
			static constexpr float TopBarHeight = 26.0f;
			static constexpr float BottomBarHeight = 32.0f;
			ImGui::BeginChild("##ContentBrowser-DirectoryStructure", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetWindowHeight() - TopBarHeight - BottomBarHeight));
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
					 * Iterates the content of the current directory and renders every browser item. 
					 */
					{
						static constexpr float RowSpacing = 12.0f;
						UI::FScopedStyle Spacing(ImGuiStyleVar_ItemSpacing, ImVec2(PaddingForOutline, RowSpacing));
						ImGui::Columns(ColumnCount, 0, false);
						UI::FScopedStyle Border(ImGuiStyleVar_FrameBorderSize, 0.0f);
						UI::FScopedStyle Padding(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
						RenderItems();
					}

					if (ImGui::IsWindowFocused() && !ImGui::IsMouseDragging(ImGuiMouseButton_Left))
					{
						UpdateInput();
					}

				}
				ImGui::EndChild();

				RenderBottomBar();
			}
			ImGui::EndChild(); // DirectoryStructure.

			ImGui::EndTable();
		}

		UI::VSeparator(3.0f);
		ImGui::PopStyleVar(3); // ItemSpacing, FramePadding, CellPadding.

		//UI::End(); // PanelID::ContentBrowser
		ImGui::End(); // PanelID::ContentBrowser
	}

	void LContentBrowserPanel::SetSceneContext(const TObjectPtr<LScene> InSceneContext)
	{
		SceneContext = InSceneContext;
	}

	void LContentBrowserPanel::OnProjectChanged(const TObjectPtr<LProject>& InProject)
	{
		LK_CORE_DEBUG_TAG("ContentBrowser", "OnProjectChanged: {}", InProject ? InProject->GetConfiguration().Name : "None");
		ItemList.Clear();
		Directories.clear();
		BaseDirectory = nullptr;
		CurrentDirectory = nullptr;
		NextDirectory = nullptr;
		PreviousDirectory = nullptr;
		BreadCrumbData.clear();
		LSelectionContext::DeselectAll();

		Project = InProject;

		const std::filesystem::path AssetDir = Project->GetAssetDirectory();
		if (!std::filesystem::exists(AssetDir))
		{
			LK_CORE_ERROR_TAG("ContentBrowser", "[OnProjectChanged] Asset directory does not exist: {}", AssetDir.string());
			return;
		}

		FAssetHandle BaseDirectoryHandle = ProcessDirectory(AssetDir.string(), nullptr);
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

	void LContentBrowserPanel::UpdateInput()
	{
		if (!bIsContentBrowserHovered)
		{
			return;
		}

		if ((!bIsAnyItemHovered && ImGui::IsMouseDown(ImGuiMouseButton_Left)) || LInput::IsKeyDown(EKey::Escape))
		{
			ClearCurrentSelection();
		}

		if (LInput::IsKeyDown(EKey::F5))
		{
			Refresh();
		}
	}

	void LContentBrowserPanel::Refresh()
	{
		LK_CORE_INFO_TAG("ContentBrowser", "Refresh");
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
			InvalidAssets.reserve(Directory->Assets.size());
			for (FAssetHandle AssetHandle : Directory->Assets)
			{
				FAssetMetadata Metadata = LProject::Current()->GetAssetManager()->GetMetadata(AssetHandle);
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

		ClearCurrentSelection();
	}

	LUUID LContentBrowserPanel::ProcessDirectory(const fs::path& DirectoryPath, const TObjectPtr<FDirectoryInfo>& ParentDir)
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

			auto AssetManager = LProject::Current()->GetAssetManager();
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
				LK_CORE_WARN_TAG("ContentBrowser", "Asset import failed for: {}", Entry.path().string());
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
			const FAssetMetadata& Asset = LProject::Current()->GetAssetManager()->GetMetadata(AssetHandle);
			const std::string Filename = StringUtils::ToLower(Asset.FilePath.filename().string());

			if (Filename.find(QueryLowerCase) != std::string::npos)
			{
				TObjectPtr<LTexture2D> AssetTexture;
				if (AssetIconMap.find(Asset.FilePath.extension().string()) != AssetIconMap.end())
				{
					AssetIconMap[Asset.FilePath.extension().string()];
				}
				else
				{ 
					AssetTexture = FEditorResources::FileIcon;
				}

				ItemList.Items.push_back(TObjectPtr<LContentBrowserAsset>::Create(Asset, AssetTexture));
			}
		}
		LK_CORE_ERROR_TAG("ContentBrowser", "After Search | Items: {}", ItemList.Items.size());
	}

	void LContentBrowserPanel::OnBrowseForward()
	{
		LK_CORE_TRACE_TAG("ContentBrowser", "OnBrowseForward");
		LK_CORE_VERIFY(NextDirectory);
		ChangeDirectory(NextDirectory);
	}

	void LContentBrowserPanel::OnBrowseBackward()
	{
		LK_CORE_TRACE_TAG("ContentBrowser", "OnBrowseBackward");
		LK_CORE_VERIFY(CurrentDirectory && NextDirectory);
		NextDirectory = CurrentDirectory;
		PreviousDirectory = CurrentDirectory->Parent;
		ChangeDirectory(PreviousDirectory);
	}

	void LContentBrowserPanel::ClearCurrentSelection()
	{
		//LK_CORE_INFO_TAG("ContentBrowser", "Clearing current selection");
		std::vector<FAssetHandle> SelectedItems = LSelectionContext::GetSelected(ESelectionContext::ContentBrowser);
		for (const FAssetHandle ItemHandle : SelectedItems)
		{
			const std::size_t Index = ItemList.Find(ItemHandle);
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
			const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload(UI::DragDropPayload::Asset);
			if (Payload)
			{
				const uint32_t Count = Payload->DataSize / sizeof(FAssetHandle);
				for (uint32_t i = 0; i < Count; i++)
				{
					const FAssetHandle AssetHandle = *(((FAssetHandle*)Payload->Data) + i);
					const std::size_t Index = ItemList.Find(AssetHandle);
					if (Index != FContentBrowserItemList::InvalidItem)
					{
						ItemList[Index]->Move(DirectoryInfo->FilePath);
						ItemList.Erase(AssetHandle);
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

		/** @fixme: Linux support, need to fix the relative pathing issue. */
		#if defined(LK_PLATFORM_WINDOWS)
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
			#if defined(LK_PLATFORM_WINDOWS)
				const std::string AssetsDirectoryName = std::filesystem::relative(Project->GetConfiguration().AssetDirectory).string();
			#elif defined(LK_PLATFORM_WINDOWS)
				const std::string AssetsDirectoryName = std::filesystem::relative(LFileSystem::GetRuntimeDir() / Project->GetConfiguration().AssetDirectory).string();
				LK_CORE_WARN_TAG("ContentBrowserPanel", "AssetsDirectoryName: {}  (Empty: {})", AssetsDirectoryName, AssetsDirectoryName.empty() ? "Yes" : "No");
			#endif
				//LK_CORE_ASSERT(!AssetsDirectoryName.empty(), "Project '{}' does not have an assigned assets directory in its configuration", Project->GetName());
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
			#endif
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
			const FContentBrowserItemAction ItemAction = Item->Render();

			/**
			 * The ordering of the browser item actions matters.
			 * Any previous selection states need to be taken into account 
			 * and not handled before any deselection actions.
			 */

			if (ItemAction & EContentBrowserAction::ClearSelections)
			{
				LK_DEBUG_CONTENTBROWSER_ITEM_LOG("ClearSelections: {}", Item->GetName());
				ClearCurrentSelection();
			}

			/* Action: Deselect. */
			if (ItemAction & EContentBrowserAction::Deselected)
			{
				LK_DEBUG_CONTENTBROWSER_ITEM_LOG("Deselected item: {}", Item->GetName());
				LSelectionContext::Deselect(ESelectionContext::ContentBrowser, Item->GetID());
			}

			/* Action: Select. */
			if (ItemAction & EContentBrowserAction::Selected)
			{
				LK_DEBUG_CONTENTBROWSER_ITEM_LOG("Selected item: {}", Item->GetName());
				LSelectionContext::Select(ESelectionContext::ContentBrowser, Item->GetID());
			}

			/* Action: SelectToHere. */
			/* Selects multiple items, is only applicable when the item count is 2. 
			 * This is because the selection cannot be calculated otherwise (needs start -> stop). */
			if (ItemAction & EContentBrowserAction::SelectToHere)
			{
				LK_DEBUG_CONTENTBROWSER_ITEM_LOG("SelectToHere: {}  Count={}", Item->GetName(), LSelectionContext::GetSelectionCount(ESelectionContext::ContentBrowser));
				if (LSelectionContext::GetSelectionCount(ESelectionContext::ContentBrowser) == 2)
				{
					std::size_t FirstIdx = ItemList.Find(LSelectionContext::GetSelected(ESelectionContext::ContentBrowser, 0));
					std::size_t LastIdx = ItemList.Find(Item->GetID());

					if (FirstIdx > LastIdx)
					{
						const std::size_t TempCopy = FirstIdx;
						FirstIdx = LastIdx;
						LastIdx = TempCopy;
					}

					LK_DEBUG_CONTENTBROWSER_ITEM_LOG("SelectToHere  FirstIdx={} LastIdx={}", FirstIdx, LastIdx);
					for (std::size_t Idx = FirstIdx; Idx < LastIdx; Idx++)
					{
						LSelectionContext::Select(ESelectionContext::ContentBrowser, ItemList[Idx]->GetID());
					}
				}
			}

			/* Action: Show In Explorer. */
			if (ItemAction & EContentBrowserAction::ShowInExplorer)
			{
				LK_CORE_DEBUG_TAG("ContentBrowser", "Show in explorer: {}", Item->GetName());
				LK_CORE_ASSERT(Project && CurrentDirectory);
				if (Item->GetType() == LContentBrowserItem::EItemType::Directory)
				{
					LFileSystem::ShowFileInExplorer(Project->GetAssetDirectory() / CurrentDirectory->FilePath / Item->GetName());
				}
				else
				{
					TObjectPtr<LEditorAssetManager> EditorAssetManager = Project->GetEditorAssetManager();
					LK_CORE_ASSERT(EditorAssetManager);
					const FAssetMetadata& ItemMetadata = EditorAssetManager->GetMetadata(Item->GetID());
					LFileSystem::ShowFileInExplorer(EditorAssetManager->GetFileSystemPath(ItemMetadata));
				}
			}

			/* Action: Start Renaming. */
			if (ItemAction & EContentBrowserAction::StartRenaming)
			{
				LK_CORE_DEBUG_TAG("ContentBrowser", "Start renaming: {}", Item->GetName());
			}

			/* Action: Reload. */
			if (ItemAction & EContentBrowserAction::Copy)
			{
				LK_CORE_DEBUG_TAG("ContentBrowser", "Copying: {} ({})", Item->GetName(), Item->GetID());
				CopiedAssets.Select(Item->GetID());
			}

			/* Action: Reload. */
			if (ItemAction & EContentBrowserAction::Reload)
			{
				LK_CORE_DEBUG_TAG("ContentBrowser", "Reload asset: {} ({})", Item->GetName(), Item->GetID());
				if (!LAssetManager::ReloadData(Item->GetID()))
				{
					LK_CORE_ERROR_TAG("ContentBrowser", "Failed to reload data for item: {} ({})", Item->GetName(), Item->GetID());
				}
			}

			/* Action: Hovered. */
			if (ItemAction & EContentBrowserAction::Hovered)
			{
				bIsAnyItemHovered = true;
			}

			Item->EndRender();

			/* Action: Renamed. */
			if (ItemAction & EContentBrowserAction::Renamed)
			{
				LK_CORE_DEBUG_TAG("ContentBrowser", "Renamed: {}", Item->GetName());
				break;
			}

			/* Action: Activated. */
			if (ItemAction & EContentBrowserAction::Activated)
			{
				LK_CORE_DEBUG_TAG("ContentBrowser", "Activated: {}", Item->GetName());
				if (Item->GetType() == LContentBrowserItem::EItemType::Directory)
				{
					LK_CORE_DEBUG_TAG("ContentBrowser", "Entering directory: {}", Item->GetName());
					LSelectionContext::DeselectAll(ESelectionContext::ContentBrowser);

					/* Required to break here as we invalidate the current
					   iteration of items whenever a new directory is entered. */
					ChangeDirectory(Item.As<LContentBrowserDirectory>()->GetDirectoryInfo());
					break;
				}
				else
				{
					/* TODO: Open editor of some sort, based on the file type. */
					auto AssetItem = Item.As<LContentBrowserAsset>();
					const FAssetMetadata& AssetMetadata = AssetItem->GetAssetMetadata();
					LAssetEditorManager::OpenEditor(LAssetManager::GetAsset<LAsset>(AssetMetadata.Handle));
				}
			}

			/* Action: Refresh. */
			if (ItemAction & EContentBrowserAction::Refresh)
			{
				LK_CORE_DEBUG_TAG("ContentBrowser", "Refresh: {}", Item->GetName());
				Refresh();
				break;
			}
		}
	}

	void LContentBrowserPanel::RenderOutlinerHierarchy(TObjectPtr<FDirectoryInfo> Directory)
	{
		const std::string Name = Directory->FilePath.filename().string();
		const std::string ID = Name + "_TreeNode";
		const bool PreviousState = ImGui::TreeNodeBehaviorIsOpen(ImGui::GetID(ID.c_str()));

		ImGuiWindow* Window = ImGui::GetCurrentWindow();
		const bool IsWindowFocused = ImGui::IsWindowFocused();

	#if defined(LK_CONTENT_BROWSER_USE_HEIGHT_HACK)
		// ImGui height hack.
		Window->DC.CurrLineSize.y = 20.0f;
		Window->DC.CurrLineTextBaseOffset = 3.0f;
	#endif
		/// FIXME: The previously selected nodes that are in lower depths are not marked 
		/// when DC.CurrLineSize.y is not added.
		const ImRect ItemRect = { 
			Window->WorkRect.Min.x, Window->DC.CursorPos.y,
			Window->WorkRect.Max.x, Window->DC.CursorPos.y
			//Window->WorkRect.Max.x, Window->DC.CursorPos.y + ImGui::GetCurrentContext()->FontSize + ImGui::GetStyle().FramePadding.y * 2
			//Window->WorkRect.Max.x, Window->DC.CursorPos.y + Window->DC.CurrLineSize.y 
		};

		const bool IsItemClicked = [&ItemRect, &ID]
		{
			if (ImGui::ItemHoverable(ItemRect, ImGui::GetID(ID.c_str()), ImGuiItemFlags_None))
			{
				return ImGui::IsMouseDown(ImGuiMouseButton_Left) || ImGui::IsMouseReleased(ImGuiMouseButton_Left);
			}
			return false;
		}();

		/* Fill with selection color if any of the child entities are selected. */
		auto CheckIfAnyDescendantSelected = [&](TObjectPtr<FDirectoryInfo>& InDirectory, auto IsAnyDescendantSelectedFunc) -> bool
		{
			if (InDirectory->Handle == CurrentDirectory->Handle)
			{
				return true;
			}

			for (auto& [ChildHandle, ChildDir] : InDirectory->SubDirectories)
			{
				if (IsAnyDescendantSelectedFunc(ChildDir, IsAnyDescendantSelectedFunc))
				{
					return true;
				}
			}

			return false;
		};

		const bool IsAnyDescendantSelected = CheckIfAnyDescendantSelected(Directory, CheckIfAnyDescendantSelected);

		auto DrawNodeBackground = [&](const ImColor& InColor)
		{
			const ImU32 Color = ImGui::ColorConvertFloat4ToU32(InColor);
			ImGui::GetWindowDrawList()->AddRectFilled(ItemRect.Min, ItemRect.Max, Color);
		};

		const bool IsActiveDirectory = (Directory->Handle == CurrentDirectory->Handle);

		ImGuiTreeNodeFlags Flags = (IsActiveDirectory ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_SpanFullWidth;
		//Flags |= ImGuiTreeNodeFlags_Framed;

		/* Fill the background if selected/clicked. */
		if (IsActiveDirectory || IsItemClicked)
		{
			if (IsWindowFocused)
			{
				DrawNodeBackground(RGBA32::Selection);
				//DrawNodeBackground(RGBA32::Text::Error);
			}
			else
			{
				const ImColor Color = UI::ColorWithMultipliedValue(RGBA32::Selection, 0.80f);
				DrawNodeBackground(UI::ColorWithMultipliedSaturation(Color, 0.70f));
			}

			//ImGui::PushStyleColor(ImGuiCol_Text, RGBA32::BackgroundDarker);
			ImGui::PushStyleColor(ImGuiCol_Text, RGBA32::NiceBlue);
		}
		else if (IsAnyDescendantSelected)
		{
			DrawNodeBackground(RGBA32::SelectionMuted);
		}

		/**
		 * Check to see if the any of the subdirectories have subdirs in them. 
		 * We should not render the arrow/bullet if that isn't the case.
		 */
		ImGuiDir OpenDirection = ImGuiDir_None;
		if (Directory && (Directory->SubDirectories.size() > 0))
		{
			OpenDirection = ImGuiDir_Down;
		}

		/**
		 * Render the tree node. 
		 * Whenever the selected entry does not have any subdirectories, do not toggle the arrow down.
		 */
		const bool IsOpen = UI::TreeNode(ID, Name, Flags, FEditorResources::FolderIcon, OpenDirection);

		if (IsActiveDirectory || IsItemClicked)
		{
			ImGui::PopStyleColor();
		}

		UI::ShiftCursorY(3.0f);

		/**
		 * Context Menu. 
		 *
		 *  Show the context menu on right clicks.
		 */
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 4.0f));
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::BeginMenu("New"))
			{
				if (ImGui::MenuItem("Folder"))
				{
					const bool DirectoryCreated = LFileSystem::CreateDirectory(LFileSystem::GetAssetsDir() / Directory->FilePath / "New Folder");
					if (DirectoryCreated)
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
		ImGui::PopStyleVar(); // ItemSpacing.

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
				//UI::FScopedStyle IndentSpacing(ImGuiStyleVar_IndentSpacing, 12.0f);
				RenderOutlinerHierarchy(ChildDirectory);
			}
		}

		UpdateDropArea(Directory);

		//if (IsOpen != (PreviousState && !IsActiveDirectory))
		if (IsOpen != PreviousState)
		{
			if (!ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.010f))
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
