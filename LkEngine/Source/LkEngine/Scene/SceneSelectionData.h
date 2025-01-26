#pragma once

#include "LkEngine/Core/Delegate/Delegate.h"


namespace LkEngine {

	class LEntity;
	class LSubmesh;

	/**
	 * FSelectionData
	 * 
	 *  Selection data in the editor.
	 *  Used in raycasting contexts.
	 */
	struct FSceneSelectionData
	{
		LEntity Entity;
		LSubmesh* Mesh{};
		float Distance = 0.0f;
	};

	LK_DECLARE_MULTICAST_DELEGATE(FOnSceneSelectionUpdated, const std::vector<FSceneSelectionData>&);

}

