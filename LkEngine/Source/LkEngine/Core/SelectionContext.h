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

	/**
	 * ESelectionContext
	 * 
	 *  Used a way to sort selections depending on where they are performed.
	 */
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
	 *  The selections are sorted in containers using the enum ESelectionContext.
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
			auto& Selection = SelectionContextMap.at(Context);

			auto Iter = std::find(Selection.begin(), Selection.end(), Handle);
			if (Iter != Selection.end())
			{
				(*Iter) = Handle;
			}
			else
			{
				Selection.push_back(Handle);
			}

			SelectedHandle = Handle;
		}

		FORCEINLINE static void Deselect(const ESelectionContext Context, const LUUID Handle)
		{
			LK_CORE_ASSERT(SelectionContextMap.contains(Context), "Selection context '{}' is missing", Enum::ToString(Context));
			LK_CORE_TRACE_TAG("SelectionContext", "Deselecting: {}", Handle);
			if (Context == ESelectionContext::Global)
			{
				for (auto& [Context, Selection] : SelectionContextMap)
				{
					const std::size_t Erased = std::erase_if(Selection, [Handle](const LUUID& CurrentHandle)
					{
						return (Handle == CurrentHandle);
					});

					/* 
					 * TODO: Not sure about to exit early here or no, depends on if the same 
					 *       UUID can appear in multiple contexts. 
					 *       Disabled for now. 
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
				std::vector<LUUID>& SelectionContext = SelectionContextMap.at(Context);
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
			LK_CORE_TRACE_TAG("SelectionContext", "DeselectAll");
			for (auto& [Context, Selection] : SelectionContextMap)
			{
				Selection.clear();
			}
		}

		/**
		 * Clear all selected items for a specific selection context.
		 */
		FORCEINLINE static void DeselectAll(const ESelectionContext Context)
		{
			LK_CORE_TRACE_TAG("SelectionContext", "DeselectAll: {}", Enum::ToString(Context));
			LK_CORE_ASSERT(SelectionContextMap.contains(Context), "Selection context '{}' is missing", Enum::ToString(Context));
			if (Context != ESelectionContext::Global)
			{
				LK_CORE_ASSERT(SelectionContextMap.contains(Context), "Selection context '{}' is missing", Enum::ToString(Context));
				SelectionContextMap.at(Context).clear();
			}
			else
			{
				for (auto& [Context, Selection] : SelectionContextMap)
				{
					Selection.clear();
				}
			}
		}

		FORCEINLINE static bool IsSelected(const ESelectionContext Context, const LUUID Handle)
		{
			LK_CORE_ASSERT(Handle > 0, "Handle is 0");
			const auto& Selection = SelectionContextMap.at(Context);
			if (Context != ESelectionContext::Global)
			{
				LK_CORE_ASSERT(SelectionContextMap.contains(Context), "Selection context '{}' is missing", Enum::ToString(Context));
				return (std::find(Selection.begin(), Selection.end(), Handle) != Selection.end());
			}
			else
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
		}

		FORCEINLINE static const std::vector<LUUID>& GetSelected(const ESelectionContext Context)
		{
			if (Context == ESelectionContext::Global)
			{
				static std::vector<LUUID> Combined;
				Combined.clear();

				auto AllRanges = SelectionContextMap | std::views::values | std::views::join;
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

		FORCEINLINE static LUUID GetSelected(const ESelectionContext Context, const std::size_t Index)
		{
			LK_CORE_ASSERT(Context != ESelectionContext::Global, "Global selection context cannot be used in GetSelected when using indices");
			LK_CORE_ASSERT(SelectionContextMap.contains(Context), "Selection context '{}' is missing", Enum::ToString(Context));
			auto& Selection = SelectionContextMap.at(Context);
			LK_CORE_VERIFY((Index >= 0) && (Index < Selection.size()));

			return Selection.at(Index);
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

		FORCEINLINE static bool IsAnySelected(const ESelectionContext Context)
		{
			return (GetSelectionCount(Context) > 0);
		}

	public:
		static LUUID SelectedHandle; /* TODO: Remove this and replace it by using sorting of the vectors instead. */

		static std::unordered_map<ESelectionContext, std::vector<LUUID>> SelectionContextMap;

	private:
		LCLASS(LSelectionContext)
	};

}
