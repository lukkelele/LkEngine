#pragma once

#include "LkEngine/Renderer/UI/Panel.h"

#include "EditorConsole/ConsoleMessage.h"


namespace LkEngine {
	
	class LEditorConsolePanel : public IPanel
	{
	public:
		LEditorConsolePanel();
		~LEditorConsolePanel();

		virtual void Render() override {}
		virtual void RenderUI(bool& IsOpen) override;

		virtual void OnProjectChanged(const TObjectPtr<LProject>& InProject) override;

		virtual void SerializeToYaml(YAML::Emitter& Out) const override;
		virtual void DeserializeFromYaml(const YAML::Node& Data) override;

	private:
		void RenderMenu(const ImVec2& InSize);
		void RenderConsole(const ImVec2& InSize);

		static const char* GetMessageType(const FConsoleMessage& Message);
		static const ImVec4& GetMessageColor(const FConsoleMessage& Message);
		ImVec4 GetToolbarButtonColor(const bool Value) const;

	private:
		static void PushMessage(const FConsoleMessage& Message);

	private:
		std::vector<FConsoleMessage> MessageBuffer;
		int16_t VerbosityFilters = (int16_t)EConsoleVerbosity::All;
		std::mutex MessageBufferMutex;

		bool bClearOnPlay = true;
		bool bDetailedPanelOpen = false;

		bool bEnableScrollToLatest = true;
		bool bScrollToLatest = false;
		float bPreviousScrollY = 0.0f;

		friend class LEditorConsoleSink;

		LCLASS(LEditorConsolePanel);
	};

}
