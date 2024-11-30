#pragma once

#include <string_view>

#include <random>
#include <time.h>

#include <glm/glm.hpp>


/* Colors. */
#define LK_COLOR_RESET              "\033[0m"
/* Foreground Colors */
#define LK_COLOR_BLACK              "\033[30m"
#define LK_COLOR_RED                "\033[31m"
#define LK_COLOR_GREEN              "\033[32m"
#define LK_COLOR_YELLOW             "\033[33m"
#define LK_COLOR_BLUE               "\033[34m"
#define LK_COLOR_MAGENTA            "\033[35m"
#define LK_COLOR_CYAN               "\033[36m"
#define LK_COLOR_WHITE              "\033[37m"
#define LK_COLOR_BRIGHT_BLACK       "\033[90m"
#define LK_COLOR_BRIGHT_RED         "\033[91m"
#define LK_COLOR_BRIGHT_GREEN       "\033[92m"
#define LK_COLOR_BRIGHT_YELLOW      "\033[93m"
#define LK_COLOR_BRIGHT_BLUE        "\033[94m"
#define LK_COLOR_BRIGHT_MAGENTA     "\033[95m"
#define LK_COLOR_BRIGHT_CYAN        "\033[96m"
#define LK_COLOR_BRIGHT_WHITE       "\033[97m"
/* Background Colors */
#define LK_COLOR_BG_BLACK           "\033[40m"
#define LK_COLOR_BG_RED             "\033[41m"
#define LK_COLOR_BG_GREEN           "\033[42m"
#define LK_COLOR_BG_YELLOW          "\033[43m"
#define LK_COLOR_BG_BLUE            "\033[44m"
#define LK_COLOR_BG_MAGENTA         "\033[45m"
#define LK_COLOR_BG_CYAN            "\033[46m"
#define LK_COLOR_BG_WHITE           "\033[47m"
#define LK_COLOR_BG_BRIGHT_BLACK    "\033[100m"
#define LK_COLOR_BG_BRIGHT_RED      "\033[101m"
#define LK_COLOR_BG_BRIGHT_GREEN    "\033[102m"
#define LK_COLOR_BG_BRIGHT_YELLOW   "\033[103m"
#define LK_COLOR_BG_BRIGHT_BLUE     "\033[104m"
#define LK_COLOR_BG_BRIGHT_MAGENTA  "\033[105m"
#define LK_COLOR_BG_BRIGHT_CYAN     "\033[106m"
#define LK_COLOR_BG_BRIGHT_WHITE    "\033[107m"


namespace LkEngine::Color {

	extern std::mt19937 RNG;
	extern std::uniform_int_distribution<int> Dist;

	/* clang-format off */

    namespace RGBA 
    {
        extern glm::vec4 Transparent;
        extern glm::vec4 White;
        extern glm::vec4 Black;
        extern glm::vec4 Gray;
        extern glm::vec4 Red;
        extern glm::vec4 Green;
        extern glm::vec4 Blue;
    }


	/**
	 * TODO: Move this to a 'Core' subfolder as this is not entirely 
	 * tied to the Renderer. 
	 */
	/*---------------------------------------------------
	                      Colors
	----------------------------------------------------*/

	static constexpr uint16_t Black   = 0x0000;
	static constexpr uint16_t Blue    = 0x0001;
	static constexpr uint16_t Green   = 0x0002;
	static constexpr uint16_t Cyan    = 0x0003;
	static constexpr uint16_t Red     = 0x0004;
	static constexpr uint16_t Magenta = 0x0005;
	static constexpr uint16_t Yellow  = 0x0006;
	static constexpr uint16_t White   = 0x0007;

	namespace Formatter 
	{
		static constexpr uint16_t Reset     = 0x0000;
		static constexpr uint16_t Bold      = 0x0100; 
		static constexpr uint16_t Dark      = 0x0200;
		static constexpr uint16_t Underline = 0x0400;
		static constexpr uint16_t Blink     = 0x0800;
		static constexpr uint16_t Reverse   = 0x1000;
		static constexpr uint16_t Concealed = 0x2000;
		static constexpr uint16_t ClearLine = 0x4000;
	}

