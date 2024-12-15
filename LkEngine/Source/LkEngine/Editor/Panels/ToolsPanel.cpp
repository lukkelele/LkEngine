#include "LKpch.h"
#include "ToolsPanel.h"


namespace LkEngine {

	bool LToolsPanel::bWindow_ObjectReferences = false;
	bool LToolsPanel::bWindow_RegisteredFonts = false;

	LToolsPanel::LToolsPanel()
	{
		LOBJECT_REGISTER();
	}

	void LToolsPanel::OnRenderUI(bool& IsOpen)
	{
		if (bWindow_ObjectReferences)
		{
			UI_ObjectReferences();
		}

		if (bWindow_RegisteredFonts)
		{ 
			UI_RegisteredFonts();
		}

		IsOpen = (bWindow_ObjectReferences || bWindow_RegisteredFonts);
	}

	void LToolsPanel::UI_ObjectReferences()
	{
		static ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoDocking;
		ImGui::SetNextWindowSize(ImVec2(720, 960), ImGuiCond_FirstUseEver);
		ImGui::Begin("Object References", &bWindow_ObjectReferences, WindowFlags);
		{
			static std::size_t SelectedIndex = -1;
			static std::size_t SelectedIndexPrev = -1;

			/* Items to show, shows a scrollbar if greater. */
			static int MaxVisibleItems = std::numeric_limits<int>::max();
			static std::vector<bool> Selected(MaxVisibleItems, false);

			/* Show only static class object references. */
			static bool bFilterByStaticClassObjects = true;

			static std::vector<TObjectPtr<LObject>> LiveObjects;
			const int LiveObjectCount = TObjectPtr_Internal::GetLiveObjects(LiveObjects, 
																			bFilterByStaticClassObjects);
			/* Resize if needed. */
			if (Selected.size() < LiveObjects.size())
			{
				LK_CORE_INFO("Resizing selected vector");
				Selected.resize(LiveObjects.size(), false);
			}

			/**
			 * Clear selection if selecting indices too large.
			 * Can happen if filtering is toggled whilst having an object
			 * reference selected.
			 */
			if (SelectedIndex >= Selected.size()) 
			{
				SelectedIndex = -1;
				SelectedIndexPrev = -1;
			}

			if (LiveObjects.size() < MaxVisibleItems)
			{
				MaxVisibleItems = static_cast<int>(LiveObjects.size());
			}

			/* Calculate the max height for the window. */
			const float ItemHeight = ImGui::GetTextLineHeightWithSpacing();
			float ObjectsWindowHeight = MaxVisibleItems * ItemHeight;
			if (SelectedIndex == -1)
			{
				float ObjectsWindowHeight = 0;
			}

			ImGui::Text("Total Live References: %d", LiveObjectCount);
			ImGui::Checkbox("Filter by single class only", &bFilterByStaticClassObjects);

			static constexpr ImGuiTableFlags TableFlags = ImGuiTableFlags_Borders 
				| ImGuiTableFlags_RowBg 
				| ImGuiTableFlags_ScrollY 
				| ImGuiTableFlags_Sortable
				| ImGuiTableFlags_SizingStretchProp;

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(2, 2));
			if (ImGui::BeginTable("Live Objects", 2, TableFlags, ImVec2(0, ObjectsWindowHeight)))
			{
				ImGui::TableSetupColumn("Object Index", ImGuiTableColumnFlags_IndentDisable);
				ImGui::TableSetupColumn("Class Name", ImGuiTableColumnFlags_IndentDisable);
				ImGui::TableHeadersRow();

				int Index = 0;
				for (const TObjectPtr<LObject>& LiveObject : LiveObjects)
				{
					LK_CORE_VERIFY(LiveObject->GetClass(), "{} object has invalid LClass reference", LiveObject->ClassName());

					/* Display the object number. */
					ImGui::TableNextColumn();
					ImGui::Text("%d", (Index + 1));

					/* Object classname. */
					char Label[200];
					std::sprintf(Label, "%s ##%lld", LiveObject->GetClass()->GetName().c_str(), reinterpret_cast<uintptr_t>(LiveObject.Get()));

					const bool bIsCurrentlySelected = Selected[Index];
					bool bSelected = false;

					ImGui::TableNextColumn();
					if (ImGui::Selectable(Label, Selected[Index], ImGuiSelectableFlags_SpanAllColumns))
					{
						bSelected = true;
					}

					/* Set to false if item already was selected. */
					if (bSelected && bIsCurrentlySelected)
					{
						Selected[Index] = false;
						SelectedIndex = -1;
					}
					/* Select the item that was not previously selected. */
					else if (bSelected && !bIsCurrentlySelected)
					{
						std::fill(Selected.begin(), Selected.end(), false);
						SelectedIndex = Index;
						Selected[Index] = true;
					}

					Index++;
				}

				ImGui::EndTable();
			}
			ImGui::PopStyleVar(2);


			/* Display information about a selected object, if any is selected. */
			if ((SelectedIndex != -1) && (SelectedIndex < LiveObjects.size()))
			{
				/* Limit visible items. */
				MaxVisibleItems = 20;

				/* 
				 * TODO: Should use a weak pointer here so the viewing of an object
				 *       does not extend its lifetime and thus prohibit its (potential) destruction.
				 */
				TObjectPtr<LObject> SelectedObject = LiveObjects.at(SelectedIndex);

				ImGui::BeginChild("Object Information", ImVec2(0, 0), true, ImGuiWindowFlags_NoNavInputs);
				{
					static constexpr ImGuiTableFlags ObjectInfoTableFlags = ImGuiTableFlags_Borders
						| ImGuiTableFlags_RowBg
						| ImGuiTableFlags_SizingFixedFit;

					const LClass* Class = SelectedObject->GetClass();
					if (ImGui::BeginTable("##ObjectInfo", 2, ObjectInfoTableFlags, ImVec2(0, 0)))
					{
						/* Name. */
						ImGui::TableNextColumn();
						ImGui::Text("Name");
						ImGui::TableNextColumn();
						ImGui::Text("%s", Class->GetName().c_str());

						/* Reference Count. */
						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						ImGui::Text("References");
						ImGui::TableNextColumn();
						ImGui::Text("%d", SelectedObject->GetReferenceCount());

						/* Memory Address. */
						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						ImGui::Text("Address");
						ImGui::TableNextColumn();
						ImGui::Text("0x%p", reinterpret_cast<intptr_t>(SelectedObject.Get()));

						ImGui::EndTable(); /* */
					}
				}
				ImGui::EndChild();
			}
			/* No object selected, expand window. */
			else
			{
				MaxVisibleItems = std::numeric_limits<int>::max();
			}

		}

