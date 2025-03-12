#pragma once

#include <LkEngine/Core/LObject/Object.h>
#include <LkEngine/Core/LObject/ObjectPtr.h>

#include <LkEngine/Renderer/UI/Panel.h>
#include <LkEngine/Renderer/UI/UILayer.h>

#include "LkEditor/Panel/ContentBrowserItem.h"

namespace LkEngine {

	class LProject;

	namespace UI
	{
		enum EDragAndDropMode
		{
			Copy,
			Move,
			Swap
		};

		namespace DragDropPayload
		{
			static constexpr const char* Asset = "LK_ASSET_PAYLOAD";
		}
	}

	struct FSelectionStack
	{
		FORCEINLINE void CopyFrom(const FSelectionStack& Other)
		{
			SelectionArray.assign(Other.begin(), Other.end());
		}

		FORCEINLINE void CopyFrom(const std::vector<FAssetHandle>& Other)
		{
			SelectionArray.assign(Other.begin(), Other.end());
		}

		FORCEINLINE void Select(const FAssetHandle Handle)
		{
			if (IsSelected(Handle))
			{
				return;
			}
			SelectionArray.push_back(Handle);
		}

		FORCEINLINE void Deselect(const FAssetHandle Handle)
		{
			if (!IsSelected(Handle))
			{
				return;
			}

			std::erase_if(SelectionArray, [Handle](const FAssetHandle OtherHandle)
			{
				return (Handle == OtherHandle);
			});
		}

		FORCEINLINE bool IsSelected(const FAssetHandle Handle) const
		{
			auto IsHandleSelected = [Handle](const FAssetHandle SelectedHandle) { return (SelectedHandle == Handle); };
			return std::find_if(SelectionArray.begin(), SelectionArray.end(), IsHandleSelected) != SelectionArray.end();
		}

		FORCEINLINE void Clear() { SelectionArray.clear(); }

		FORCEINLINE std::size_t Count() const { return SelectionArray.size(); }
		FORCEINLINE const FAssetHandle* Data() const { return SelectionArray.data(); }

		FORCEINLINE FAssetHandle operator[](const std::size_t Idx) const
		{
			LK_CORE_ASSERT((Idx >= 0) && (Idx < SelectionArray.size()));
			return SelectionArray[Idx];
		}

		FORCEINLINE std::vector<FAssetHandle>::iterator begin() { return SelectionArray.begin(); }
		FORCEINLINE std::vector<FAssetHandle>::const_iterator begin() const { return SelectionArray.begin(); }
		FORCEINLINE std::vector<FAssetHandle>::iterator end() { return SelectionArray.end(); }
		FORCEINLINE std::vector<FAssetHandle>::const_iterator end() const { return SelectionArray.end(); }

	private:
		std::vector<FAssetHandle> SelectionArray{};
	};


	/**
	 * FContentBrowserItemList
	 *
	 *  Data structure for content browser items.
	 */
	struct FContentBrowserItemList
	{
		static constexpr std::size_t InvalidItem = std::numeric_limits<std::size_t>::max();

		std::vector<TObjectPtr<LContentBrowserItem>> Items{};

		std::vector<TObjectPtr<LContentBrowserItem>>::iterator begin() { return Items.begin(); }
		std::vector<TObjectPtr<LContentBrowserItem>>::iterator end() { return Items.end(); }
		std::vector<TObjectPtr<LContentBrowserItem>>::const_iterator begin() const { return Items.begin(); }
		std::vector<TObjectPtr<LContentBrowserItem>>::const_iterator end() const { return Items.end(); }

		FORCEINLINE TObjectPtr<LContentBrowserItem>& operator[](const std::size_t Index) 
		{ 
			return Items[Index]; 
		}

		FORCEINLINE const TObjectPtr<LContentBrowserItem>& operator[](const std::size_t Idx) const 
		{ 
			return Items[Idx]; 
		}

		FContentBrowserItemList() = default;
		FContentBrowserItemList(const FContentBrowserItemList& Other)
			: Items(Other.Items)
		{
		}

		FContentBrowserItemList& operator=(const FContentBrowserItemList& Other)
		{
			Items = Other.Items;
			return *this;
		}

		FORCEINLINE void Clear()
		{
			std::scoped_lock<std::mutex> Lock(Mutex);
			Items.clear();
		}

