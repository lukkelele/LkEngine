#include "LKpch.h"
#include "Viewport.h"


namespace LkEngine {

	LViewport::LViewport()
	{
		LCLASS_REGISTER();
	}

	void LViewport::Update()
	{
		LK_PROFILE_FUNC();

		if (bDirty)
		{
			//LK_CORE_DEBUG_TAG("Viewport", "Broadcasting size={}  Scalers={}", Size.ToString(), Scalers.ToString());
			OnSizeUpdated.Broadcast(Size.X, Size.Y);

			bDirty = false;
		}
	}

}

