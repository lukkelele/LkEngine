#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"
#include "LkEngine/Asset/Asset.h"

#include "LkEngine/Renderer/UI/Panel.h"
#include "LkEngine/Renderer/UI/UILayer.h"

#define LK_DEBUG_CONTENTBROWSER_ITEMS 1

/* Verbose logging for clicking things in the ContentBrowser. */
#if LK_DEBUG_CONTENTBROWSER_ITEMS
#	define LK_DEBUG_CONTENTBROWSER_ITEM_LOG(...) LK_CORE_INFO_TAG("ContentBrowser", __VA_ARGS__)
#else
#	define LK_DEBUG_CONTENTBROWSER_ITEM_LOG(...)
#endif

namespace LkEngine {

	/**
	 * TODO: Add support for 'None' again and change underlying type to int32_t.
	 */
	enum class EContentBrowserAction : uint32_t
	{
		//None                = -1,
		Refresh             = LK_BIT(0),
		ClearSelections     = LK_BIT(1),
		Selected			= LK_BIT(2),
		Deselected			= LK_BIT(3),
		Hovered             = LK_BIT(4),
		Renamed             = LK_BIT(5),
		OpenDeleteDialogue  = LK_BIT(6),
		SelectToHere        = LK_BIT(7),
		Moved               = LK_BIT(8),
		ShowInExplorer      = LK_BIT(9),
		OpenExternal        = LK_BIT(10),
		Reload              = LK_BIT(11),
		Copy				= LK_BIT(12),
		Duplicate			= LK_BIT(13),
		StartRenaming		= LK_BIT(14),
		Activated			= LK_BIT(15),
	};
	LK_ENUM_CLASS(EContentBrowserAction);
	LK_ENUM_RANGE_FLAGS_BY_FIRST_AND_LAST(EContentBrowserAction, 
										  EContentBrowserAction::Refresh, 
										  EContentBrowserAction::Activated);

	/**
	 * FContentBrowserItemAction
	 *
	 *  Content browser item action result.
	 */
	struct FContentBrowserItemAction
	{
		using FlagType = std::underlying_type_t<EContentBrowserAction>;
		FlagType Flags = 0;

		FORCEINLINE void operator|=(const EContentBrowserAction Flag)
		{
			Flags |= Flag;
		}

		FORCEINLINE FlagType operator&(const EContentBrowserAction Flag) const
		{
			return (Flags & Flag);
		}

		FORCEINLINE void Clear(const EContentBrowserAction Flag)
		{
			Flags &= ~Flag;
		}

		FORCEINLINE bool IsSet(const EContentBrowserAction Flag) const 
		{ 
			return Flags & Flag;
		}
	};
	
	/**
	 * LContentBrowserItem
	 */
	class LContentBrowserItem : public LObject
	{
	public:
		enum class EItemType : uint16_t
		{
			None = 0,
			Asset, 
			Directory
		};
	public:
		LContentBrowserItem(const EItemType InType, 
							const LUUID InID, 
							std::string_view InName,
							const TObjectPtr<LTexture2D> InIcon);
		LContentBrowserItem() = delete;
		virtual ~LContentBrowserItem() = default;

		void BeginRender();
		virtual FContentBrowserItemAction Render();
		void EndRender();

		virtual void Delete() {};
		virtual bool Move(const std::filesystem::path& Destination) { return false; }

		FORCEINLINE LUUID GetID() const { return ID; }
		FORCEINLINE EItemType GetType() const { return ItemType; }

		/**
		 * @brief Get the icon texture.
		 */
		FORCEINLINE const TObjectPtr<LTexture2D> GetIcon() const { return Icon; }

		FORCEINLINE const std::string& GetName() const { return FileName; }

		void StartRenaming();
		void StopRenaming();
		FORCEINLINE bool IsRenaming() const { return bIsRenaming; }

		void Rename(const std::string& NewName);
		void SetDisplayNameFromFileName();

	protected:
		virtual void OnRenamed(const std::string& NewName) { FileName = NewName; }
		virtual void RenderCustomContextItems() {}
		virtual void UpdateDrop(FContentBrowserItemAction& ActionResult) {}
	private:
		void OnContextMenuOpen(FContentBrowserItemAction& ActionResult);

	public:
		inline static constexpr int INPUT_BUFFER_SIZE = 128;
		inline static constexpr int DISPLAY_NAME_MAX_LENGTH = 22;
	private:
		EItemType ItemType = EItemType::None;
		LUUID ID = 0;
		std::string FileName{};
		std::string DisplayName{};
		TObjectPtr<LTexture2D> Icon{};

