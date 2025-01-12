#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"
#include "LkEngine/Asset/Asset.h"

#include "LkEngine/UI/Panel.h"
#include "LkEngine/UI/UILayer.h"


namespace LkEngine {

	enum class EContentBrowserAction : uint16_t
	{
		None                = 0,
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
		Activated			= LK_BIT(15)
	};
	LK_ENUM_CLASS_FLAGS(EContentBrowserAction);

	/**
	 * FContentBrowserItemAction
	 *
	 *  Content browser item action result.
	 */
	struct FContentBrowserItemAction
	{
		using FlagType = uint16_t;
		FlagType Flags = 0;

		void Set(const EContentBrowserAction Flag, const bool Value)
		{
			static_assert(std::numeric_limits<decltype(Flags)>::max() == std::numeric_limits<std::underlying_type_t<EContentBrowserAction>>::max());
			if (Value)
			{
				Flags |= (FlagType)Flag;
			}
			else
			{
				Flags &= ~(FlagType)Flag;
			}
		}

		bool IsSet(const EContentBrowserAction Flag) const 
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
		std::vector<LUUID> Assets;

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
		TObjectPtr<FDirectoryInfo> DirectoryInfo;

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
			return nullptr;
		}
	}

}
