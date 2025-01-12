#include "LKpch.h"
#include "ContentBrowserItem.h"

#include "LkEngine/Core/SelectionContext.h"
#include "LkEngine/Core/IO/FileSystem.h"
#include "LkEngine/Core/Input/Input.h"
#include "LkEngine/Editor/EditorSettings.h"
#include "LkEngine/Editor/Panels/ContentBrowserPanel.h"

#include "LkEngine/Renderer/Color.h"
#include "LkEngine/UI/UILayer.h"

#include "LkEngine/Project/Project.h"


namespace LkEngine {

	namespace fs = std::filesystem;

	static char RenameBuffer[LContentBrowserItem::INPUT_BUFFER_SIZE];

	LContentBrowserItem::LContentBrowserItem(const EItemType InType, 
											 const LUUID InID, 
											 std::string_view InName, 
											 const TObjectPtr<LTexture2D> InIcon)
		: ItemType(InType)
		, ID(InID)
		, FileName(InName)
		, Icon(InIcon)
	{
		LOBJECT_REGISTER();
		LK_CORE_VERIFY(Icon, "Icon is nullptr for '{}' ({})", InName, Enum::ToString(InType));

		DisplayName = FileName;
		if (FileName.size() > DISPLAY_NAME_MAX_LENGTH)
		{
			DisplayName = FileName.substr(0, DISPLAY_NAME_MAX_LENGTH) + "...";
		}
	}

	void LContentBrowserItem::BeginRender()
	{
		ImGui::PushID(&ID);
		ImGui::BeginGroup();
	}

