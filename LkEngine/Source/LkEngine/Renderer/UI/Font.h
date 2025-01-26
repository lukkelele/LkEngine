#pragma once

#include <string>
#include <string_view>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

/* Use newer font registration system with changes to sizes. */
//#define LK_UI_FONT_REWORKED 


namespace LkEngine {

	enum class EFontSize
	{
		Regular = 0,
		Smaller,
		Small,
		Large,
		Larger,
		Title,
		Header,
	};

	extern const std::unordered_map<EFontSize, float> FontSizeMap;

	enum class EFont
	{
		Default = 0,
		SourceSansPro,
		Roboto,
	};

	struct FFontConfiguration
	{
		std::string FontName{};
		std::string FilePath{};
		float Size = 18.0f;

		bool MergeWithLast = false;
		const ImWchar* GlyphRanges = nullptr;
	};

	struct FFontEntry
	{
		std::string Name{};
		float Size = 0.0f;
		std::string FilePath{};

		bool operator==(const FFontEntry& Other) const
		{
			return (Name == Other.Name);
		}
	};

	/**
	 * Font
	 *
	 *  Manipulate fonts in the UI.
	 *  Uses Begin/End style of API.
	 */
	namespace UI::Font 
	{
		void Add(const FFontConfiguration& FontConfig, bool IsDefault = false);
		void Push(const std::string& FontName);
		void Pop();
		ImFont* Get(const std::string& FontName);

		std::size_t GetRegistered(std::vector<FFontEntry>& InFonts);
	}

}

namespace std 
{
    template<>
    struct hash<LkEngine::FFontEntry>
    {
        std::size_t operator()(const LkEngine::FFontEntry& Entry) const noexcept
        {
			std::size_t Hash = 0;
            for (const char Character : Entry.Name)
            {
				/* Common hash multiplier. */
			#if defined(LK_UI_FONT_REWORKED)
                Hash = (Hash * 31 + static_cast<unsigned char>(Character));
				Hash += Entry.Size;
			#else
				Hash = (Hash * 31 + static_cast<unsigned char>(Character));
			#endif
            }

            return Hash;
        }
    };
}

