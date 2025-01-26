#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "LkEngine/Core/Delegate/Delegate.h"
#include "LkEngine/Core/Input/MouseCodes.h"
#include "LkEngine/Core/Math/Vector.h"

#include <glm/glm.hpp>


namespace LkEngine {

	class LMouse
	{
	public:
		LK_DECLARE_MULTICAST_DELEGATE(FOnMouseButtonPressed, const FMouseButtonData&);
		LK_DECLARE_MULTICAST_DELEGATE(FOnMouseButtonReleased, const FMouseButtonData&);
		LK_DECLARE_MULTICAST_DELEGATE(FOnMouseScrolled, const EMouseScrollDirection);
		LK_DECLARE_MULTICAST_DELEGATE(FOnCursorModeChanged, const ECursorMode);
		LK_DECLARE_MULTICAST_DELEGATE(FOnMouseEnabled, const bool);
	public:
		static float GetMouseX();
		static float GetMouseY();

		static bool IsButtonPressed(const EMouseButton MouseButton);

		static void Enable();
		static void Disable();
		static bool IsEnabled();

	public:
		inline static FOnMouseEnabled OnMouseEnabled;
		inline static FOnMouseButtonPressed OnMouseButtonPressed;
		inline static FOnMouseButtonReleased OnMouseButtonReleased;
		inline static FOnMouseScrolled OnMouseScrolled;
		inline static FOnCursorModeChanged OnCursorModeChanged;
	};

}
