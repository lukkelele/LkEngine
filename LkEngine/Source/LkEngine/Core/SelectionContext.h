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

	class LSelectionContext : public LObject
	{
	public:
		LSelectionContext();
		~LSelectionContext() = default;

		static LSelectionContext& Get();

		FORCEINLINE void SelectEntity(const LEntity InEntity)
		{
			GSelectedObject = InEntity;
			OnObjectSelectionChanged.Broadcast(InEntity);
		}

		FORCEINLINE void Clear()
		{
			/* TODO */
		}

	public:
		FOnObjectSelectionChanged OnObjectSelectionChanged;
	private:
		LCLASS(LSelectionContext)
	};

}