	namespace Background 
	{
		static constexpr uint16_t Black   = 0x0000;
		static constexpr uint16_t Red     = 0x0010;
		static constexpr uint16_t Green   = 0x0020;
		static constexpr uint16_t Yellow  = 0x0030;
		static constexpr uint16_t Blue    = 0x0040;
		static constexpr uint16_t Magenta = 0x0050;
		static constexpr uint16_t Cyan    = 0x0060;
		static constexpr uint16_t White   = 0x0070;
	}

	namespace Bold 
	{
		static constexpr uint16_t Black   = 0xFF00;
		static constexpr uint16_t Blue    = 0xFF01;
		static constexpr uint16_t Green   = 0xFF02;
		static constexpr uint16_t Cyan    = 0xFF03;
		static constexpr uint16_t Red     = 0xFF04;
		static constexpr uint16_t Magenta = 0xFF05;
		static constexpr uint16_t Yellow  = 0xFF06;
		static constexpr uint16_t White   = 0xFF07;
	}

	namespace ANSI
	{
		/* Formatting codes. */
		constexpr std::string_view Reset          = "\033[m";
		constexpr std::string_view Bold           = "\033[1m";
		constexpr std::string_view Dark           = "\033[2m";
		constexpr std::string_view Underline      = "\033[4m";
		constexpr std::string_view Blink          = "\033[5m";
		constexpr std::string_view Reverse        = "\033[7m";
		constexpr std::string_view Concealed      = "\033[8m";
		constexpr std::string_view ClearLine      = "\033[K";

		/* Foreground colors. */
		constexpr std::string_view Black          = "\033[30m";
		constexpr std::string_view Red            = "\033[31m";
		constexpr std::string_view Green          = "\033[32m";
		constexpr std::string_view Yellow         = "\033[33m";
		constexpr std::string_view Blue           = "\033[34m";
		constexpr std::string_view Magenta        = "\033[35m";
		constexpr std::string_view Cyan           = "\033[36m";
		constexpr std::string_view White          = "\033[37m";

		/* Background colors. */
		namespace Bg
		{
			constexpr std::string_view Black        = "\033[40m";
			constexpr std::string_view Red          = "\033[41m";
			constexpr std::string_view Green        = "\033[42m";
			constexpr std::string_view Yellow       = "\033[43m";
			constexpr std::string_view Blue         = "\033[44m";
			constexpr std::string_view Magenta      = "\033[45m";
			constexpr std::string_view Cyan         = "\033[46m";
			constexpr std::string_view White        = "\033[47m";
		}

		/* Bold colors. */
		constexpr std::string_view YellowBold     = "\033[33m\033[1m";
		constexpr std::string_view RedBold        = "\033[31m\033[1m";
		constexpr std::string_view BoldOnRed      = "\033[1m\033[41m";
	}