	FContentBrowserItemAction LContentBrowserItem::Render()
	{
		FContentBrowserItemAction Result{};

		const auto& EditorSettings = FEditorSettings::Get();
		const float ThumbnailSize = (float)EditorSettings.ContentBrowserThumbnailSize;
		const bool bDisplayAssetType = EditorSettings.bContentBrowserShowAssetTypes;

		SetDisplayNameFromFileName();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

		static constexpr float EdgeOffset = 4.0f;
		const float TextLineHeight = ImGui::GetTextLineHeightWithSpacing() * 2.0f + EdgeOffset * 2.0f;
		const float InfoPanelHeight = std::max(
			(bDisplayAssetType ? (ThumbnailSize * 0.50f) : TextLineHeight),
			TextLineHeight
		);

		const ImVec2 TopLeft = ImGui::GetCursorScreenPos();
		const ImVec2 ThumbBottomRight = { TopLeft.x + ThumbnailSize, TopLeft.y + ThumbnailSize };
		const ImVec2 InfoTopLeft = { TopLeft.x,	TopLeft.y + ThumbnailSize };
		const ImVec2 BottomRight = { TopLeft.x + ThumbnailSize, TopLeft.y + ThumbnailSize + InfoPanelHeight };

		/**
		 * Draw shadow on current item.
		 */
		auto DrawShadow = [](const ImVec2& TopLeft, const ImVec2& BottomRight, const bool IsDirectory)
		{
			ImDrawList* DrawList = ImGui::GetWindowDrawList();
			const ImRect ItemRect = UI::RectOffset(ImRect(TopLeft, BottomRight), 1.0f, 1.0f);
			DrawList->AddRect(ItemRect.Min, 
							  ItemRect.Max, 
							  RGBA32::PropertyField, 
							  6.0f, 
							  (IsDirectory ? 0 : ImDrawFlags_RoundCornersBottom), 
							  2.0f);
		};

		const bool IsFocused = ImGui::IsWindowFocused();
		const bool IsSelected = LSelectionContext::IsSelected(ESelectionContext::ContentBrowser, ID);

		if (ItemType != EItemType::Directory)
		{
			ImDrawList* DrawList = ImGui::GetWindowDrawList();

			/* Draw shadow. */
			DrawShadow(TopLeft, BottomRight, false);

			/* Draw background. */
			DrawList->AddRectFilled(TopLeft, ThumbBottomRight, RGBA32::BackgroundDark);
			DrawList->AddRectFilled(InfoTopLeft, BottomRight, RGBA32::GroupHeader, 6.0f, ImDrawFlags_RoundCornersBottom);
		}

		/**
		 * Thumbnail.
		 */
		LK_CORE_ASSERT(Icon && Icon->IsValid(), "Icon is not valid");
		ImGui::InvisibleButton("##ThumbnailButton", ImVec2(ThumbnailSize, ThumbnailSize));
		UI::DrawButtonImage(
			Icon, 
			IM_COL32(255, 255, 255, 225),
			IM_COL32(255, 255, 255, 255),
			IM_COL32(255, 255, 255, 255),
			UI::RectExpanded(UI::GetItemRect(), -6.0f, -6.0f)
		);

		auto RenamingWidget = [&]
		{
			ImGui::SetKeyboardFocusHere();
			ImGui::InputText("##Rename", RenameBuffer, INPUT_BUFFER_SIZE);

			if (ImGui::IsItemDeactivatedAfterEdit() || LInput::IsKeyDown(EKey::Enter))
			{
				Rename(RenameBuffer);
				bIsRenaming = false;
				SetDisplayNameFromFileName();
				Result.Set(EContentBrowserAction::Renamed, true);
			}
		};

		UI::ShiftCursor(EdgeOffset, EdgeOffset);
		if (ItemType == EItemType::Directory)
		{
			const ImVec2 InfoPanelSize(ThumbnailSize - EdgeOffset * 2.0f, InfoPanelHeight - EdgeOffset);
			ImGui::BeginGroup();
			/* Center-align the directory name. */
			{
				const ImVec2 HorizontalSize(ThumbnailSize - 2.0f, 0.0f);
				ImGui::BeginGroup();

				const float AvailableSpace = HorizontalSize.x - ImGui::CalcTextSize(DisplayName.c_str()).x;
				if (AvailableSpace > 0)
				{
					ImGui::Dummy(ImVec2(AvailableSpace / 2.0f, 0.0f));
					ImGui::SameLine();
				}

				/* Center-aligned text. */
				ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + (ThumbnailSize - EdgeOffset * 3.0f));
				const float TextWidth = std::min(ImGui::CalcTextSize(DisplayName.c_str()).x, ThumbnailSize);

				if (bIsRenaming)
				{
					ImGui::SetNextItemWidth(ThumbnailSize - EdgeOffset * 3.0f);
					RenamingWidget();
				}
				else
				{
					ImGui::SetNextItemWidth(TextWidth);
					ImGui::Text(DisplayName.c_str());
				}
				ImGui::PopTextWrapPos();

				/* Add a spring on the right side. */
				if (AvailableSpace > 0)
				{
					ImGui::SameLine();
					ImGui::Dummy(ImVec2(AvailableSpace / 2.0f, 0.0f));
				}

				ImGui::EndGroup();
			}

			ImGui::Dummy(ImVec2(0.0f, EdgeOffset));

			ImGui::EndGroup();
		}
		else
		{
			ImGui::BeginGroup();
			{
				const ImVec2 PanelSize(ThumbnailSize - EdgeOffset * 3.0f, InfoPanelHeight - EdgeOffset);

				/* Begin horizontal layout for the label. */
				ImGui::BeginGroup();

				/* Suspend layout for renaming or displaying the name. */
				const float TextWrapPos = ImGui::GetCursorPosX() + (ThumbnailSize - EdgeOffset * 2.0f);
				ImGui::PushTextWrapPos(TextWrapPos);

				if (bIsRenaming)
				{
					const float ItemWidth = ThumbnailSize - EdgeOffset * 3.0f;
					ImGui::SetNextItemWidth(ItemWidth);
					RenamingWidget();
				}
				else
				{
					ImGui::Text(DisplayName.c_str());
				}

				ImGui::PopTextWrapPos();

				/* Add a spring for spacing. */
				const float SpringOffset = 0.0f;
				ImGui::Dummy(ImVec2(SpringOffset, 0.0f));

				ImGui::EndGroup(); /* End horizontal layout for the label. */
			}

			const float VerticalSpringOffset = EdgeOffset;
			ImGui::Dummy(ImVec2(0.0f, VerticalSpringOffset));

			if (bDisplayAssetType)
			{
				/* Begin horizontal layout for the asset type. */
				ImGui::SetCursorPosX(EdgeOffset);
				ImGui::BeginGroup();
				ImGui::Dummy(ImVec2(0.0f, EdgeOffset));
				{
					const FAssetMetadata& Metadata = LProject::GetRuntimeAssetManager()->GetMetadata(ID);
					const std::string AssetType = StringUtils::ToUpper(Enum::ToString(Metadata.Type));

					/* Set text color. */
					UI::FScopedColor TextColour(ImGuiCol_Text, RGBA32::Text::Darker);

					UI::Font::Push("Small");

					ImGui::TextUnformatted(AssetType.c_str());
					UI::Font::Pop();
				}

				ImGui::EndGroup(); /* End horizontal layout for the asset type. */

				/* Add a spring for alignment with negative spacing. */
				const float SpringAlignmentOffset = -1.0f;
				ImGui::Dummy(ImVec2(SpringAlignmentOffset, EdgeOffset));
			}

			ImGui::EndGroup(); /* End vertical layout for the info panel. */

		}
		UI::ShiftCursor(-EdgeOffset, -EdgeOffset);

