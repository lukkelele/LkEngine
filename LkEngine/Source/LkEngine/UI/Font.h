#pragma once

#include <string>
#include <string_view>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>


namespace LkEngine::UI {

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

	enum class EFont
	{
		Default = 0,
		SourceSansPro,
		Roboto,
	};

	struct FFontConfiguration
	{
		std::string FontName{};
		std::string_view FilePath{};
		float Size = 18.0f;

		bool MergeWithLast = false;
		const ImWchar* GlyphRanges = nullptr;
	};

	struct FFontEntry
	{
		std::string Name{};

		bool operator==(const FFontEntry& Other) const
		{
			return (Name == Other.Name);
		}
	};

	namespace Font 
	{
		void Add(const FFontConfiguration& FontConfig, bool IsDefault = false);
		void Push(const std::string& FontName);
		void Pop();
		ImFont* Get(const std::string& FontName);
	}

}

namespace std 
{
    template<>
    struct hash<LkEngine::UI::FFontEntry>
    {
        std::size_t operator()(const LkEngine::UI::FFontEntry& Entry) const noexcept
        {
			std::size_t Hash = 0;
            for (const char Character : Entry.Name)
            {
				/* Common hash multiplier. */
                Hash = (Hash * 31 + static_cast<unsigned char>(Character));
            }

            return Hash;
        }
    };
}
