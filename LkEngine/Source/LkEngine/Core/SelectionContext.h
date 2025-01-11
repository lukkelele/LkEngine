/******************************************************************
 * SelectionContext
 * 
 *
 *******************************************************************/
#pragma once

#include "LkEngine/Core/Delegate/Delegate.h"
#include "LkEngine/Core/Delegate/DelegateTraits.h"

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Scene/Entity.h"


namespace LkEngine {

	enum class ESelectionContext
	{
		Global = 0, 
		Scene, 
		ContentBrowser, 
	};

	namespace Enum 
	{ 
		static constexpr const char* ToString(const ESelectionContext Context)
		{
			switch (Context)
			{
				case ESelectionContext::Global:         return "Global";
				case ESelectionContext::Scene:          return "Scene";
				case ESelectionContext::ContentBrowser: return "ContentBrowser";
			}
			LK_CORE_ASSERT(false, "Enum::ToString failed for ESelectionContext with value: {}", static_cast<int>(Context));
			return nullptr;
		}
	}

	/**
	 * LSelectionContext
	 *
	 *  Keeps track of selected objects and items.
	 *  The selections are sorted in to containers using the enum ESelectionContext.
	 */
	class LSelectionContext : public LObject
	{
	public:
		LSelectionContext();
		~LSelectionContext() = default;

		static LSelectionContext& Get();

		FORCEINLINE static void Select(const ESelectionContext Context, const LUUID Handle)
		{
			LK_CORE_ASSERT(SelectionContextMap.contains(Context), "Selection context '{}' is missing", Enum::ToString(Context));
			auto& SelectionContext = SelectionContextMap.at(Context);
			SelectionContext.push_back(Handle);

			SelectedHandle = Handle;
		}

		FORCEINLINE static void Deselect(const ESelectionContext Context, const LUUID Handle)
		{
			LK_CORE_ASSERT(SelectionContextMap.contains(Context), "Selection context '{}' is missing", Enum::ToString(Context));
			if (Context == ESelectionContext::Global)
			{
				for (auto& [Context, Selection] : SelectionContextMap)
				{
					const std::size_t Erased = std::erase_if(Selection, [Handle](const LUUID& CurrentHandle)
					{
						return (Handle == CurrentHandle);
					});

					/* 
					 * TODO: Early exit or not? Depends on if same ID can appear in multiple contexts. 
					 * Disabled for now. 
					 */
				#if 0
					if (Erased > 0)
					{
						return;
					}
				#endif
				}
			}
			else
			{
				LK_CORE_ASSERT(SelectionContextMap.contains(Context), "Selection context '{}' is missing", Enum::ToString(Context));
				auto& SelectionContext = SelectionContextMap.at(Context);
				std::erase_if(SelectionContext, [Handle](const LUUID& CurrentHandle)
				{
					return (Handle == CurrentHandle);
				});
			}

		}

		/**
		 * Deselect all selection contexts (global).
		 */
		FORCEINLINE static void DeselectAll()
		{
			for (auto& [Context, Selection] : SelectionContextMap)
			{
				LK_CORE_TRACE_TAG("SelectionContext", "Deselecting: {}", Enum::ToString(Context));
				Selection.clear();
			}
		}

		/**
		 * Clear all selected items for a specific selection context.
		 */
		FORCEINLINE static void DeselectAll(const ESelectionContext Context)
		{
			LK_CORE_TRACE_TAG("SelectionContext", "Deselecting all for: {}", Enum::ToString(Context));
			LK_CORE_ASSERT(SelectionContextMap.contains(Context), "Selection context '{}' is missing", Enum::ToString(Context));
			if (Context == ESelectionContext::Global)
			{
				for (auto& [Context, Selection] : SelectionContextMap)
				{
					Selection.clear();
				}
			}
			else
			{
				LK_CORE_ASSERT(SelectionContextMap.contains(Context), "Selection context '{}' is missing", Enum::ToString(Context));
				SelectionContextMap.at(Context).clear();
			}
		}

		FORCEINLINE static bool IsSelected(const ESelectionContext Context, const LUUID Handle)
		{
			LK_CORE_ASSERT(Handle > 0, "AssetHandle is 0");
			const auto& SelectionContext = SelectionContextMap.at(Context);
			if (Context == ESelectionContext::Global)
			{
				for (auto& [Context, Selection] : SelectionContextMap)
				{
					if (std::find(Selection.begin(), Selection.end(), Handle) != Selection.end())
					{
						return true;
					}
				}

				return false;
			}
			else
			{
				LK_CORE_ASSERT(SelectionContextMap.contains(Context), "Selection context '{}' is missing", Enum::ToString(Context));
				return (std::find(SelectionContext.begin(), SelectionContext.end(), Handle) != SelectionContext.end());
			}
		}

		FORCEINLINE static const std::vector<LUUID>& GetSelected(const ESelectionContext Context)
		{
			if (Context == ESelectionContext::Global)
			{
				auto AllRanges = SelectionContextMap | std::views::values | std::views::join;
				std::vector<LUUID> Combined;
				Combined.reserve(std::ranges::distance(AllRanges));
				std::ranges::copy(AllRanges, std::back_inserter(Combined));
				return Combined;
			}
			else
			{
				LK_CORE_ASSERT(SelectionContextMap.contains(Context), "Selection context '{}' is missing", Enum::ToString(Context));
				return SelectionContextMap.at(Context);
			}
		}

		/**
		 * Get the count of currently selected items for a context.
		 */
		FORCEINLINE static std::size_t GetSelectionCount(const ESelectionContext Context)
		{
			if (Context == ESelectionContext::Global)
			{
				auto AllRanges = SelectionContextMap | std::views::values | std::views::join;
				return std::ranges::distance(AllRanges);
			}
			else
			{
				LK_CORE_ASSERT(SelectionContextMap.contains(Context), "Selection context '{}' is missing", Enum::ToString(Context));
				return SelectionContextMap[Context].size();
			}
		}

	public:
		/** Currently active selection. */
		static LUUID SelectedHandle;

		static std::unordered_map<ESelectionContext, std::vector<LUUID>> SelectionContextMap;

	private:
		LCLASS(LSelectionContext)
	};

}
