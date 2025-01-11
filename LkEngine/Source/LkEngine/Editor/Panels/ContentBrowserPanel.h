#pragma once

#include "LkEngine/UI/Panel.h"
#include "LkEngine/UI/UILayer.h"

#include "LkEngine/Editor/Panels/ContentBrowserItem.h"


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
	}

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

		FORCEINLINE const TObjectPtr<LContentBrowserItem>& operator[](const std::size_t Index) const 
		{ 
			return Items[Index]; 
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

		FORCEINLINE void Erase(const LUUID Handle)
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

		[[nodiscard]] FORCEINLINE std::size_t Find(const LUUID Handle)
		{
			if (Items.empty())
			{
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
	 * LContentBrowserPanel
	 *
	 *  Content Browser.
	 */
	class LContentBrowserPanel : public IPanel
	{
	public:
		LContentBrowserPanel();
		~LContentBrowserPanel() = default;

		virtual void Initialize() override;

		virtual void OnRender() override {}
		virtual void OnRenderUI(bool& IsOpen) override;

		void SetSceneContext(const TObjectPtr<LScene> InSceneContext);
		void OnProjectChanged(TObjectPtr<LProject> InProject);

		FORCEINLINE FContentBrowserItemList& GetCurrentItems() { return ItemList; }
		TObjectPtr<FDirectoryInfo> GetDirectory(const std::filesystem::path& Filepath) const;

		static LContentBrowserPanel& Get()
		{
			LK_CORE_ASSERT(Instance);
			return *Instance;
		}		

		virtual void SerializeToYaml(YAML::Emitter& Out) const override;
		virtual void DeserializeFromYaml(const YAML::Node& Data) override;

	private:
		void UpdateInput();

		void Refresh();
		void SortItemList();
		void ChangeDirectory(TObjectPtr<FDirectoryInfo> Directory);

		/**
		 * Process a directory.
		 */
		FAssetHandle ProcessDirectory(const std::filesystem::path& DirectoryPath,
									  const TObjectPtr<FDirectoryInfo>& ParentDir);

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

		inline static LContentBrowserPanel* Instance{};

		LCLASS(LContentBrowserPanel);
	};

}
