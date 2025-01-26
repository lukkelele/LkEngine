#pragma once

#include "LkEngine/Core/LObject/Enum.h"
#include "LkEngine/Core/Delegate/Delegate.h"
#include "LkEngine/Renderer/UI/UICore.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>


/** Draw bounding box around an item. */
#define LK_UI_DEBUG_BOUNDING_BOX(Color) \
			if (Debug::UI::bBoundingBoxesOnHover && ImGui::IsItemHovered()) \
			{                                                          \
				const ImVec2 RectMin = ImGui::GetItemRectMin();        \
				const ImVec2 RectMax = ImGui::GetItemRectMax();        \
				ImDrawList* DrawList = ImGui::GetWindowDrawList();     \
				DrawList->AddRect(RectMin, RectMax, Color);            \
			}


/************************************************************
 *                     CUSTOM IMGUI
 ************************************************************/
namespace ImGui {

	bool TreeNodeBehaviorIsOpen(const ImGuiID NodeID, ImGuiTreeNodeFlags Flags = ImGuiTreeNodeFlags_None);

	/**
	 * TreeNodeWithIcon
	 *
	 *  OpenDirection is the direction which the opened node points, down by default.
	 */
	bool TreeNodeWithIcon(::LkEngine::TObjectPtr<::LkEngine::LTexture2D> Icon,
						  const ImGuiID ID,
						  ImGuiTreeNodeFlags TreeNodeFlags,
						  const char* Label,
						  const char* LabelEnd,
						  const ImColor IconTint = IM_COL32_WHITE,
						  const ImGuiDir OpenDirection = ImGuiDir_Down);

}

namespace LkEngine {

	enum class EMessageBoxFlag : uint32_t
	{
		OkButton     = LK_BIT(0),
		CancelButton = LK_BIT(1),
		UserFunction = LK_BIT(2),
		AutoSize     = LK_BIT(3),
	};

	LK_ENUM_CLASS_FLAGS(EMessageBoxFlag);
}

namespace LkEngine::UI {

	LK_DECLARE_MULTICAST_DELEGATE(FOnMessageBoxCancelled, const char* /* UI Element Name */);

	extern FOnMessageBoxCancelled OnMessageBoxCancelled;

	/** Unlimited positive value. */
	static constexpr float SLIDER_MIN_UNLIMITED_POS = 0.0f;
	static constexpr float SLIDER_MAX_UNLIMITED = 0.0f;

	/** 
	 * FMessageBox
	 */
	struct FMessageBox
	{
		std::string Title = "";
		std::string Body = "";
		uint32_t Flags = 0;
		uint32_t Width = 0;
		uint32_t Height = 0;
		uint32_t MinWidth = 0;
		uint32_t MinHeight = 0;
		uint32_t MaxWidth = -1;
		uint32_t MaxHeight = -1;
		std::function<void()> UserRenderFunction;
		bool bShouldOpen = true;
		bool bIsOpen = false;
	};
	
	namespace Internal 
	{
		static std::unordered_map<std::string, FMessageBox> MessageBoxes;
	}

	void ShowMessageBox(const char* Title,
						const std::function<void()>& RenderFunction,
						uint32_t Flags = (uint32_t)EMessageBoxFlag::AutoSize,
						const uint32_t Width = 600,
						const uint32_t Height = 0,
						const uint32_t MinWidth = 140,
						const uint32_t MinHeight = 60,
						const uint32_t MaxWidth = -1,
						const uint32_t MaxHeight = -1);

	void RenderMessageBoxes();

	static bool PropertyGridHeader(const std::string& Name, bool OpenByDefault = true)
	{
		ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Framed
			| ImGuiTreeNodeFlags_SpanAvailWidth
			| ImGuiTreeNodeFlags_AllowItemOverlap
			| ImGuiTreeNodeFlags_FramePadding;

		if (OpenByDefault)
		{
			TreeNodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
		}

		static constexpr float FramePaddingX = 6.0f;
		static constexpr float FramePaddingY = 6.0f; // affects height of the header

		UI::FScopedStyle HeaderRounding(ImGuiStyleVar_FrameRounding, 0.0f);
		UI::FScopedStyle HeaderPaddingAndHeight(ImGuiStyleVar_FramePadding, ImVec2(FramePaddingX, FramePaddingY));

		ImGui::PushID(Name.c_str());
		//IsOpen = ImGui::TreeNodeEx("##DummyID", TreeNodeFlags, StringUtils::ToUpper(Name).c_str());
		const bool IsOpen = ImGui::TreeNodeEx("##DummyID", TreeNodeFlags, Name.c_str());
		ImGui::PopID();

		return IsOpen;
	}

