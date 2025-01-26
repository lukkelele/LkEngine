#pragma once

#include <stdint.h>

#include "LkEngine/Core/Log/Log.h"
#include "LkEngine/Core/Assert.h"


namespace LkEngine {

	/**
	 * EKey
	 *
	 *  Key code.
	 */
	enum class EKey : uint16_t
	{
		None = 0,

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

	/**
	 * EKeyState
	 *
	 *  State of a key.
	 */
	enum class EKeyState
	{
		None = -1,
		Pressed,
		Held,
		Released
	};

	namespace Enum 
	{
		FORCEINLINE static constexpr const char* ToString(const EKey Key)
		{
			switch (Key)
			{
				case EKey::None:		 return "None";
				case EKey::Space:		 return "Space";
				case EKey::Apostrophe:	 return "Apostrophe";
				case EKey::Comma:		 return "Comma";
				case EKey::Minus:		 return "Minus";
				case EKey::Period:		 return "Period";
				case EKey::Slash:		 return "Slash";
				case EKey::D0:			 return "D0";
				case EKey::D1:			 return "D1";
				case EKey::D2:			 return "D2";
				case EKey::D3:			 return "D3";
				case EKey::D4:			 return "D4";
				case EKey::D5:			 return "D5";
				case EKey::D6:			 return "D6";
				case EKey::D7:			 return "D7";
				case EKey::D8:			 return "D8";
				case EKey::D9:			 return "D9";
				case EKey::Semicolon:	 return "Semicolon";
				case EKey::Equal:		 return "Equal";
				case EKey::A:			 return "A";
				case EKey::B:			 return "B";
				case EKey::C:			 return "C";
				case EKey::D:			 return "D";
				case EKey::E:			 return "E";
				case EKey::F:			 return "F";
				case EKey::G:			 return "G";
				case EKey::H:			 return "H";
				case EKey::I:			 return "I";
				case EKey::J:			 return "J";
				case EKey::K:			 return "K";
				case EKey::L:			 return "L";
				case EKey::M:			 return "M";
				case EKey::N:			 return "N";
				case EKey::O:			 return "O";
				case EKey::P:			 return "P";
				case EKey::Q:			 return "Q";
				case EKey::R:			 return "R";
				case EKey::S:			 return "S";
				case EKey::T:			 return "T";
				case EKey::U:			 return "U";
				case EKey::V:			 return "V";
				case EKey::W:			 return "W";
				case EKey::X:			 return "X";
				case EKey::Y:			 return "Y";
				case EKey::Z:			 return "Z";
				case EKey::LeftBracket:	 return "LeftBracket";
				case EKey::Backslash:	 return "Backslash";
				case EKey::RightBracket: return "RightBracket";
				case EKey::GraveAccent:	 return "GraveAccent";
				case EKey::Escape:		 return "Escape";
				case EKey::Enter:		 return "Enter";
				case EKey::Tab:			 return "Tab";
				case EKey::Backspace:	 return "Backspace";
				case EKey::Insert:		 return "Insert";
				case EKey::Delete:		 return "Delete";
				case EKey::Right:		 return "Right";
				case EKey::Left:		 return "Left";
				case EKey::Down:		 return "Down";
				case EKey::Up:			 return "Up";
				case EKey::PageUp:		 return "PageUp";
				case EKey::PageDown:	 return "PageDown";
				case EKey::Home:		 return "Home";
				case EKey::End:			 return "End";
				case EKey::CapsLock:	 return "CapsLock";
				case EKey::ScrollLock:	 return "ScrollLock";
				case EKey::NumLock:		 return "NumLock";
				case EKey::PrintScreen:	 return "PrintScreen";
				case EKey::Pause:		 return "Pause";
				case EKey::F1:			 return "F1";
				case EKey::F2:			 return "F2";
				case EKey::F3:			 return "F3";
				case EKey::F4:			 return "F4";
				case EKey::F5:			 return "F5";
				case EKey::F6:			 return "F6";
				case EKey::F7:			 return "F7";
				case EKey::F8:			 return "F8";
				case EKey::F9:			 return "F9";
				case EKey::LeftShift:	 return "LeftShift";
				case EKey::LeftControl:	 return "LeftControl";
				case EKey::LeftAlt:		 return "LeftAlt";
				case EKey::LeftSuper:	 return "LeftSuper";
				case EKey::RightShift:	 return "RightShift";
				case EKey::RightControl: return "RightControl";
				case EKey::RightAlt:	 return "RightAlt";
				case EKey::RightSuper:	 return "RightSuper";
				case EKey::Menu:		 return "Menu";
			}

			LK_CORE_VERIFY(false, "Enum::ToString(EKey) failed with value: {}", static_cast<uint16_t>(Key));
			return nullptr;
		}

		FORCEINLINE static constexpr const char* ToString(const EKeyState State)
		{
			switch (State)
			{
				case EKeyState::None:	  return "None";
				case EKeyState::Pressed:  return "Pressed";
				case EKeyState::Held:	  return "Held";
				case EKeyState::Released: return "Released";
			}

			LK_CORE_VERIFY(false, "Enum::ToString(EKeyState) failed with value: {}", static_cast<int>(State));
			return nullptr;
		}

	}

}