		LK_UI_DEBUG_ON_HOVER();
		ImGui::End();
	}

	void LToolsPanel::UI_RegisteredFonts()
	{
		static std::vector<FFontEntry> RegisteredFonts;
		const std::size_t RegisteredFontsCount = UI::Font::GetRegistered(RegisteredFonts);

		/* Place the default font named 'Default' first. */
		std::sort(RegisteredFonts.begin(), RegisteredFonts.end(), [](const FFontEntry& A, const FFontEntry& B)
		{
			return (A.Name == "Default") || (B.Name != "Default" && A.Name < B.Name);
		});

		/* Determine the maximum width based on the longest font name. */
		auto FontEntryComparitor = [](const FFontEntry& A, const FFontEntry& B)
		{
			return A.Name.length() < B.Name.length();
		};

		/* Calculate the width dynamically based on the longest name, with some padding. */
		static constexpr float NamePadding = 12.0f;
		const auto LongestNameIt = std::max_element(RegisteredFonts.begin(), RegisteredFonts.end(), FontEntryComparitor);
		const float NameColumnWidth = ImGui::CalcTextSize(LongestNameIt->Name.c_str()).x + NamePadding;

		const std::string WindowName = std::format("Registered Fonts: {}", RegisteredFontsCount);
		ImGui::Begin(WindowName.c_str(), &bWindow_RegisteredFonts);
		if (ImGui::BeginTable("RegisteredFontsTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
		{
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, NameColumnWidth);
			ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 34.0f);
			ImGui::TableSetupColumn("Filepath", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableHeadersRow();
			for (const auto& Font : RegisteredFonts)
			{
				ImGui::TableNextRow();
				/* Column 1: Font Name */
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s", Font.Name.c_str());

				/* Column 2: Font Size */
				ImGui::TableSetColumnIndex(1);
				ImGui::Text("%.1f", Font.Size);

				/* Column 3: Filepath */
				ImGui::TableSetColumnIndex(2);
				ImGui::Text("%s", Font.FilePath.c_str());
			}

			ImGui::EndTable();
		}

		LK_UI_DEBUG_ON_HOVER();

		ImGui::End();
	}

}