		bool bIsRenaming = false;
		bool bWasJustSelected = false;
		bool bIsDragging = false;

		LCLASS(LContentBrowserItem);
	};

	/**
	 * FDirectoryInfo
	 * 
	 *  Information about a directory.
	 */
	struct FDirectoryInfo : public LObject
	{
		LUUID Handle = 0;
		std::filesystem::path FilePath{};
		std::vector<LUUID> Assets{};

		TObjectPtr<FDirectoryInfo> Parent{};
		std::map<LUUID, TObjectPtr<FDirectoryInfo>> SubDirectories;

		FDirectoryInfo()
		{
			LOBJECT_REGISTER();

			SubDirectories.clear();
		}

	private:
		LSTRUCT(FDirectoryInfo);
	};

	/**
	 * LContentBrowserDirectory
	 */
	class LContentBrowserDirectory : public LContentBrowserItem
	{
	public:
		LContentBrowserDirectory(const TObjectPtr<FDirectoryInfo>& InDirectoryInfo);
		LContentBrowserDirectory() = delete;
		virtual ~LContentBrowserDirectory() = default;

		TObjectPtr<FDirectoryInfo>& GetDirectoryInfo() { return DirectoryInfo; }

		virtual void Delete() override;
		virtual bool Move(const std::filesystem::path& Destination) override;

	private:
		virtual void OnRenamed(const std::string& NewName) override;
		virtual void UpdateDrop(FContentBrowserItemAction& ActionResult) override;

	private:
		TObjectPtr<FDirectoryInfo> DirectoryInfo{};

		LCLASS(LContentBrowserDirectory);
	};

	/**
	 * LContentBrowserAsset
	 */
	class LContentBrowserAsset : public LContentBrowserItem
	{
	public:
		LContentBrowserAsset(const FAssetMetadata& InAssetMetadata, const TObjectPtr<LTexture2D>& InIcon);
		virtual ~LContentBrowserAsset() = default;

		FORCEINLINE const FAssetMetadata& GetAssetMetadata() const { return AssetMetadata; }

		virtual void Delete() override;
		virtual bool Move(const std::filesystem::path& Destination) override;

	private:
		virtual void OnRenamed(const std::string& NewName) override;

	private:
		FAssetMetadata AssetMetadata;

		LCLASS(LContentBrowserAsset);
	};


	namespace Enum 
	{
		static constexpr const char* ToString(const LContentBrowserItem::EItemType ItemType)
		{
			switch (ItemType)
			{
				case LContentBrowserItem::EItemType::Asset:      return "Asset";
				case LContentBrowserItem::EItemType::Directory:  return "Directory";
				case LContentBrowserItem::EItemType::None:       return "None";
			}

			LK_CORE_ASSERT(false, "ToString(LContentBrowserItem::EItemType) failed for: {}", static_cast<int>(ItemType));
			return nullptr;
		}

		static constexpr const char* ToString(const EContentBrowserAction BrowserAction)
		{
			switch (BrowserAction)
			{
				//case EContentBrowserAction::None:               return "None";
				case EContentBrowserAction::Refresh:            return "Refresh";
				case EContentBrowserAction::ClearSelections:    return "ClearSelections";
				case EContentBrowserAction::Selected:           return "Selected";
				case EContentBrowserAction::Deselected:         return "Deselected";
				case EContentBrowserAction::Hovered:            return "Hovered";
				case EContentBrowserAction::Renamed:            return "Renamed";
				case EContentBrowserAction::OpenDeleteDialogue: return "OpenDeleteDialogue";
				case EContentBrowserAction::SelectToHere:       return "SelectToHere";
				case EContentBrowserAction::Moved:              return "Moved";
				case EContentBrowserAction::ShowInExplorer:     return "ShowInExplorer";
				case EContentBrowserAction::OpenExternal:       return "OpenExternal";
				case EContentBrowserAction::Reload:             return "Reload";
				case EContentBrowserAction::Copy:               return "Copy";
				case EContentBrowserAction::Duplicate:          return "Duplicate";
				case EContentBrowserAction::StartRenaming:      return "StartRenaming";
				case EContentBrowserAction::Activated:          return "Activated";
			}

			LK_CORE_ASSERT(false, "ToString(EContentBrowserAction) failed for: {}", static_cast<int>(BrowserAction));
			return nullptr;
		}
	}

}
