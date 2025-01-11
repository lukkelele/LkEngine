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

		FORCEINLINE static void Select(const LUUID Handle)
		{
			SelectedEntities.clear();
			SelectedHandle = Handle;
		}

		FORCEINLINE static void Select(const ESelectionContext Context, const LUUID Handle, const bool Multiselect = false)
		{
			LK_CORE_ASSERT(SelectionContextMap.contains(Context), "Selection context '{}' is missing", Enum::ToString(Context));
			SelectedHandle = Handle;
			auto& SelectionContext = SelectionContextMap.at(Context);
			if (!Multiselect)
			{
				SelectionContext.clear();
			}

			SelectionContext.push_back(Handle);
		}

		FORCEINLINE static void Deselect(const LUUID Handle)
		{
			std::erase_if(SelectedEntities, [Handle](const LUUID& CurrentHandle)
			{
				return (Handle == CurrentHandle);
			});
		}

		FORCEINLINE static void Deselect(const ESelectionContext Context, const LUUID Handle)
		{
			LK_CORE_ASSERT(SelectionContextMap.contains(Context), "Selection context '{}' is missing", Enum::ToString(Context));
			auto& SelectionContext = SelectionContextMap.at(Context);
			std::erase_if(SelectionContext, [Handle](const LUUID& CurrentHandle)
			{
				return (Handle == CurrentHandle);
			});
		}

		FORCEINLINE static void DeselectAll()
		{
			for (auto& [Context, Selection] : SelectionContextMap)
			{
				LK_CORE_TRACE_TAG("SelectionContext", "Deselecting: {}", Enum::ToString(Context));
				Selection.clear();
			}
		}

		FORCEINLINE static void DeselectAll(const ESelectionContext Context)
		{
			LK_CORE_TRACE_TAG("SelectionContext", "Deselecting all for: {}", Enum::ToString(Context));
			LK_CORE_ASSERT(SelectionContextMap.contains(Context), "Selection context '{}' is missing", Enum::ToString(Context));
			auto& SelectionContext = SelectionContextMap.at(Context);
			SelectionContext.clear();
		}

		FORCEINLINE static bool IsSelected(const LUUID Handle)
		{
			return (SelectedHandle == Handle);
		}

		FORCEINLINE static bool IsSelected(const ESelectionContext Context, const LUUID Handle)
		{
			LK_CORE_ASSERT(SelectionContextMap.contains(Context), "Selection context '{}' is missing", Enum::ToString(Context));
			LK_CORE_ASSERT(Handle > 0, "AssetHandle is 0");
			const auto& SelectionContext = SelectionContextMap.at(Context);
			return (std::find(SelectionContext.begin(), SelectionContext.end(), Handle) != SelectionContext.end());
		}

		FORCEINLINE static const std::vector<LUUID>& GetSelected(const ESelectionContext Context)
		{
			LK_CORE_ASSERT(SelectionContextMap.contains(Context), "Selection context '{}' is missing", Enum::ToString(Context));
			return SelectionContextMap.at(Context);
		}

		FORCEINLINE static std::size_t GetSelectionCount(const ESelectionContext Context)
		{
			return SelectionContextMap[Context].size();
		}

		FORCEINLINE static std::vector<LUUID>& GetSelected()
		{
			return SelectedEntities;
		}

		FORCEINLINE void Clear()
		{
			SelectedEntities.clear();
		}

	public:
		static std::vector<LUUID> SelectedEntities;
		static std::unordered_map<ESelectionContext, std::vector<LUUID>> SelectionContextMap;

		static LUUID SelectedHandle;

	private:
		LCLASS(LSelectionContext)
	};

}
