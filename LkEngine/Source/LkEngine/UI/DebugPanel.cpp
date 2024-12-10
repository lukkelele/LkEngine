#include "LKpch.h"
#include "DebugPanel.h"



namespace LkEngine {

	LDebugPanel::LDebugPanel()
	{
		LCLASS_REGISTER();
	}

	void LDebugPanel::OnRender()
	{
	}

	void LDebugPanel::OnRenderUI(bool& IsOpen)
	{
		if (bWindow_ObjectReferences)
		{
			UI_ObjectReferences();
		}
	}

	void LDebugPanel::UI_ObjectReferences()
	{
		static ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoDocking;
		ImGui::Begin("Object References", &bWindow_ObjectReferences, WindowFlags);
		{
			static std::size_t SelectedIndex = -1;
			static std::size_t SelectedIndexPrev = -1;

			/* Items to show, shows a scrollbar if greater. */
			//static int MaxVisibleItems = 20;
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
					//sprintf_s(Label, LK_ARRAYSIZE(Label), "%s ##%lld", LiveObject->GetClass()->GetName().c_str(), reinterpret_cast<intptr_t>(LiveObject.Get()));
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

		ImGui::End();
	}

}