		FORCEINLINE void Erase(const FAssetHandle Handle)
		{
			const std::size_t Index = Find(Handle);
			if (Index == InvalidItem)
			{
				return;
			}

			std::scoped_lock<std::mutex> Lock(Mutex);
			auto Iter = Items.begin() + Index;
			Items.erase(Iter);
		}

		[[nodiscard]] FORCEINLINE std::size_t Find(const FAssetHandle Handle)
		{
			if (Items.empty())
			{
				LK_CORE_WARN_TAG("ContentBrowserList", "Items array is empty");
				return InvalidItem;
			}

			std::scoped_lock<std::mutex> Lock(Mutex);
			for (std::size_t Idx = 0; Idx < Items.size(); Idx++)
			{
				if (Items[Idx]->GetID() == Handle)
				{
					return Idx;
				}
			}

			return InvalidItem;
		}

	private:
		std::mutex Mutex;
	};


	/**
	 * LContentBrowser
	 *
	 *  Content Browser.
	 */
	class LContentBrowserPanel : public IPanel
	{
	public:
		LContentBrowserPanel(); /// TODO: make private and grant access to LPanelManager
		~LContentBrowserPanel() = default;

		virtual void Initialize() override;

		virtual void Render() override {}
		virtual void RenderUI(bool& IsOpen) override;

		void SetSceneContext(const TObjectPtr<LScene> InSceneContext);
		virtual void OnProjectChanged(const TObjectPtr<LProject>& InProject) override;

		FORCEINLINE FContentBrowserItemList& GetCurrentItems() { return ItemList; }
		TObjectPtr<FDirectoryInfo> GetDirectory(const std::filesystem::path& Filepath) const;

		virtual void SerializeToYaml(YAML::Emitter& Out) const override;
		virtual void DeserializeFromYaml(const YAML::Node& Data) override;

		static LContentBrowserPanel& Get();

	private:
		void UpdateInput();

		void Refresh();
		void SortItemList();
		void ChangeDirectory(TObjectPtr<FDirectoryInfo> Directory);

		/**
		 * Process a directory.
		 */
		FAssetHandle ProcessDirectory(const std::filesystem::path& DirectoryPath, const TObjectPtr<FDirectoryInfo>& ParentDir);

		void Search(const std::string& Query, 
					const TObjectPtr<FDirectoryInfo>& DirectoryInfo,
					FContentBrowserItemList& ItemList);

		void OnBrowseForward();
		void OnBrowseBackward();

		void ClearCurrentSelection();
		void UpdateDropArea(const TObjectPtr<FDirectoryInfo>& DirectoryInfo);

		/**
		 * Render the top bar of the content browser which holds the search bar 
		 * and current directory path.
		 */
		void RenderTopBar(const float Height = 32.0f);

		/**
		 * Render the bottom bar.
		 */
		void RenderBottomBar(const float Height = 30.0f);

		void RenderItems();

		/**
		 * @brief Render the directories and files in the outliner.
		 */
		void RenderOutlinerHierarchy(TObjectPtr<FDirectoryInfo> Directory);

	private:
		TObjectPtr<LScene> SceneContext{};
		TObjectPtr<LProject> Project{};

		FContentBrowserItemList ItemList{};
		TObjectPtr<FDirectoryInfo> CurrentDirectory{};
		TObjectPtr<FDirectoryInfo> BaseDirectory{};
		TObjectPtr<FDirectoryInfo> NextDirectory{};
		TObjectPtr<FDirectoryInfo> PreviousDirectory{};

		std::unordered_map<FAssetHandle, TObjectPtr<FDirectoryInfo>> Directories{};
		std::vector<TObjectPtr<FDirectoryInfo>> BreadCrumbData{};
		char SearchBuffer[LContentBrowserItem::INPUT_BUFFER_SIZE];

		bool bIsContentBrowserHovered = false;
		bool bIsAnyItemHovered = false;

		/** Flag to update the navigation path for the breadcrumbs. */
		bool bUpdateNavigationPath = false;

		std::map<std::string, TObjectPtr<LTexture2D>> AssetIconMap{};

		FSelectionStack CopiedAssets{};

		LPANEL(LContentBrowserPanel);
	};

}