	enum class RGB : uint32_t
	{
		AliceBlue             = 0xF0F8FF,  // RGB(240,248,255)
		AntiqueWhite          = 0xFAEBD7,  // RGB(250,235,215)
		Aqua                  = 0x00FFFF,  // RGB(0,255,255)
		Aquamarine            = 0x7FFFD4,  // RGB(127,255,212)
		Azure                 = 0xF0FFFF,  // RGB(240,255,255)
		Beige                 = 0xF5F5DC,  // RGB(245,245,220)
		Bisque                = 0xFFE4C4,  // RGB(255,228,196)
		Black                 = 0x000000,  // RGB(0,0,0)
		BlanchedAlmond        = 0xFFEBCD,  // RGB(255,235,205)
		Blue                  = 0x0000FF,  // RGB(0,0,255)
		BlueViolet            = 0x8A2BE2,  // RGB(138,43,226)
		Brown                 = 0xA52A2A,  // RGB(165,42,42)
		BurlyWood             = 0xDEB887,  // RGB(222,184,135)
		CadetBlue             = 0x5F9EA0,  // RGB(95,158,160)
		Chartreuse            = 0x7FFF00,  // RGB(127,255,0)
		Chocolate             = 0xD2691E,  // RGB(210,105,30)
		Coral                 = 0xFF7F50,  // RGB(255,127,80)
		CornflowerBlue        = 0x6495ED,  // RGB(100,149,237)
		Cornsilk              = 0xFFF8DC,  // RGB(255,248,220)
		Crimson               = 0xDC143C,  // RGB(220,20,60)
		Cyan                  = 0x00FFFF,  // RGB(0,255,255)
		DarkBlue              = 0x00008B,  // RGB(0,0,139)
		DarkCyan              = 0x008B8B,  // RGB(0,139,139)
		DarkGoldenRod         = 0xB8860B,  // RGB(184,134,11)
		DarkGray              = 0xA9A9A9,  // RGB(169,169,169)
		DarkGreen             = 0x006400,  // RGB(0,100,0)
		DarkKhaki             = 0xBDB76B,  // RGB(189,183,107)
		DarkMagenta           = 0x8B008B,  // RGB(139,0,139)
		DarkOliveGreen        = 0x556B2F,  // RGB(85,107,47)
		DarkOrange            = 0xFF8C00,  // RGB(255,140,0)
		DarkOrchid            = 0x9932CC,  // RGB(153,50,204)
		DarkRed               = 0x8B0000,  // RGB(139,0,0)
		DarkSalmon            = 0xE9967A,  // RGB(233,150,122)
		DarkSeaGreen          = 0x8FBC8F,  // RGB(143,188,143)
		DarkSlateBlue         = 0x483D8B,  // RGB(72,61,139)
		DarkSlateGray         = 0x2F4F4F,  // RGB(47,79,79)
		DarkTurquoise         = 0x00CED1,  // RGB(0,206,209)
		DarkViolet            = 0x9400D3,  // RGB(148,0,211)
		DeepPink              = 0xFF1493,  // RGB(255,20,147)
		DeepSkyBlue           = 0x00BFFF,  // RGB(0,191,255)
		DimGray               = 0x696969,  // RGB(105,105,105)
		DodgerBlue            = 0x1E90FF,  // RGB(30,144,255)
		FireBrick             = 0xB22222,  // RGB(178,34,34)
		FloralWhite           = 0xFFFAF0,  // RGB(255,250,240)
		ForestGreen           = 0x228B22,  // RGB(34,139,34)
		Fuchsia               = 0xFF00FF,  // RGB(255,0,255)
		Gainsboro             = 0xDCDCDC,  // RGB(220,220,220)
		GhostWhite            = 0xF8F8FF,  // RGB(248,248,255)
		Gold                  = 0xFFD700,  // RGB(255,215,0)
		GoldenRod             = 0xDAA520,  // RGB(218,165,32)
		Gray                  = 0x808080,  // RGB(128,128,128)
		Green                 = 0x008000,  // RGB(0,128,0)
		GreenYellow           = 0xADFF2F,  // RGB(173,255,47)
		HoneyDew              = 0xF0FFF0,  // RGB(240,255,240)
		HotPink               = 0xFF69B4,  // RGB(255,105,180)
		IndianRed             = 0xCD5C5C,  // RGB(205,92,92)
		Indigo                = 0x4B0082,  // RGB(75,0,130)
		Ivory                 = 0xFFFFF0,  // RGB(255,255,240)
		Khaki                 = 0xF0E68C,  // RGB(240,230,140)
		Lavender              = 0xE6E6FA,  // RGB(230,230,250)
		LavenderBlush         = 0xFFF0F5,  // RGB(255,240,245)
		LawnGreen             = 0x7CFC00,  // RGB(124,252,0)
		LemonChiffon          = 0xFFFACD,  // RGB(255,250,205)
		LightBlue             = 0xADD8E6,  // RGB(173,216,230)
		LightCoral            = 0xF08080,  // RGB(240,128,128)
		LightCyan             = 0xE0FFFF,  // RGB(224,255,255)
		LightGoldenRodYellow  = 0xFAFAD2,  // RGB(250,250,210)
		LightGray             = 0xD3D3D3,  // RGB(211,211,211)
		LightGreen            = 0x90EE90,  // RGB(144,238,144)
		LightPink             = 0xFFB6C1,  // RGB(255,182,193)
		LightSalmon           = 0xFFA07A,  // RGB(255,160,122)
		LightSeaGreen         = 0x20B2AA,  // RGB(32,178,170)
		LightSkyBlue          = 0x87CEFA,  // RGB(135,206,250)
		LightSlateGray        = 0x778899,  // RGB(119,136,153)
		LightSteelBlue        = 0xB0C4DE,  // RGB(176,196,222)
		LightYellow           = 0xFFFFE0,  // RGB(255,255,224)
		Lime                  = 0x00FF00,  // RGB(0,255,0)
		LimeGreen             = 0x32CD32,  // RGB(50,205,50)
		Linen                 = 0xFAF0E6,  // RGB(250,240,230)
		Magenta               = 0xFF00FF,  // RGB(255,0,255)
		Maroon                = 0x800000,  // RGB(128,0,0)
		MediumAquamarine      = 0x66CDAA,  // RGB(102,205,170)
		MediumBlue            = 0x0000CD,  // RGB(0,0,205)
		MediumOrchid          = 0xBA55D3,  // RGB(186,85,211)
		MediumPurple          = 0x9370DB,  // RGB(147,112,219)
		MediumSeaGreen        = 0x3CB371,  // RGB(60,179,113)
		MediumSlateBlue       = 0x7B68EE,  // RGB(123,104,238)
		MediumSpringGreen     = 0x00FA9A,  // RGB(0,250,154)
		MediumTurquoise       = 0x48D1CC,  // RGB(72,209,204)
		MediumVioletRed       = 0xC71585,  // RGB(199,21,133)
		MidnightBlue          = 0x191970,  // RGB(25,25,112)
		MintCream             = 0xF5FFFA,  // RGB(245,255,250)
		MistyRose             = 0xFFE4E1,  // RGB(255,228,225)
		Moccasin              = 0xFFE4B5,  // RGB(255,228,181)
		NavajoWhite           = 0xFFDEAD,  // RGB(255,222,173)
		Navy                  = 0x000080,  // RGB(0,0,128)
		OldLace               = 0xFDF5E6,  // RGB(253,245,230)
		Olive                 = 0x808000,  // RGB(128,128,0)
		OliveDrab             = 0x6B8E23,  // RGB(107,142,35)
		Orange                = 0xFFA500,  // RGB(255,165,0)
		OrangeRed             = 0xFF4500,  // RGB(255,69,0)
		Orchid                = 0xDA70D6,  // RGB(218,112,214)
		PaleGoldenRod         = 0xEEE8AA,  // RGB(238,232,170)
		PaleGreen             = 0x98FB98,  // RGB(152,251,152)
		PaleTurquoise         = 0xAFEEEE,  // RGB(175,238,238)
		PaleVioletRed         = 0xDB7093,  // RGB(219,112,147)
		PapayaWhip            = 0xFFEFD5,  // RGB(255,239,213)
		PeachPuff             = 0xFFDAB9,  // RGB(255,218,185)
		Peru                  = 0xCD853F,  // RGB(205,133,63)
		Pink                  = 0xFFC0CB,  // RGB(255,192,203)
		Plum                  = 0xDDA0DD,  // RGB(221,160,221)
		PowderBlue            = 0xB0E0E6,  // RGB(176,224,230)
		Purple                = 0x800080,  // RGB(128,0,128)
		RebeccaPurple         = 0x663399,  // RGB(102,51,153)
		Red                   = 0xFF0000,  // RGB(255,0,0)
		RosyBrown             = 0xBC8F8F,  // RGB(188,143,143)
		RoyalBlue             = 0x4169E1,  // RGB(65,105,225)
		SaddleBrown           = 0x8B4513,  // RGB(139,69,19)
		Salmon                = 0xFA8072,  // RGB(250,128,114)
		SandyBrown            = 0xF4A460,  // RGB(244,164,96)
		SeaGreen              = 0x2E8B57,  // RGB(46,139,87)
		SeaShell              = 0xFFF5EE,  // RGB(255,245,238)
		Sienna                = 0xA0522D,  // RGB(160,82,45)
		Silver                = 0xC0C0C0,  // RGB(192,192,192)
		SkyBlue               = 0x87CEEB,  // RGB(135,206,235)
		SlateBlue             = 0x6A5ACD,  // RGB(106,90,205)
		SlateGray             = 0x708090,  // RGB(112,128,144)
		Snow                  = 0xFFFAFA,  // RGB(255,250,250)
		SpringGreen           = 0x00FF7F,  // RGB(0,255,127)
		SteelBlue             = 0x4682B4,  // RGB(70,130,180)
		Tan                   = 0xD2B48C,  // RGB(210,180,140)
		Teal                  = 0x008080,  // RGB(0,128,128)
		Thistle               = 0xD8BFD8,  // RGB(216,191,216)
		Tomato                = 0xFF6347,  // RGB(255,99,71)
		Turquoise             = 0x40E0D0,  // RGB(64,224,208)
		Violet                = 0xEE82EE,  // RGB(238,130,238)
		Wheat                 = 0xF5DEB3,  // RGB(245,222,179)
		White                 = 0xFFFFFF,  // RGB(255,255,255)
		WhiteSmoke            = 0xF5F5F5,  // RGB(245,245,245)
		Yellow                = 0xFFFF00,  // RGB(255,255,0)
		YellowGreen           = 0x9ACD32   // RGB(154,205,50)
	};