		if (!bIsRenaming)
		{
			if (LInput::IsKeyDown(EKey::F2) && IsSelected && IsFocused)
			{
				StartRenaming();
			}
		}

		ImGui::PopStyleVar(); /* ItemSpacing */
		ImGui::EndGroup();

		/* Draw outline. */
		if (IsSelected || ImGui::IsItemHovered())
		{
			ImDrawList* DrawList = ImGui::GetWindowDrawList();

			if (IsSelected)
			{
				const bool IsMouseDown = ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsItemHovered();
				const ImColor ColorTransition = UI::ColorWithMultipliedValue(RGBA32::Selection, 0.65f);

				const ImVec2 ItemRectMin = (ItemType == EItemType::Directory) ? ImGui::GetItemRectMin() : InfoTopLeft;
				const ImVec2 ItemRectMax = (ItemType == EItemType::Directory) ? ImGui::GetItemRectMax() : BottomRight;

				DrawList->AddRect(
					ItemRectMin,
					ItemRectMax,
					(IsMouseDown ? ImColor(ColorTransition) : ImColor(RGBA32::Selection)), 
					6.0f, 
					(ItemType == EItemType::Directory ? 0 : ImDrawFlags_RoundCornersBottom), 
					1.0f
				);
			}
			else /* Is Hovered */
			{
				ImRect ItemRect = UI::GetItemRect();
				if (ItemType != EItemType::Directory)
				{
					DrawList->AddRect(
						InfoTopLeft, 
						BottomRight,
						RGBA32::Muted, 
						6.0f, 
						ImDrawFlags_RoundCornersBottom, 
						1.0f
					);
				}
			}
		}

		/***************************************
		 * Mouse Events.
		 ***************************************/
		//UpdateDrop(Result);

		bool Dragging = false;
		if (Dragging = ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			bIsDragging = true;

			const auto& SelectedItems = LSelectionContext::GetSelected(ESelectionContext::ContentBrowser);
			if (!LSelectionContext::IsSelected(ESelectionContext::ContentBrowser, ID))
			{
				Result.Set(EContentBrowserAction::ClearSelections, true);
			}

			auto& CurrentItems = LContentBrowserPanel::Get().GetCurrentItems();

			if (!SelectedItems.empty())
			{
				for (const auto& SelectedItemHandles : SelectedItems)
				{
					const std::size_t Index = CurrentItems.Find(SelectedItemHandles);
					if (Index == FContentBrowserItemList::InvalidItem)
					{
						continue;
					}

					const TObjectPtr<LContentBrowserItem>& Item = CurrentItems[Index];
					UI::Image(Item->GetIcon(), ImVec2(20, 20));
					ImGui::SameLine();
					const std::string& ItemName = Item->GetName();
					ImGui::TextUnformatted(ItemName.c_str());
				}

				/* Accept drag'n'drop payload. */
				ImGui::SetDragDropPayload(
					"AssetPayload", 
					SelectedItems.data(), 
					sizeof(LUUID) * SelectedItems.size()
				);
			}

			Result.Set(EContentBrowserAction::Selected, true);
			ImGui::EndDragDropSource();
		}

