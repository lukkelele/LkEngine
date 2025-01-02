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

		FORCEINLINE static void Select(const LEntity InEntity)
		{
			LK_CORE_ASSERT(InEntity.GetUUID());
			SelectedEntities.clear();
			SelectedEntityID = InEntity.GetUUID();
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

		FORCEINLINE static void Deselect(const ESelectionContext Context, const LUUID Handle)
		{
			LK_CORE_ASSERT(SelectionContextMap.contains(Context), "Selection context '{}' is missing", Enum::ToString(Context));
			std::vector<LUUID>& SelectionContext = SelectionContextMap.at(Context);
		#if 0
			auto Iter = std::find(SelectionContext.begin(), SelectionContext.end(), Handle);
			if (Iter != SelectionContext.end())
			{
				//std::erase(SelectionContext, Iter);
				SelectionContext.erase(Iter);
			}
		#endif
		}

		FORCEINLINE static bool DeselectAll()
		{
			LK_CORE_DEBUG_TAG("SelectionContext", "DeselectAll");
			for (auto& [Context, SelectionArray] : SelectionContextMap)
			{
				LK_CORE_DEBUG_TAG("SelectionContext", "Deselecting: {}", Enum::ToString(Context));
			}

			return true;
		}

		FORCEINLINE static bool DeselectAll(const ESelectionContext Context)
		{
			LK_CORE_DEBUG_TAG("SelectionContext", "DeselectAll: {}", Enum::ToString(Context));
			return true;
		}

		FORCEINLINE static bool IsSelected(const LEntity InEntity)
		{
			return (SelectedEntityID == InEntity.GetUUID());
		}

		/* TODO: Implement this fully. */
		FORCEINLINE static bool IsSelected(const ESelectionContext Context, const LUUID Handle)
		{
			LK_CORE_ASSERT(SelectionContextMap.contains(Context), "Selection context '{}' is missing", Enum::ToString(Context));
			LK_CORE_ASSERT(Handle > 0, "AssetHandle is 0");
			const std::vector<LUUID>& SelectionContext = SelectionContextMap.at(Context);
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

		FORCEINLINE void SelectEntity(const LEntity InEntity)
		{
			SelectedEntityID = InEntity.GetUUID();
		#if 0
			GSelectedObject = InEntity;
			OnObjectSelectionChanged.Broadcast(InEntity);
		#endif
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
		static LUUID SelectedEntityID;
		static std::vector<LUUID> SelectedEntities;
		static std::unordered_map<ESelectionContext, std::vector<LUUID>> SelectionContextMap;

		FOnObjectSelectionChanged OnObjectSelectionChanged;

	private:
		LCLASS(LSelectionContext)
	};

}