	enum class EColorCode
	{
		Reset,

		/* Foreground Colors. */
		Black,
		Red,
		Green,
		Yellow,
		Blue,
		Magenta,
		Cyan,
		White,
		BrightBlack,
		BrightRed,
		BrightGreen,
		BrightYellow,
		BrightBlue,
		BrightMagenta,
		BrightCyan,
		BrightWhite,

		/* Background Colors. */
		BgBlack,
		BgRed,
		BgGreen,
		BgYellow,
		BgBlue,
		BgMagenta,
		BgCyan,
		BgWhite,
		BgBrightBlack,
		BgBrightRed,
		BgBrightGreen,
		BgBrightYellow,
		BgBrightBlue,
		BgBrightMagenta,
		BgBrightCyan,
		BgBrightWhite,

		COUNT /* Keep last. */
	};

	/* ANSI Escape Codes mapped to EColorCode. */
	static constexpr std::array<const char*, static_cast<std::size_t>(EColorCode::COUNT)> ColorEscapeCodes = {
		"\033[0m",    // Reset

		/* Foreground Colors. */
		"\033[30m",   // Black
		"\033[31m",   // Red
		"\033[32m",   // Green
		"\033[33m",   // Yellow
		"\033[34m",   // Blue
		"\033[35m",   // Magenta
		"\033[36m",   // Cyan
		"\033[37m",   // White
		"\033[90m",   // Bright Black
		"\033[91m",   // Bright Red
		"\033[92m",   // Bright Green
		"\033[93m",   // Bright Yellow
		"\033[94m",   // Bright Blue
		"\033[95m",   // Bright Magenta
		"\033[96m",   // Bright Cyan
		"\033[97m",   // Bright White

		/* Background Colors. */
		"\033[40m",   // BgBlack
		"\033[41m",   // BgRed
		"\033[42m",   // BgGreen
		"\033[43m",   // BgYellow
		"\033[44m",   // BgBlue
		"\033[45m",   // BgMagenta
		"\033[46m",   // BgCyan
		"\033[47m",   // BgWhite
		"\033[100m",  // BgBrightBlack
		"\033[101m",  // BgBrightRed
		"\033[102m",  // BgBrightGreen
		"\033[103m",  // BgBrightYellow
		"\033[104m",  // BgBrightBlue
		"\033[105m",  // BgBrightMagenta
		"\033[106m",  // BgBrightCyan
		"\033[107m"   // BgBrightWhite
	};

	static constexpr const char* GetEscapeCode(const EColorCode InColor)
	{
		return ColorEscapeCodes[static_cast<std::size_t>(InColor)];
	}

}
