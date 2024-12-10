#include "LKpch.h"
#include "Viewport.h"

#if defined(LK_ENGINE_OPENGL)
#include "LkEngine/Renderer/Backend/OpenGL/LkOpenGL.h"
#endif


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
			OnSizeUpdated.Broadcast(Size.X, Size.Y);
			bDirty = false;
		}
	}

	/* MOVE ELSEWHERE */
	void LViewport::SetViewport(const int PosX, const int PosY, const int Width, const int Height)
	{
	#if defined(LK_ENGINE_OPENGL)
		glViewport(PosX, PosY, Width, Height);
	#endif
	}

}