		if (ImGui::IsItemHovered())
		{
			//Result.Set(EContentBrowserAction::Hovered, true);
			Result.Flags |= EContentBrowserAction::Hovered;

			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && !bIsRenaming)
			{
				//Result.Set(EContentBrowserAction::Activated, true);
				Result.Flags |= EContentBrowserAction::Activated;
			}
			else
			{
				const bool MouseAction = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
				const bool IsSelected = LSelectionContext::IsSelected(ESelectionContext::ContentBrowser, ID);
				const bool SkipBecauseDragging = bIsDragging && IsSelected;

				if (MouseAction && !SkipBecauseDragging)
				{
					if (bWasJustSelected)
					{
						bWasJustSelected = false;
					}

					if (IsSelected && LInput::IsKeyDown(EKey::LeftControl) && !bWasJustSelected)
					{
						//Result.Set(EContentBrowserAction::Deselected, true);
						Result.Flags |= EContentBrowserAction::Deselected;
					}

					if (!IsSelected)
					{
						//Result.Set(EContentBrowserAction::Selected, true);
						Result.Flags |= EContentBrowserAction::Selected;
						bWasJustSelected = true;
					}

					if (!LInput::IsKeyDown(EKey::LeftControl) && !LInput::IsKeyDown(EKey::LeftShift) && bWasJustSelected)
					{
						//Result.Set(EContentBrowserAction::ClearSelections, true);
						Result.Flags |= EContentBrowserAction::ClearSelections;
					}

					if (LInput::IsKeyDown(EKey::LeftShift))
					{
						Result.Flags |= EContentBrowserAction::SelectToHere;
					}
				}
			}
		}

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 4.0f));
		if (ImGui::BeginPopupContextItem("CBItemContextMenu"))
		{
			Result.Set(EContentBrowserAction::Selected, true);
			OnContextMenuOpen(Result);
			ImGui::EndPopup();
		}
		ImGui::PopStyleVar();

		bIsDragging = Dragging;

		return Result;
	}

	void LContentBrowserItem::EndRender()
	{
		ImGui::PopID();
		ImGui::NextColumn();
	}

	void LContentBrowserItem::StartRenaming()
	{
		if (bIsRenaming)
		{
			return;
		}

		std::memset(RenameBuffer, 0, INPUT_BUFFER_SIZE);
		std::memcpy(RenameBuffer, FileName.c_str(), FileName.size());
		bIsRenaming = true;
	}

	void LContentBrowserItem::StopRenaming()
	{
		bIsRenaming = false;
		SetDisplayNameFromFileName();
		std::memset(RenameBuffer, 0, INPUT_BUFFER_SIZE);
	}

	void LContentBrowserItem::Rename(const std::string& NewName)
	{
		OnRenamed(NewName);
	}

	void LContentBrowserItem::SetDisplayNameFromFileName()
	{
	}

	void LContentBrowserItem::OnContextMenuOpen(FContentBrowserItemAction& ActionResult)
	{
		if (ImGui::MenuItem("Reload"))
		{
			ActionResult.Set(EContentBrowserAction::Reload, true);
		}

		if ((LSelectionContext::GetSelectionCount(ESelectionContext::ContentBrowser) == 1) 
			&& ImGui::MenuItem("Rename"))
		{
			ActionResult.Set(EContentBrowserAction::StartRenaming, true);
		}

		if (ImGui::MenuItem("Copy"))
		{
			ActionResult.Set(EContentBrowserAction::Copy, true);
		}

		if (ImGui::MenuItem("Duplicate"))
		{
			ActionResult.Set(EContentBrowserAction::Duplicate, true);
		}

		if (ImGui::MenuItem("Delete"))
		{
			ActionResult.Set(EContentBrowserAction::OpenDeleteDialogue, true);
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Show In Explorer"))
		{
			ActionResult.Set(EContentBrowserAction::ShowInExplorer, true);
		}

		if (ImGui::MenuItem("Open Externally"))
		{
			ActionResult.Set(EContentBrowserAction::OpenExternal, true);
		}

		RenderCustomContextItems();
	}


	/******************************************************************
	 * LContentBrowserDirectory
	 ******************************************************************/
	LContentBrowserDirectory::LContentBrowserDirectory(const TObjectPtr<FDirectoryInfo>& InDirectoryInfo)
		: LContentBrowserItem(
			LContentBrowserItem::EItemType::Directory, 
			InDirectoryInfo->Handle, 
			InDirectoryInfo->FilePath.filename().string(), 
			FEditorResources::FolderIcon)
		, DirectoryInfo(InDirectoryInfo)
	{
		LOBJECT_REGISTER();
	}

	void LContentBrowserDirectory::Delete()
	{
		const bool Deleted = LFileSystem::DeleteFile(LProject::Current()->GetAssetDirectory() / DirectoryInfo->FilePath);
		if (!Deleted)
		{
			LK_CORE_ERROR_TAG("ContentBrowserDirectory", "Failed to delete folder: '{}'", DirectoryInfo->FilePath);
			return;
		}

		for (LUUID Asset : DirectoryInfo->Assets)
		{
			/* TODO: Notify about deleted asset. */
			//LProject::GetRuntimeAssetManager()->OnAssetDeleted(asset);
			LK_CORE_FATAL_TAG("ContentBrowserAsset", "[FIXME] OnAssetDeleted");
		}
	}

	bool LContentBrowserDirectory::Move(const std::filesystem::path& Destination)
	{
		const std::filesystem::path AssetDir = LProject::Current()->GetAssetDirectory();
		const bool WasMoved = LFileSystem::MoveFile(AssetDir / DirectoryInfo->FilePath, AssetDir / Destination);
		if (!WasMoved)
		{
			return false;
		}

		return true;
	}

	void LContentBrowserDirectory::OnRenamed(const std::string& NewName)
	{
		LContentBrowserItem::OnRenamed(NewName);
	}

	void LContentBrowserDirectory::UpdateDrop(FContentBrowserItemAction& ActionResult)
	{
		LContentBrowserItem::UpdateDrop(ActionResult);
	}

	/******************************************************************
	 * LContentBrowserAsset
	 ******************************************************************/
	LContentBrowserAsset::LContentBrowserAsset(const FAssetMetadata& InAssetMetadata, 
											   const TObjectPtr<LTexture2D>& InIcon)
		: LContentBrowserItem(
			LContentBrowserItem::EItemType::Asset, 
			InAssetMetadata.Handle, 
			InAssetMetadata.FilePath.filename().string(), 
			InIcon)
		, AssetMetadata(InAssetMetadata)
	{
		LOBJECT_REGISTER();
	}

	void LContentBrowserAsset::Delete()
	{
		const fs::path Filepath = LProject::GetRuntimeAssetManager()->GetFileSystemPath(AssetMetadata);
		const bool Deleted = LFileSystem::DeleteFile(Filepath);
		if (!Deleted)
		{
			LK_CORE_ERROR_TAG("ContentBrowserAsset", "Failed to delete: '{}'", AssetMetadata.FilePath);
			return;
		}

		TObjectPtr<FDirectoryInfo> CurrentDirectory = LContentBrowserPanel::Get().GetDirectory(AssetMetadata.FilePath.parent_path());
		CurrentDirectory->Assets.erase(
			std::remove(CurrentDirectory->Assets.begin(), CurrentDirectory->Assets.end(), AssetMetadata.Handle), 
			CurrentDirectory->Assets.end()
		);

		/* TODO: Notify about deleted asset. */
		//LProject::GetRuntimeAssetManager()->OnAssetDeleted(AssetMetadata.Handle);
		LK_CORE_FATAL_TAG("ContentBrowserAsset", "[FIXME] OnAssetDeleted");
	}

	bool LContentBrowserAsset::Move(const std::filesystem::path& Destination)
	{
		const fs::path Filepath = LProject::GetRuntimeAssetManager()->GetFileSystemPath(AssetMetadata);
		const bool WasMoved = LFileSystem::MoveFile(Filepath, LProject::Current()->GetAssetDirectory() / Destination);
		if (!WasMoved)
		{
			LK_CORE_ERROR_TAG("ContentBrowserPanel", "Failed to move '{}' to '{}'", AssetMetadata.FilePath, Destination);
			return false;
		}

		/* TODO: Notify about renamed asset. */
		//LProject::GetRuntimeAssetManager()->OnAssetRenamed(AssetMetadata.Handle, Destination / Filepath.filename());
		LK_CORE_FATAL_TAG("ContentBrowserAsset", "[FIXME] OnAssetRenamed");

		return true;
	}

	void LContentBrowserAsset::OnRenamed(const std::string& NewName)
	{
		fs::path Filepath = LProject::GetRuntimeAssetManager()->GetFileSystemPath(AssetMetadata);
		const std::string Extension = Filepath.extension().string();
		const fs::path NewFilepath = std::format("{}\\{}{}", Filepath.parent_path().string(), NewName, Extension);

		const std::string TargetName = std::format("{}{}", NewName, Extension);
		if (StringUtils::ToLower(TargetName) == StringUtils::ToLower(Filepath.filename().string()))
		{
			LFileSystem::RenameFilename(Filepath, "Temp-Rename");
			Filepath = std::format("{}\\Temp-Rename{}", Filepath.parent_path().string(), Extension);
		}

		if (LFileSystem::RenameFilename(Filepath, NewName))
		{
			//LProject::GetRuntimeAssetManager()->OnAssetRenamed(m_AssetInfo.Handle, newFilepath);
			LK_CORE_FATAL_TAG("ContentBrowserAsset", "[FIXME] OnAssetRenamed");
		}
		else
		{
			LK_CORE_ERROR_TAG("ContentBrowserAsset", "Failed to rename '{}' to '{}'", Filepath.filename().string(), NewName);
		}
	}

}