	FORCEINLINE void EndPropertyGridHeader()
	{
		ImGui::TreePop();
	}

	FORCEINLINE bool BeginTreeNode(const char* NodeName, const bool DefaultOpen)
	{
		ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
		if (DefaultOpen)
		{
			TreeNodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
		}

		return ImGui::TreeNodeEx(NodeName, TreeNodeFlags);
	}

	FORCEINLINE void EndTreeNode()
	{
		ImGui::TreePop();
	}

	FORCEINLINE void PopupMenuHeader(const std::string& Text, 
									 const bool IndentAfter = true, 
									 const bool UnindentBefore = false)
	{
		if (UnindentBefore)
		{
			ImGui::Unindent();
		}

		static constexpr ImColor TextColor = ImColor(170, 170, 170);
		ImGui::TextColored(TextColor.Value, Text.c_str());
		ImGui::Separator();
		if (IndentAfter)
		{
			ImGui::Indent();
		}
	};

	FORCEINLINE bool IsItemDisabled()
	{
		return ImGui::GetItemFlags() & ImGuiItemFlags_Disabled;
	}

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

	inline ImColor ColorWithMultipliedHue(const ImColor& Color, const float Multiplier)
	{
		const ImVec4& ColorRaw = Color.Value;
		float Hue, Saturation, Value;
		ImGui::ColorConvertRGBtoHSV(ColorRaw.x, ColorRaw.y, ColorRaw.z, Hue, Saturation, Value);
		return ImColor::HSV(std::min(Hue * Multiplier, 1.0f), Saturation, Value);
	}

