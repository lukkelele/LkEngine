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


/// TODO: Move from UI directory into Core

namespace LkEngine {

	/// TESTING ONLY
	/// REMOVE
	LK_DECLARE_MULTICAST_DELEGATE(FOnSelectedEntityChanged, const LObject&);
	LK_DECLARE_DELEGATE(FOnSelectedEntityChangedSingle, const LObject&);

	/** Global, selected entity. */
	extern TObjectPtr<LObject> GSelectedObject;

	class LSelectionContext : public LObject
	{
	public:
		LSelectionContext();
		~LSelectionContext() = default;

		static LSelectionContext& Get();

		template<typename TLambda>
		FORCEINLINE static void Register(TLambda&& InLambda)
		{
			using TDelegate = std::decay_t<decltype(OnSelectedEntityChanged)>;
			if constexpr (IsMulticastDelegate_v<TDelegate>)
			{
				LK_CORE_DEBUG_TAG("SelectionContext", "(NO ARGS) Registering lambda to OnSelectedEntityChanged  (MULTICAST DELEGATE)");
				OnSelectedEntityChanged.Add(std::forward<TLambda>(InLambda));
			}
			else
			{
				LK_CORE_DEBUG_TAG("SelectionContext", "(NO ARGS) Registering lambda        (SINGLE DELEGATE)");
				OnSelectedEntityChangedSingle.BindLambda(std::forward<TLambda>(InLambda));
			}
		}

		template<typename TLambda, typename... TArgs>
		FORCEINLINE static void Register(TLambda&& InLambda, TArgs&&... Args)
		{
			using TDelegate = std::decay_t<decltype(OnSelectedEntityChangedSingle)>;
			if constexpr (IsMulticastDelegate_v<TDelegate>)
			{
				LK_CORE_DEBUG_TAG("SelectionContext", "(ARGS) Registering lambda to OnSelectedEntityChanged  (MULTICAST DELEGATE)");
				OnSelectedEntityChanged.Add(std::forward<TLambda>(InLambda), std::forward<TArgs>(Args)...);
			}
			else
			{
				LK_CORE_DEBUG_TAG("SelectionContext", "(NO ARGS) Registering lambda        (SINGLE DELEGATE)");
				OnSelectedEntityChangedSingle.BindLambda(std::forward<TLambda>(InLambda), std::forward<TArgs>(Args)...);
			}
		}

		static void SelectEntity(LEntity InEntity)
		{
			LK_UNUSED(InEntity);
		}

		static void Clear()
		{
		}

		static void ClearEntitySelection()
		{
		}

	public:
		inline static FOnSelectedEntityChanged OnSelectedEntityChanged;
		inline static FOnSelectedEntityChangedSingle OnSelectedEntityChangedSingle;

	private:
		LCLASS(LSelectionContext)
	};

}