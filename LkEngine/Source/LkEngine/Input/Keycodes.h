#pragma once

#include <stdint.h>


namespace LkEngine {

	enum class EKey : uint16_t 
	{
		Space = 32,
		Apostrophe = 39,
		Comma = 44,
		Minus = 45, 
		Period = 46, 
		Slash = 47, 
		D0 = 48, 
		D1 = 49, 
		D2 = 50, 
		D3 = 51, 
		D4 = 52, 
		D5 = 53, 
		D6 = 54, 
		D7 = 55, 
		D8 = 56, 
		D9 = 57, 
		Semicolon = 59,
		Equal = 61,
		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,
		LeftBracket = 91,
		Backslash = 92,
		RightBracket = 93,
		GraveAccent = 96,
		/* Function keys */
		Escape = 256,
		Enter = 257,
		Tab = 258,
		Backspace = 259,
		Insert = 260,
		Delete = 261,
		Right = 262,
		Left = 263,
		Down = 264,
		Up = 265,
		PageUp = 266,
		PageDown = 267,
		Home = 268,
		End = 269,
		CapsLock = 280,
		ScrollLock = 281,
		NumLock = 282,
		PrintScreen = 283,
		Pause = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		Menu = 348
	};

	enum class EKeyState
	{
		None = -1,
		Pressed,
		Held,
		Released
	};

	/// @FIXME: Temporary until metadata is available.
	namespace Enum
	{
		FORCEINLINE static constexpr const char* ToString(const EKeyState State)
		{
			switch (State)
			{
				case EKeyState::None:      return "None";
				case EKeyState::Pressed:   return "Pressed";
				case EKeyState::Held:      return "Held";
				case EKeyState::Released:  return "Released";
			}

			assert(false);
			return nullptr;
		}
	}

}