	FORCEINLINE void VSeparator(const float Height = 10.0f)
	{
		ImGui::Dummy(ImVec2(0, Height));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0, Height));
	}

	FORCEINLINE void DrawButtonImage(const TObjectPtr<LTexture2D>& ImageNormal, 
									 const TObjectPtr<LTexture2D>& ImageHovered, 
									 const TObjectPtr<LTexture2D>& ImagePressed, 
									 ImU32 TintNormal, 
									 ImU32 TintHovered, 
									 ImU32 TintPressed, 
									 ImVec2 RectMin, 
									 ImVec2 RectMax)
	{
		ImDrawList* DrawList = ImGui::GetWindowDrawList();
		if (ImGui::IsItemActive())
		{
			DrawList->AddImage(GetTextureID(ImagePressed), RectMin, RectMax, ImVec2(0, 0), ImVec2(1, 1), TintPressed);
		}
		else if (ImGui::IsItemHovered())
		{
			DrawList->AddImage(GetTextureID(ImageHovered), RectMin, RectMax, ImVec2(0, 0), ImVec2(1, 1), TintHovered);
		}
		else
		{
			DrawList->AddImage(GetTextureID(ImageNormal), RectMin, RectMax, ImVec2(0, 0), ImVec2(1, 1), TintNormal);
		}
	};

	FORCEINLINE void DrawButtonImage(const TObjectPtr<LTexture2D>& Image, ImU32 TintNormal, ImU32 TintHovered, ImU32 TintPressed, ImRect Rectangle)
	{
		DrawButtonImage(Image, Image, Image, TintNormal, TintHovered, TintPressed, Rectangle.Min, Rectangle.Max);
	};

	FORCEINLINE ImRect GetItemRect()
	{
		return ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
	}

	FORCEINLINE ImRect RectExpanded(const ImRect& Rect, const float x, const float y)
	{
		ImRect Result = Rect;
		Result.Min.x -= x;
		Result.Min.y -= y;
		Result.Max.x += x;
		Result.Max.y += y;
		return Result;
	}

	FORCEINLINE ImRect RectOffset(const ImRect& Rect, const float x, const float y)
	{
		ImRect Result = Rect;
		Result.Min.x += x;
		Result.Min.y += y;
		Result.Max.x += x;
		Result.Max.y += y;
		return Result;
	}

	FORCEINLINE void ShiftCursorX(const float Distance)
	{
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + Distance);
	}

	FORCEINLINE void ShiftCursorY(const float Distance)
	{
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + Distance);
	}

	FORCEINLINE void ShiftCursor(const float InX, const float InY)
	{
		const ImVec2 Cursor = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(Cursor.x + InX, Cursor.y + InY));
	}

	FORCEINLINE void HelpMarker(const char* HelpDesc, const char* HelpSymbol = "(?)")
	{
		static constexpr float WrapPosOffset = 35.0f;
		ImGui::TextDisabled(HelpSymbol);
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * WrapPosOffset);
			ImGui::TextUnformatted(HelpDesc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	FORCEINLINE void DrawItemActivityOutline()
	{
		/* TODO */
	}

	FORCEINLINE void Spring(const float Weight = 1.0f)
	{
		const ImVec2 AvailableSpace = ImGui::GetContentRegionAvail();
		const float SpringSize = AvailableSpace.x * Weight;
		ImGui::Dummy(ImVec2(SpringSize, 0.0f));
	}

	FORCEINLINE void AlignHorizontalCenter(const float ItemWidth)
	{
		const ImVec2 AvailableSpace = ImGui::GetContentRegionAvail();
		const float Offset = (AvailableSpace.x - ItemWidth) / 2.0f;
		if (Offset > 0.0f)
		{
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + Offset);
		}
	}

	namespace Draw 
	{
		FORCEINLINE void Underline(bool FullWidth = false, const float OffsetX = 0.0f, const float OffsetY = -1.0f)
		{
			if (FullWidth)
			{
				if (ImGui::GetCurrentWindow()->DC.CurrentColumns != nullptr)
				{
					ImGui::PushColumnsBackground();
				}
				else if (ImGui::GetCurrentTable() != nullptr)
				{
					ImGui::TablePushBackgroundChannel();
				}
			}

			const float Width = FullWidth ? ImGui::GetWindowWidth() : ImGui::GetContentRegionAvail().x;
			const ImVec2 Cursor = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddLine(
				ImVec2(Cursor.x + OffsetX, Cursor.y + OffsetY),
				ImVec2(Cursor.x + Width, Cursor.y + OffsetY),
				RGBA32::BackgroundDark,
				1.0f
			);

			if (FullWidth)
			{
				if (ImGui::GetCurrentWindow()->DC.CurrentColumns != nullptr)
				{
					ImGui::PopColumnsBackground();
				}
				else if (ImGui::GetCurrentTable() != nullptr)
				{
					ImGui::TablePopBackgroundChannel();
				}
			}
		}

		FORCEINLINE bool DragFloat(const char* Label, float* Value, float ValueSpeed = 1.0f, float ValueMin = 0.0f, float ValueMax = 0.0f, const char* Format = "%.3f", ImGuiSliderFlags Flags = 0)
		{
			return ImGui::DragScalar(Label, ImGuiDataType_Float, Value, ValueSpeed, &ValueMin, &ValueMax, Format, Flags);
		}

		FORCEINLINE bool DragFloat3(const char* Label, float Vec[3], float VecSpeed = 1.0f, float VecMin = 0.0f, float VecMax = 0.0f, const char* Format = "%.3f", ImGuiSliderFlags Flags = 0)
		{
			const bool Changed = ImGui::DragFloat3(Label, Vec, VecSpeed, VecMin, VecMax, Format, Flags);
			DrawItemActivityOutline();
			return Changed;
		}

		FORCEINLINE bool SliderFloat3(const char* Label, float Vec[3], float VecMin, float VecMax, const char* Format = "%.3f", ImGuiSliderFlags Flags = 0) 
		{
			const bool Changed = ImGui::SliderFloat3(Label, Vec, VecMin, VecMax, Format, Flags);
			DrawItemActivityOutline();
			return Changed;
		}

		/**
		 * Vec3Control
		 * 
		 *  To be used in already existing tables.
		 */
		FORCEINLINE bool Vec3Control(const std::string& Label, 
									 glm::vec3& Values, 
									 bool& ManuallyEdited, 
									 const float ResetValue = 0.0f, 
									 const float ValueSpeed = 0.10f,
									 const float ValueMin = 0.0f,
									 const float ValueMax = 0.0f,
									 const float ColumnWidth = 100.0f, 
									 uint32_t RenderMultiSelectAxes = 0,
									 const char* Format = "%.2f")
		{
			bool Modified = false;

			//UI::PushID();
			ImGui::TableSetColumnIndex(0);
			UI::ShiftCursor(17.0f, 7.0f);

			ImGui::Text(Label.c_str());
			UI::Draw::Underline(false, 0.0f, 2.0f);

			ImGui::TableSetColumnIndex(1);
			UI::ShiftCursor(7.0f, 0.0f);
			{
				static constexpr float SpacingX = 8.0f;
				UI::FScopedStyle ItemSpacing(ImGuiStyleVar_ItemSpacing, ImVec2(SpacingX, 0.0f));
				UI::FScopedStyle Padding(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 2.0f));
				{
					UI::FScopedColor Padding(ImGuiCol_Border, IM_COL32(0, 0, 0, 0));
					UI::FScopedColor Frame(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 0));

					ImGui::BeginChild(
						ImGui::GetID((Label + "Subwindow").c_str()),
						ImVec2((ImGui::GetContentRegionAvail().x - SpacingX), ImGui::GetFrameHeightWithSpacing() + 8.0f),
						0, /* Child Flags. */
						ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse /* Window Flags. */
					);
				}

				//static constexpr float FramePadding = 2.0f;
				static constexpr float FramePadding = 3.0f;
				static constexpr float OutlineSpacing = 1.0f;
				const float LineHeight = GImGui->Font->FontSize + FramePadding * 2.0f;
				const ImVec2 ButtonSize = { LineHeight + 2.0f, LineHeight };
				const float InputItemWidth = (ImGui::GetContentRegionAvail().x - SpacingX) / 3.0f - ButtonSize.x;

				UI::ShiftCursor(0.0f, FramePadding);

				auto DrawControl = [&](const std::string& InLabel, 
									   float& InValue, 
									   const ImVec4& InColorNormal,
									   const ImVec4& InColorHover, 
									   const ImVec4& InColorPressed, 
									   bool RenderMultiSelect)
				{
					{
						UI::FScopedStyle ButtonFrame(ImGuiStyleVar_FramePadding, ImVec2(FramePadding, 0.0f));
						UI::FScopedStyle ButtonRounding(ImGuiStyleVar_FrameRounding, 1.0f);
						UI::FScopedColorStack ButtonColours(
							ImGuiCol_Button, InColorNormal, 
							ImGuiCol_ButtonHovered, InColorHover,
							ImGuiCol_ButtonActive, InColorPressed
						);

						//UI::ShiftCursorY(FramePadding);
						if (ImGui::Button(InLabel.c_str(), ButtonSize))
						{
							InValue = ResetValue;
							Modified = true;
							LK_CORE_DEBUG("Pressed Button: {}", InLabel.c_str());
						}
					}

					ImGui::SameLine(0.0f, OutlineSpacing);
					ImGui::SetNextItemWidth(InputItemWidth);
					//UI::ShiftCursorY(-FramePadding);

					ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, RenderMultiSelect);
					const ImGuiID InputID = ImGui::GetID(("##" + InLabel).c_str());
					const bool WasTempInputActive = ImGui::TempInputIsActive(InputID);
					//Modified |= ImGui::DragFloat(("##" + InLabel).c_str(), &InValue, 0.1f, 0.0f, 0.0f, "%.2f", 0);
					Modified |= ImGui::DragFloat(("##" + InLabel).c_str(), &InValue, ValueSpeed, ValueMin, ValueMax, Format, 0);

					if (ImGui::TempInputIsActive(InputID))
					{
						Modified = false;
					}

					ImGui::PopItemFlag();

					if (WasTempInputActive)
					{
						ManuallyEdited |= ImGui::IsItemDeactivatedAfterEdit();
					}
				};

				/* Draw X. */
				DrawControl(
					"X", 
					Values.x, 
					ImVec4(0.80f, 0.10f, 0.15f, 1.0f), /* Normal  */
					ImVec4(0.90f, 0.20f, 0.20f, 1.0f), /* Hover   */
					ImVec4(0.80f, 0.10f, 0.15f, 1.0f), /* Pressed */
					(RenderMultiSelectAxes & EVectorAxis::X)
				); 

				/* Draw Y. */
				ImGui::SameLine(0.0f, OutlineSpacing);
				DrawControl(
					"Y", 
					Values.y, 
					ImVec4(0.20f, 0.70f, 0.20f, 1.0f), 
					ImVec4(0.30f, 0.80f, 0.30f, 1.0f), 
					ImVec4(0.20f, 0.70f, 0.20f, 1.0f), 
					(RenderMultiSelectAxes & EVectorAxis::Y)
				);

				/* Draw Z. */
				ImGui::SameLine(0.0f, OutlineSpacing);
				DrawControl(
					"Z", 
					Values.z, 
					ImVec4(0.10f, 0.25f, 0.80f, 1.0f), 
					ImVec4(0.20f, 0.35f, 0.90f, 1.0f), 
					ImVec4(0.10f, 0.25f, 0.80f, 1.0f), 
					(RenderMultiSelectAxes & EVectorAxis::Z)
				);
				//ImGui::Text("Modified: %s   Manually Edited: %s", (Modified ? "Yes" : "No"), (ManuallyEdited ? "Yes" : "No"));

				ImGui::EndChild();
			}
			//UI::PopID();

			return Modified || ManuallyEdited;
		}
	}

	FORCEINLINE void BeginPropertyGrid(const uint32_t Columns = 2)
	{
		/* TODO: Evaluate the need of pushing/popping ID's for property grids. */
		//PushID();
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));
		ImGui::Columns(Columns);
	}

	FORCEINLINE void EndPropertyGrid()
	{
		/* TODO: Evaluate the need of pushing/popping ID's for property grids. */
		ImGui::Columns(1);
		Draw::Underline();
		ImGui::PopStyleVar(2); /* ItemSpacing, FramePadding */
		ShiftCursorY(18.0f);
		//PopID();
	}

	FORCEINLINE bool Property(const char* Label, bool& Value, const char* HelpText = "")
	{
		bool Modified = false;

		ShiftCursor(10.0f, 9.0f);
		ImGui::Text(Label);

		if (std::strlen(HelpText) > 0)
		{
			ImGui::SameLine();
			HelpMarker(HelpText);
		}

		ImGui::NextColumn();
		ShiftCursorY(4.0f);
		ImGui::PushItemWidth(-1);

		Modified = ImGui::Checkbox(std::format("##{0}", Label).c_str(), &Value);

		ImGui::PopItemWidth();
		ImGui::NextColumn();
		Draw::Underline();

		return Modified;
	}

	static bool Property(const char* Label, float& Value, float Delta = 0.10f, float Min = 0.0f, float Max = 0.0f, const char* HelpText = "", const char* HelpSymbol = "(?)")
	{
		ShiftCursor(10.0f, 9.0f);
		ImGui::Text(Label);

		if (std::strlen(HelpText) != 0)
		{
			ImGui::SameLine();
			HelpMarker(HelpText, HelpSymbol);
		}

		ImGui::NextColumn();
		ShiftCursorY(4.0f);
		ImGui::PushItemWidth(-1);

		const bool Modified = UI::Draw::DragFloat(std::format("##{0}", Label).c_str(), &Value, Delta, Min, Max);

		ImGui::PopItemWidth();
		ImGui::NextColumn();
		Draw::Underline();

		return Modified;
	}

	FORCEINLINE bool Property(const char* Label, glm::vec3& Value, float Delta = 0.10f, float Min = 0.0f, float Max = 0.0f, const char* HelpText = "")
	{
		ShiftCursor(10.0f, 9.0f);
		ImGui::Text(Label);

		if (std::strlen(HelpText) != 0)
		{
			ImGui::SameLine();
			HelpMarker(HelpText);
		}

		ImGui::NextColumn();
		ShiftCursorY(4.0f);

		ImGui::PushItemWidth(-1);
		const bool Modified = UI::Draw::DragFloat3(std::format("##{0}", Label).c_str(), glm::value_ptr(Value), Delta, Min, Max);
		ImGui::PopItemWidth();

		ImGui::NextColumn();
		Draw::Underline();

		return Modified;
	}

	FORCEINLINE bool IsItemHovered(const float DelayInSeconds = 0.10f, 
								   ImGuiHoveredFlags Flags = ImGuiHoveredFlags_None)
	{
		return ImGui::IsItemHovered() && (GImGui->HoveredIdTimer > DelayInSeconds); /* HoveredIdNotActiveTimer. */
	}

	FORCEINLINE void SetTooltip(std::string_view Text, 
								const float DelayInSeconds = 0.10f, 
								const bool AllowWhenDisabled = true, 
								const ImVec2 Padding = ImVec2(5, 5))
	{
		if (IsItemHovered(DelayInSeconds, AllowWhenDisabled ? ImGuiHoveredFlags_AllowWhenDisabled : ImGuiHoveredFlags_None))
		{
			UI::FScopedStyle TooltipPadding(ImGuiStyleVar_WindowPadding, Padding);
			UI::FScopedColor TextCol(ImGuiCol_Text, RGBA32::Text::Brighter);
			ImGui::SetTooltip(Text.data());
		}
	}

	FORCEINLINE bool TreeNode(const std::string& ID, 
							  const std::string& Label, 
							  const ImGuiTreeNodeFlags Flags = 0, 
							  const TObjectPtr<LTexture2D>& Icon = nullptr,
							  const ImGuiDir OpenDirection = ImGuiDir_Down)
	{
		ImGuiWindow* Window = ImGui::GetCurrentWindow();
		if (!Window || Window->SkipItems)
		{
			return false;
		}

		return ImGui::TreeNodeWithIcon(
			Icon, 
			Window->GetID(ID.c_str()), 
			Flags, 
			Label.c_str(),  /* Label Start. */
			nullptr,        /* Label End.   */
			IM_COL32_WHITE, /* Icon Tint.   */
			OpenDirection   /* Arrow direction on opened node. */
		);
	}

}

