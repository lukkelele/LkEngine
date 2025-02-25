#pragma once

#include "LkEngine/Renderer/UI/Font.h"


namespace LkEngine::UI {

	/**
	 * EStyle
	 *
	 *  Styles that can be added to UI elements.
	 */
	enum class EStyle : uint32_t
	{
		AlignHorizontal,
		COUNT
	};
	LK_ENUM_CLASS_FLAGS(EStyle);


	class FScopedStyle
	{
	public:
		template<typename T>
		FORCEINLINE FScopedStyle(const ImGuiStyleVar StyleVar, const T Value) { ImGui::PushStyleVar(StyleVar, Value); }
		~FScopedStyle() { ImGui::PopStyleVar(); }
		FScopedStyle(const FScopedStyle&) = delete;
		FScopedStyle& operator=(const FScopedStyle&) = delete;
	};

	class FScopedColor
	{
	public:
		template<typename T>
		FORCEINLINE FScopedColor(const ImGuiCol ColorID, const T Color) { ImGui::PushStyleColor(ColorID, ImColor(Color).Value); }
		~FScopedColor() { ImGui::PopStyleColor(); }
		FScopedColor(const FScopedColor&) = delete;
		FScopedColor& operator=(const FScopedColor&) = delete;
	};

	class FScopedID
	{
	public:
		template<typename T>
		FORCEINLINE FScopedID(const T ID) { ImGui::PushID(ID); }
		~FScopedID() { ImGui::PopID(); }
		FScopedID(const FScopedID&) = delete;
		FScopedID& operator=(const FScopedID&) = delete;
	};

	class FScopedColorStack
	{
	public:
		template <typename ColorType, typename... OtherColors>
		FORCEINLINE FScopedColorStack(const ImGuiCol FirstColorID, 
									  const ColorType FirstColor, 
									  OtherColors&& ... OtherColorPairs)
			: Count((sizeof... (OtherColorPairs) / 2) + 1)
		{
			static_assert((sizeof... (OtherColorPairs) & 1u) == 0, "FScopedColorStack expects a list of pairs of color IDs and colors");
			PushColor(FirstColorID, FirstColor, std::forward<OtherColors>(OtherColorPairs)...);
		}

		~FScopedColorStack() { ImGui::PopStyleColor(Count); }
		FScopedColorStack(const FScopedColorStack&) = delete;
		FScopedColorStack& operator=(const FScopedColorStack&) = delete;

	private:
		int Count = 0;

		template <typename ColorType, typename... OtherColors>
		FORCEINLINE void PushColor(const ImGuiCol ColorID, 
								   const ColorType Color, 
								   OtherColors&& ... OtherColorPairs)
		{
			if constexpr (sizeof... (OtherColorPairs) == 0)
			{
				ImGui::PushStyleColor(ColorID, ImColor(Color).Value);
			}
			else
			{
				ImGui::PushStyleColor(ColorID, ImColor(Color).Value);
				PushColor(std::forward<OtherColors>(OtherColorPairs)...);
			}
		}
	};

	class FScopedStyleStack
	{
	public:
		template<typename ValueType, typename... OtherStylePairs>
		FORCEINLINE FScopedStyleStack(const ImGuiStyleVar FirstStyleVar, 
									  const ValueType FirstValue, 
									  OtherStylePairs&& ... OtherPairs)
			: StackCount((sizeof...(OtherPairs) / 2) + 1)
		{
			static_assert((sizeof...(OtherPairs) & 1u) == 0);
			PushStyle(FirstStyleVar, FirstValue, std::forward<OtherStylePairs>(OtherPairs)...);
		}

		~FScopedStyleStack() { ImGui::PopStyleVar(StackCount); }
		FScopedStyleStack(const FScopedStyleStack&) = delete;
		FScopedStyleStack& operator=(const FScopedStyleStack&) = delete;

	private:
		int StackCount = 0;

		template<typename ValueType, typename... OtherStylePairs>
		FORCEINLINE void PushStyle(const ImGuiStyleVar StyleVar, 
								   const ValueType Value, 
								   OtherStylePairs&& ... OtherPairs)
		{
			if constexpr (sizeof...(OtherPairs) == 0)
			{
				ImGui::PushStyleVar(StyleVar, Value);
			}
			else
			{
				ImGui::PushStyleVar(StyleVar, Value);
				PushStyle(std::forward<OtherStylePairs>(OtherPairs)...);
			}
		}
	};

	class FScopedFont
	{
	public:
		FORCEINLINE FScopedFont(ImFont* Font) { ImGui::PushFont(Font); }
		~FScopedFont() { ImGui::PopFont(); }
		FScopedFont(const FScopedFont&) = delete;
		FScopedFont& operator=(const FScopedFont&) = delete;
	};

	struct FStyleMod
	{
	private:
		using T = std::underlying_type_t<EStyle>;
	public:
		T VarIdx;
		union 
		{ 
			int ArrInt[2]; 
			float ArrFloat[2]; 
		};

		FStyleMod(const EStyle Style, const int Value)     
			: VarIdx((T)Style)
		{ 
			ArrInt[0] = Value; 
		}

		FStyleMod(const EStyle Style, const float Value)
			: VarIdx((T)Style)
		{ 
			ArrFloat[0] = Value; 
		}

		FStyleMod(const EStyle Style, const LVector2& Value)
			: VarIdx((T)Style)
		{ 
			ArrFloat[0] = Value.X; 
			ArrFloat[1] = Value.Y; 
		}
	};

	/**
	 * LStyle
	 */
	struct LStyle
	{
		float AlignHorizontal = -1.0f; /* Disabled when -1.0 */
	};

	FORCEINLINE ImColor ColorWithMultipliedValue(const ImColor& Color, const float Multiplier)
	{
		const ImVec4& ColorRaw = Color.Value;
		float Hue, Saturation, Value;
		ImGui::ColorConvertRGBtoHSV(ColorRaw.x, ColorRaw.y, ColorRaw.z, Hue, Saturation, Value);
		return ImColor::HSV(Hue, Saturation, std::min(Value * Multiplier, 1.0f));
	}

	FORCEINLINE ImColor ColorWithMultipliedSaturation(const ImColor& Color, const float Multiplier)
	{
		const ImVec4& ColorRaw = Color.Value;
		float Hue, Saturation, Value;
		ImGui::ColorConvertRGBtoHSV(ColorRaw.x, ColorRaw.y, ColorRaw.z, Hue, Saturation, Value);
		return ImColor::HSV(Hue, std::min(Saturation * Multiplier, 1.0f), Value);
	}

	FORCEINLINE ImColor ColorWithMultipliedHue(const ImColor& Color, const float Multiplier)
	{
		const ImVec4& ColorRaw = Color.Value;
		float Hue, Saturation, Value;
		ImGui::ColorConvertRGBtoHSV(ColorRaw.x, ColorRaw.y, ColorRaw.z, Hue, Saturation, Value);
		return ImColor::HSV(std::min(Hue * Multiplier, 1.0f), Saturation, Value);
	}

}
