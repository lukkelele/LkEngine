#pragma once

#include <cstdint>


namespace LkEngine {

	typedef enum class MouseButton : uint16_t
	{
		Button0 = 0,
		Button1 = 1,
		Button2 = 2,
		Button3 = 3,
		Button4 = 4,
		Button5 = 5,
		Left = Button0,
		Right = Button1,
		Middle = Button2
	} Button;

	enum class CursorMode
	{
		Normal = 0,
		Hidden = 1,
		Locked = 2
	};

}