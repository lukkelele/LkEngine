#pragma once

#include "LkEngine/Core/LObject/Enum.h"
#include "LkEngine/Core/Delegate/Delegate.h"
#include "LkEngine/Renderer/UI/UICore.h"

#include <deque>

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
	LK_ENUM_RANGE_FLAGS_BY_FIRST_AND_LAST(EMessageBoxFlag, EMessageBoxFlag::OkButton, EMessageBoxFlag::AutoSize);

}

namespace LkEngine::UI {

	LK_DECLARE_MULTICAST_DELEGATE(FOnMessageBoxCancelled, const char* /* UI Element Name */);

	extern FOnMessageBoxCancelled OnMessageBoxCancelled;

	FORCEINLINE void PushAligned(const float Spacing = 0.0f)
	{
		UI::UIContext.AlignedStack.emplace_front(Spacing, UIContext.AlignedStack.size());
	}

	FORCEINLINE void PopAligned()
	{
		UI::UIContext.AlignedStack.pop_front();
	}

	void Separator(const ImVec2& Size, const ImVec4& Color);

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
									 const float ValueSpeed = 0.10f,
									 const float ResetValue = 0.0f, 
									 const float ValueMin = 0.0f,
									 const float ValueMax = 0.0f,
									 const float ColumnWidth = 100.0f, 
									 const char* Format = "%.2f",
									 uint32_t RenderMultiSelectAxes = 0)
		{
			bool Modified = false;
			bool ManuallyEdited = false;

		#if 0
			ImGui::TableSetColumnIndex(0);
			UI::ShiftCursor(17.0f, 7.0f);

			ImGui::Text(Label.c_str());
			UI::Draw::Underline(false, 0.0f, 2.0f);

			ImGui::TableSetColumnIndex(1);
			UI::ShiftCursor(7.0f, 0.0f);
		#else
			if (InTable())
			{
				ImGui::TableSetColumnIndex(0);
				UI::ShiftCursor(17.0f, 7.0f);

				ImGui::Text(Label.c_str());
				UI::Draw::Underline(false, 0.0f, 2.0f);

				ImGui::TableSetColumnIndex(1);
				UI::ShiftCursor(7.0f, 0.0f);
			}
			else
			{
				if (!Label.empty() && Label.at(0) != '#')
				{
					ImGui::Text(Label.c_str());
					/* TODO: Do PushStyle here instead. */
					ImGui::SameLine();
				}
			}
		#endif

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
						ImGuiChildFlags_None,
						ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse /* Window Flags. */
					);
				}

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

						if (ImGui::Button(InLabel.c_str(), ButtonSize))
						{
							InValue = ResetValue;
							Modified = true;
							LK_CORE_DEBUG("Pressed Button: {}", InLabel.c_str());
						}
					}

					ImGui::SameLine(0.0f, OutlineSpacing);
					ImGui::SetNextItemWidth(InputItemWidth);

					ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, RenderMultiSelect);
					const ImGuiID InputID = ImGui::GetID(("##" + InLabel).c_str());
					const bool WasTempInputActive = ImGui::TempInputIsActive(InputID);
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

				ImGui::EndChild();
			}

			/* TODO: Check if we are in a table before calling this. */
			if (InTable())
			{
				ImGui::TableNextRow();
			}

			return Modified || ManuallyEdited;
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
						ImGuiChildFlags_None,
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

				ImGui::EndChild();
			}

			/* TODO: Check if we are in a table before calling this. */
			if (InTable())
			{
				ImGui::TableNextRow();
			}

			return Modified || ManuallyEdited;
		}
	}

	FORCEINLINE void BeginPropertyGrid(const char* Label = nullptr,
									   const ImVec2& Size = ImVec2(0.0f, 0.0f), 
									   const ImGuiTableFlags Flags = ImGuiTableFlags_SizingStretchProp,
									   const bool UseHeaderLabels = false)
	{
		UI::PushID();
		UIContext.TableStack.push_back({ Label });

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));
		if (!ImGui::BeginTable(Label ? Label : GenerateID(), 2, Flags, Size))
		{
			return;
		}

		static constexpr ImGuiTableColumnFlags ColumnFlags = ImGuiTableColumnFlags_None;
		for (uint32_t ColumnIdx = 0; ColumnIdx < 2; ColumnIdx++)
		{
			if (UseHeaderLabels)
			{
				ImGui::TableSetupColumn(std::format("Column-{}", ColumnIdx).c_str(), ColumnFlags);
			}
			else
			{
				ImGui::TableSetupColumn(std::format("##Column-{}", ColumnIdx).c_str(), ColumnFlags);
			}
		}

		if (UseHeaderLabels)
		{
			ImGui::TableHeadersRow();
		}
		ImGui::TableNextRow();
	}

	FORCEINLINE void EndPropertyGrid()
	{
		ImGui::EndTable();
		ImGui::PopStyleVar(2); /* ItemSpacing, FramePadding */

		UI::PopID();
		UIContext.TableStack.pop_back();
	}

	FORCEINLINE bool Property(const char* Label, bool& Value, const char* HelpText = "")
	{
		bool Modified = false;

		const bool IsCurrentlyInTable = InTable();
		if (IsCurrentlyInTable)
		{
			ImGui::TableSetColumnIndex(0);
		}

		ShiftCursor(10.0f, 9.0f);
		ImGui::Text(Label);

		if (std::strlen(HelpText) > 0)
		{
			ImGui::SameLine();
			HelpMarker(HelpText);
		}

		if (IsCurrentlyInTable)
		{
			ImGui::TableSetColumnIndex(1);
		}

		ShiftCursorY(4.0f);

		ImGui::PushItemWidth(-1);
		Modified = ImGui::Checkbox(std::format("##{0}", Label).c_str(), &Value);
		ImGui::PopItemWidth();

		if (IsCurrentlyInTable)
		{
			ImGui::TableNextRow();
		}

		return Modified;
	}

	static bool Property(const char* Label, float& Value, float Delta = 0.10f, float Min = 0.0f, float Max = 0.0f, const char* HelpText = "", const char* HelpSymbol = "(?)")
	{
		const bool IsCurrentlyInTable = InTable();
		if (IsCurrentlyInTable)
		{
			ImGui::TableSetColumnIndex(0);
		}

		ShiftCursor(10.0f, 9.0f);
		ImGui::Text(Label);

		if (std::strlen(HelpText) != 0)
		{
			ImGui::SameLine();
			HelpMarker(HelpText, HelpSymbol);
		}

		if (IsCurrentlyInTable)
		{
			ImGui::TableSetColumnIndex(1);
		}
		ShiftCursorY(4.0f);

		ImGui::PushItemWidth(-1);
		const bool Modified = UI::Draw::DragFloat(std::format("##{}", Label).c_str(), &Value, Delta, Min, Max);
		ImGui::PopItemWidth();

		if (IsCurrentlyInTable)
		{
			ImGui::TableNextRow();
		}

		return Modified;
	}

	/**
	 * Slider property (float 3).
	 */
	FORCEINLINE bool Property(const char* Label, 
							  glm::vec3& Value, 
							  float Delta = 0.10f, 
							  float Min = 0.0f, 
							  float Max = 0.0f, 
							  const char* HelpText = "")
	{
		ImGui::TableSetColumnIndex(0);

		ShiftCursor(10.0f, 9.0f);
		ImGui::Text(Label);

		if (std::strlen(HelpText) != 0)
		{
			ImGui::SameLine();
			HelpMarker(HelpText);
		}

		ImGui::TableSetColumnIndex(1);
		ShiftCursorY(4.0f);

		ImGui::PushItemWidth(-1);
		const bool Modified = UI::Draw::DragFloat3(std::format("##{0}", Label).c_str(), glm::value_ptr(Value), Delta, Min, Max);
		ImGui::PopItemWidth();

		ImGui::TableNextRow();

		return Modified;
	}

	FORCEINLINE bool IsItemHovered(const float DelayInSeconds = 0.10f, ImGuiHoveredFlags Flags = ImGuiHoveredFlags_None)
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

	template<typename T>
	static void Table(const char* TableName, 
					  const char** Columns, 
					  const uint32_t ColumnCount, 
					  const ImVec2& Size, 
					  T RenderFunc)
	{
		if ((Size.x <= 0.0f) || (Size.y <= 0.0f))
		{
			return;
		}

		static constexpr float EdgeOffset = 4.0f;

		FScopedStyle CellPadding(ImGuiStyleVar_CellPadding, ImVec2(4.0f, 0.0f));
		const ImColor BgColor = ImColor(RGBA32::Background);
		const ImColor ColRowAlt = ColorWithMultipliedValue(BgColor, 1.29f);

		FScopedColor RowColor(ImGuiCol_TableRowBg, BgColor);
		FScopedColor RowAltColor(ImGuiCol_TableRowBgAlt, ColRowAlt);
		FScopedColor TableColor(ImGuiCol_ChildBg, BgColor);

		static constexpr ImGuiTableFlags TableFlags = ImGuiTableFlags_NoPadInnerX
			| ImGuiTableFlags_Resizable
			| ImGuiTableFlags_Reorderable
			| ImGuiTableFlags_ScrollY
			| ImGuiTableFlags_RowBg;

		if (!ImGui::BeginTable(TableName, ColumnCount, TableFlags, Size))
		{
			return;
		}

		/* Cache the cursor position on the X-axis. */
		const float CursorX = ImGui::GetCursorScreenPos().x;

		for (uint32_t Idx = 0; Idx < ColumnCount; Idx++)
		{
			ImGui::TableSetupColumn(Columns[Idx]);
		}

		/* Setup table headers. */
		{
			const ImColor ActiveColor = ColorWithMultipliedValue(BgColor, 1.35f);
			FScopedColorStack HeaderCol(ImGuiCol_HeaderHovered, ActiveColor, 
										ImGuiCol_HeaderActive, ActiveColor);

			ImGui::TableSetupScrollFreeze(ImGui::TableGetColumnCount(), 1);
			ImGui::TableNextRow(ImGuiTableRowFlags_Headers, 24.0f);

			for (uint32_t Idx = 0; Idx < ColumnCount; Idx++)
			{
				ImGui::TableSetColumnIndex(Idx);
				const char* ColumnName = ImGui::TableGetColumnName(Idx);
				ImGui::PushID(ColumnName);

				/**
				 * Not sure about the edge offset on the Y-axis.
				 * A factor of 1.0f or 2.0f works.
				 */
				//UI::ShiftCursor(EdgeOffset * 3.0f, EdgeOffset * 2.0f);
				UI::ShiftCursor(EdgeOffset * 3.0f, EdgeOffset * 1.0f);
				ImGui::TableHeader(ColumnName);
				UI::ShiftCursor(-EdgeOffset * 3.0f, -EdgeOffset * 1.0f);
				//UI::ShiftCursor(-EdgeOffset * 3.0f, -EdgeOffset * 2.0f);

				ImGui::PopID();
			}

			ImGui::SetCursorScreenPos(ImVec2(CursorX, ImGui::GetCursorScreenPos().y));
		}

		RenderFunc();

		ImGui::EndTable();
	}

	bool TableRowClickable(const char* RowID, const float RowHeight);

	FORCEINLINE bool ColoredButton(const char* Label, const ImVec4& BgColor, const ImVec2& ButtonSize)
	{
		FScopedColor ButtonCol(ImGuiCol_Button, BgColor);
		return ImGui::Button(Label, ButtonSize);
	}

	FORCEINLINE bool ColoredButton(const char* Label, const ImVec4& BgColor, const ImVec4& FgColor, const ImVec2& ButtonSize)
	{
		FScopedColor TextColor(ImGuiCol_Text, FgColor);
		FScopedColor ButtonColor(ImGuiCol_Button, BgColor);
		return ImGui::Button(Label, ButtonSize);
	}

	FORCEINLINE bool BeginCombo(const char* Label, const char* PreviewValue, const ImGuiComboFlags Flags = ImGuiComboFlags_None)
	{
		const bool Opened = ImGui::BeginCombo(Label, PreviewValue, Flags);
		DrawItemActivityOutline();

		return Opened;
	}

	FORCEINLINE void EndCombo()
	{
		ImGui::EndCombo();
	}

	template<typename TEnum>
	FORCEINLINE bool PropertyDropdown(const char* Label, 
									  const char** Options, 
									  const uint16_t ArrSize, 
									  TEnum& Selected, 
									  const char* HelpText = "")
	{
		using T = std::underlying_type_t<TEnum>;
		const T SelectedIndex = static_cast<std::underlying_type_t<TEnum>>(Selected);

		const char* CurrentOption = Options[SelectedIndex];
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

		bool Modified = false;

		const std::string ID = std::format("##{}", Label);
		if (UI::BeginCombo(ID.c_str(), CurrentOption))
		{
			for (uint16_t Idx = 0; Idx < ArrSize; Idx++)
			{
				const bool IsSelected = (CurrentOption == Options[Idx]);
				if (ImGui::Selectable(Options[Idx], IsSelected))
				{
					CurrentOption = Options[Idx];
					Selected = static_cast<TEnum>(Idx);
					Modified = true;
				}

				if (IsSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			UI::EndCombo();
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();
		Draw::Underline();

		return Modified;
	}

	template<std::size_t ArrSize, typename T, typename TEnum>
	FORCEINLINE bool PropertyDropdown(const char* Label, 
									  const std::array<T, ArrSize>& Options,
									  TEnum& Selected, 
									  const char* HelpText = "",
									  const int ComboWidth = -1)
	{
		using ValueType = std::remove_cvref_t<T>;
		static_assert((std::is_same_v<ValueType, const char*> || Core::IsPair<ValueType>), "Unsupported type");

		using PrimitiveType = std::underlying_type_t<TEnum>;
		const PrimitiveType SelectedIndex = static_cast<PrimitiveType>(Selected);

		const char* CurrentOption = nullptr;
		if constexpr (std::is_same_v<ValueType, const char*>)
		{
			CurrentOption = Options.at(SelectedIndex);
		}
		else if constexpr (Core::IsPair<ValueType>)
		{
			static_assert(Core::IsPairWithFirstArgConstChar<ValueType>, "The first pair argument (the label) is not const char*, which is required");
			/* Select the label from the std::pair (first argument). */
			CurrentOption = Options.at(SelectedIndex).first;
		}
		else
		{
			LK_CORE_VERIFY(false);
		}

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

		bool Modified = false;

		const std::string ID = std::format("##{}", Label);
		if (UI::BeginCombo(ID.c_str(), CurrentOption))
		{
			for (uint16_t Idx = 0; Idx < ArrSize; Idx++)
			{
				const char* Option = nullptr;
				if constexpr (std::is_same_v<ValueType, const char*>)
				{
					Option = Options[Idx];
				}
				else if constexpr (Core::IsPair<ValueType>)
				{
					Option = Options.at(Idx).first;
				}
				else
				{
					LK_CORE_VERIFY(false);
				}

				const bool IsSelected = (Option == CurrentOption);
				if (ImGui::Selectable(Option, IsSelected))
				{
					CurrentOption = Options[Idx];
					Selected = static_cast<TEnum>(Idx);
					Modified = true;
				}

				if (IsSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			UI::EndCombo();
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();
		Draw::Underline();

		return Modified;
	}


	FORCEINLINE bool PropertyDropdown(const char* Label, 
									  const char* const* Options, 
									  const uint16_t ArrSize, 
									  int32_t* Selected, 
									  const char* HelpText = "",
									  const int ComboWidth = -1)
	{
		const bool IsCurrentlyInTable = !UIContext.TableStack.empty();
		const bool ShouldAlign = !UIContext.AlignedStack.empty();

		if (IsCurrentlyInTable)
		{
			ImGui::TableSetColumnIndex(0);
		}

		if (ShouldAlign)
		{
			Internal::FAlignData& AlignData = UIContext.AlignedStack.front();
			if (AlignData.PassedElements > 0)
			{
				ImGui::SameLine(0.0f, AlignData.Dist);
			}
			AlignData.PassedElements++;
		}
		else
		{
			ShiftCursor(10.0f, 9.0f);
		}

		const char* CurrentOption = Options[*Selected];

		/* Skip rendering text for '#' and '##' identifier labels. */
		if (Label && Label[0] != '#')
		{
			ImGui::Text(Label);
			if (std::strlen(HelpText) != 0)
			{
				ImGui::SameLine();
				HelpMarker(HelpText);
			}
		}

		if (IsCurrentlyInTable)
		{
			ImGui::TableSetColumnIndex(1);
		}
		else
		{
			ImGui::NextColumn();
		}

		if (ShouldAlign)
		{
			Internal::FAlignData& AlignData = UIContext.AlignedStack.front();
			ImGui::SameLine(0.0f, AlignData.Dist);
			AlignData.PassedElements++;
		}
		else
		{
			ShiftCursorY(4.0f);
		}
		ImGui::PushItemWidth(ComboWidth);

		bool Modified = false;

		const std::string ID = std::format("##{}", Label);
		if (UI::BeginCombo(ID.c_str(), CurrentOption))
		{
			for (uint16_t Idx = 0; Idx < ArrSize; Idx++)
			{
				const bool IsSelected = (CurrentOption == Options[Idx]);
				if (ImGui::Selectable(Options[Idx], IsSelected))
				{
					CurrentOption = Options[Idx];
					*Selected = Idx;
					Modified = true;
				}

				if (IsSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			UI::EndCombo();
		}

		ImGui::PopItemWidth();

		if (IsCurrentlyInTable)
		{
			ImGui::TableNextRow();
		}
		else
		{
			ImGui::NextColumn();
			Draw::Underline();
		}

		return Modified;
	}

#if 0
	template<std::size_t ArrSize>
	FORCEINLINE bool PropertyDropdown(const char* Label, 
									  const char* const* Options, 
									  int32_t* Selected, 
									  const char* HelpText = "",
									  const int ComboWidth = -1)
	{
		const bool IsCurrentlyInTable = !UIContext.TableStack.empty();
		const bool ShouldAlign = !UIContext.AlignedStack.empty();

		if (IsCurrentlyInTable)
		{
			ImGui::TableSetColumnIndex(0);
		}

		if (ShouldAlign)
		{
			Internal::FAlignData& AlignData = UIContext.AlignedStack.front();
			if (AlignData.PassedElements > 0)
			{
				ImGui::SameLine(0.0f, AlignData.Dist);
			}
			AlignData.PassedElements++;
		}
		else
		{
			ShiftCursor(10.0f, 9.0f);
		}

		const char* CurrentOption = Options[*Selected];

		/* Skip rendering text for '#' and '##' identifier labels. */
		if (Label && Label[0] != '#')
		{
			ImGui::Text(Label);
			if (std::strlen(HelpText) != 0)
			{
				ImGui::SameLine();
				HelpMarker(HelpText);
			}
		}

		if (IsCurrentlyInTable)
		{
			ImGui::TableSetColumnIndex(1);
		}
		else
		{
			ImGui::NextColumn();
		}

		if (ShouldAlign)
		{
			Internal::FAlignData& AlignData = UIContext.AlignedStack.front();
			ImGui::SameLine(0.0f, AlignData.Dist);
			AlignData.PassedElements++;
		}
		else
		{
			ShiftCursorY(4.0f);
		}
		ImGui::PushItemWidth(ComboWidth);

		bool Modified = false;

		const std::string ID = std::format("##{}", Label);
		if (UI::BeginCombo(ID.c_str(), CurrentOption))
		{
			for (uint16_t Idx = 0; Idx < ArrSize; Idx++)
			{
				const bool IsSelected = (CurrentOption == Options[Idx]);
				if (ImGui::Selectable(Options[Idx], IsSelected))
				{
					CurrentOption = Options[Idx];
					*Selected = Idx;
					Modified = true;
				}

				if (IsSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			UI::EndCombo();
		}

		ImGui::PopItemWidth();

		if (IsCurrentlyInTable)
		{
			ImGui::TableNextRow();
		}
		else
		{
			ImGui::NextColumn();
			Draw::Underline();
		}

		return Modified;
	}
#endif

	template<std::size_t ArrSize, typename T>
	FORCEINLINE bool PropertyDropdown(const char* Label, 
									  const std::array<T, ArrSize>& Options,
									  int32_t* Selected, 
									  const char* HelpText = "",
									  const int ComboWidth = -1)
	{
		using ValueType = std::remove_cvref_t<T>;
		static_assert((std::is_same_v<ValueType, const char*> || Core::IsPair<ValueType>), "Unsupported type");

		const bool ShouldAlign = !UIContext.AlignedStack.empty();
		const bool IsCurrentlyInTable = InTable();

		if (IsCurrentlyInTable)
		{
			ImGui::TableSetColumnIndex(0);
		}

		if (ShouldAlign)
		{
			Internal::FAlignData& AlignData = UIContext.AlignedStack.front();
			if (AlignData.PassedElements > 0)
			{
				ImGui::SameLine(0.0f, AlignData.Dist);
			}
			AlignData.PassedElements++;
		}
		else
		{
			ShiftCursor(10.0f, 9.0f);
		}

		const char* CurrentOption = nullptr;
		if constexpr (std::is_same_v<ValueType, const char*>)
		{
			CurrentOption = Options.at(*Selected);
		}
		else if constexpr (Core::IsPair<ValueType>)
		{
			static_assert(Core::IsPairWithFirstArgConstChar<ValueType>, "The first pair argument (the label) is not const char*, which is required");
			/* Select the label from the std::pair (first argument). */
			CurrentOption = Options.at(*Selected).first;
		}

		/* Skip rendering text for '#' and '##' identifier labels. */
		if (Label && Label[0] != '#')
		{
			ImGui::Text(Label);
			if (std::strlen(HelpText) != 0)
			{
				ImGui::SameLine();
				HelpMarker(HelpText);
			}
		}

		if (IsCurrentlyInTable)
		{
			ImGui::TableSetColumnIndex(1);
		}
		else
		{
			ImGui::NextColumn();
		}

		if (ShouldAlign)
		{
			Internal::FAlignData& AlignData = UIContext.AlignedStack.front();
			ImGui::SameLine(0.0f, AlignData.Dist);
			AlignData.PassedElements++;
		}
		else
		{
			ShiftCursorY(4.0f);
		}
		ImGui::PushItemWidth(ComboWidth);

		bool Modified = false;

		const std::string ID = std::format("##{}", Label);
		if (UI::BeginCombo(ID.c_str(), CurrentOption))
		{
			for (uint16_t Idx = 0; Idx < ArrSize; Idx++)
			{
				const char* Option = nullptr;
				if constexpr (std::is_same_v<ValueType, const char*>)
				{
					Option = Options[Idx];
				}
				else if constexpr (Core::IsPair<ValueType>)
				{
					Option = Options.at(Idx).first;
				}
				else
				{
					LK_CORE_VERIFY(false);
				}

				const bool IsSelected = (Option == CurrentOption);
				if (ImGui::Selectable(Option, IsSelected))
				{
					CurrentOption = Option;
					*Selected = Idx;
					Modified = true;
				}

				if (IsSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			UI::EndCombo();
		}

		ImGui::PopItemWidth();

		if (IsCurrentlyInTable)
		{
			ImGui::TableNextRow();
		}
		else
		{
			ImGui::NextColumn();
			Draw::Underline();
		}

		return Modified;
	}


}

