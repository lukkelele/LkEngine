#pragma once

#include "LkEngine/Core/Delegate/Delegate.h"
#include "LkEngine/Core/Input/MouseCodes.h"
#include "LkEngine/Core/Math/Vector.h"

#include <glm/glm.hpp>


namespace LkEngine {

	///
	/// FIXME: UPDATE THIS IMPL
	/// ALSO RENAME CLASS
	///

	class LMouse
	{
	public:
		LK_DECLARE_MULTICAST_DELEGATE(FOnMouseButtonPressed, const FMouseButtonData&);
		LK_DECLARE_MULTICAST_DELEGATE(FOnMouseButtonReleased, const FMouseButtonData&);
		LK_DECLARE_MULTICAST_DELEGATE(FOnMouseScrolled, const EMouseScrollDirection);
		LK_DECLARE_MULTICAST_DELEGATE(FOnCursorModeChanged, const ECursorMode);
		LK_DECLARE_MULTICAST_DELEGATE(FOnMouseEnabled, const bool);
	public:
		static void Initialize();

		static glm::vec2 GetPos();
		static LVector2 GetPosition();
		static glm::vec2 GetRawPos();

		static void Enable();
		static void Disable();
		static bool IsEnabled();

		static bool IsButtonPressed(const EMouseButton MouseButton);

		static float GetMouseX();
		static float GetMouseY();

		static void SetPos(float x, float y); 
		static void SetScaledPos(float x, float y);
		static glm::vec2 GetCenterPos(); 
		static glm::vec2 GetScaledPos();
		static glm::vec2 GetScaledCenterPos();

	public:
		inline static glm::vec2 Pos = { 0.0f, 0.0f };
		inline static glm::vec2 ScaledPos = { 1.0f, 1.0f };

		inline static glm::vec2 CenterPos = { 0.0f, 0.0f }; 
		inline static glm::vec2 ScaledCenterPos = { 0.0f, 0.0f };

		inline static FOnMouseEnabled OnMouseEnabled;
		inline static FOnMouseButtonPressed OnMouseButtonPressed;
		inline static FOnMouseButtonReleased OnMouseButtonReleased;
		inline static FOnMouseScrolled OnMouseScrolled;
		inline static FOnCursorModeChanged OnCursorModeChanged;

	private:
		friend class LWindow;
	};

}
