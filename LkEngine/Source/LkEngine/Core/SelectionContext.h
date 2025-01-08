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

	LK_DECLARE_MULTICAST_DELEGATE(FOnObjectSelectionChanged, const LObject&);

	/** Global, selected entity. */
	extern TObjectPtr<LObject> GSelectedObject;

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

	class LSelectionContext : public LObject
	{
	public:
		LSelectionContext();
		~LSelectionContext() = default;

		static LSelectionContext& Get();

	#if 0
		FORCEINLINE static void Select(const LEntity InEntity)
		{
			LK_CORE_ASSERT(InEntity.GetUUID());
			SelectedEntities.clear();
			SelectedHandle = InEntity.GetUUID();
		}

		FORCEINLINE static void Deselect(const LEntity InEntity)
		{
			LK_CORE_ASSERT(InEntity.GetUUID());
			/// FIXME
		#if 0
			auto Iter = std::find(SelectedEntities.begin(), SelectedEntities.end(), InEntity.GetUUID());
			if (Iter != SelectedEntities.end())
			{
				std::erase(SelectedEntities, Iter);
			}
		#endif
		}
	#endif

		FORCEINLINE static void Select(const LUUID Handle)
		{
			SelectedEntities.clear();
			SelectedHandle = Handle;
		}

		FORCEINLINE static void Select(const ESelectionContext Context, const LUUID Handle, const bool Multiselect = false)
		{
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
			LK_CORE_DEBUG_TAG("SelectionContext", "DeselectAll");
			for (auto& [Context, Selection] : SelectionContextMap)
			{
				LK_CORE_DEBUG_TAG("SelectionContext", "Deselecting: {}", Enum::ToString(Context));
				Selection.clear();
			}
		}

		FORCEINLINE static void DeselectAll(const ESelectionContext Context)
		{
			LK_CORE_DEBUG_TAG("SelectionContext", "Deselecting all for: {}", Enum::ToString(Context));
			auto& SelectionContext = SelectionContextMap.at(Context);
			SelectionContext.clear();
		}

	#if 0
		FORCEINLINE static bool IsSelected(const LEntity InEntity)
		{
			return (SelectedHandle == InEntity.GetUUID());
		}
	#endif
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

	#if 0
		FORCEINLINE void SelectEntity(const LEntity InEntity)
		{
			SelectedHandle = InEntity.GetUUID();
		#if 0
			GSelectedObject = InEntity;
			OnObjectSelectionChanged.Broadcast(InEntity);
		#endif
		}
	#endif

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

		FOnObjectSelectionChanged OnObjectSelectionChanged;

	private:
		LCLASS(LSelectionContext)
	};

}
