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

		/* Viewport Scalers. */
	#if 0
		Scalers.X = (Size.X / ViewportBounds[1].X);
		Scalers.Y = (Size.Y / ViewportBounds[1].Y);
	#endif

		Size.X = (ViewportBounds[1].X - ViewportBounds[0].X); 
		Size.Y = (ViewportBounds[1].Y - ViewportBounds[0].Y);

		if (bDirty)
		{
			LK_CORE_DEBUG_TAG("Viewport", "Broadcasting size {}", Size.ToString());
			OnSizeUpdated.Broadcast(Size.X, Size.Y);

			bDirty = false;
		}
	}

}

