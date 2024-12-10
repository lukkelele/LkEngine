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

	class LSelectionContext : public LObject
	{
	public:
		LSelectionContext();
		~LSelectionContext() = default;

		static LSelectionContext& Get();

		FORCEINLINE static void Select(const LEntity InEntity)
		{
			SelectedEntityID = InEntity.GetUUID();
		}

		FORCEINLINE void SelectEntity(const LEntity InEntity)
		{
			SelectedEntityID = InEntity.GetUUID();
		#if 0
			GSelectedObject = InEntity;
			OnObjectSelectionChanged.Broadcast(InEntity);
		#endif
		}

		FORCEINLINE void Clear()
		{
			/* TODO */
		}

	public:
		static UUID SelectedEntityID;
		FOnObjectSelectionChanged OnObjectSelectionChanged;
	private:
		LCLASS(LSelectionContext)
	};

}
