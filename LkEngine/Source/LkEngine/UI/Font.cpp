#include "LKpch.h"
#include "Font.h"

#include "LkEngine/Core/Core.h"
#include "LkEngine/Core/IO/FileSystem.h"


namespace LkEngine::UI {

	static std::unordered_map<FFontEntry, ImFont*> Fonts;

	void Font::Add(const FFontConfiguration& FontConfig, bool IsDefault)
	{
		LK_CORE_ASSERT(LFileSystem::Exists(FontConfig.FilePath), "Invalid font filepath");

		using EntryPair = std::pair<FFontEntry, ImFont*>;
		auto FindFont = [&FontConfig](const EntryPair& FontEntry) -> bool
		{
			return (FontEntry.first.Name == FontConfig.FontName);
		};
		if (auto Iter = std::find_if(Fonts.begin(), Fonts.end(), FindFont); Iter != Fonts.end())
		{
			LK_CORE_WARN("Failed to add font '{}', the name is already taken by another font", FontConfig.FontName);
			return;
		}

		ImFontConfig ImguiFontConfig;
		ImguiFontConfig.MergeMode = FontConfig.MergeWithLast;
		ImGuiIO& IO = ImGui::GetIO();
		ImFont* Font = IO.Fonts->AddFontFromFileTTF(
			FontConfig.FilePath.data(), 
			FontConfig.Size, 
			&ImguiFontConfig, 
			(FontConfig.GlyphRanges == nullptr ? IO.Fonts->GetGlyphRangesDefault() : FontConfig.GlyphRanges)
		);
		LK_CORE_VERIFY(Font, "Failed to load font");

		/* Populate the map with the registered font. */
		const FFontEntry FontEntry = {
			.Name = FontConfig.FontName,
			.Size = FontConfig.Size,
			.FilePath = FontConfig.FilePath
		};
		Fonts[FontEntry] = Font;

		if (IsDefault)
		{
			IO.FontDefault = Font;
		}
	}

	void Font::Push(const std::string& FontName)
	{
		using EntryPair = std::pair<FFontEntry, ImFont*>;
		auto FindFont = [&FontName](const EntryPair& FontEntry) -> bool
		{
			return (FontEntry.first.Name == FontName);
		};
		if (auto Iter = std::find_if(Fonts.begin(), Fonts.end(), FindFont); Iter != Fonts.end())
		{
			ImGui::PushFont(Iter->second);
			return;
		}

		/* Use default font if no font was found. */
		const ImGuiIO& IO = ImGui::GetIO();
		ImGui::PushFont(IO.FontDefault);
	}

	void Font::Pop()
	{
		ImGui::PopFont();
	}

	ImFont* Font::Get(const std::string& FontName)
	{
		using EntryPair = std::pair<FFontEntry, ImFont*>;
		auto FindFont = [&FontName](const EntryPair& FontEntry) -> bool
		{
			return (FontEntry.first.Name == FontName);
		};
		if (auto Iter = std::find_if(Fonts.begin(), Fonts.end(), FindFont); Iter != Fonts.end())
		{
			return Iter->second;
		}

		LK_VERIFY(false, "Failed to find font '{}'", FontName);
		return nullptr;
	}

	std::size_t Font::GetRegistered(std::vector<FFontEntry>& InFonts)
	{
		InFonts.clear();
		InFonts.reserve(Fonts.size());

		auto GetFontEntry = [](const auto& FontEntry) { return FontEntry.first; };
		std::ranges::transform(Fonts, std::back_inserter(InFonts), GetFontEntry);

		return InFonts.size();
	}


}
