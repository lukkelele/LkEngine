#pragma once

#include <mutex>

#include <spdlog/sinks/base_sink.h>

#include <LkEngine/Core/Assert.h>

#include "LkEngine/Editor/EditorConsolePanel.h"


namespace LkEngine {

	class LEditorConsoleSink : public spdlog::sinks::base_sink<std::mutex>
	{
	public:
		explicit LEditorConsoleSink(const uint32_t InBufferCapacity)
			: MessageBufferCapacity(InBufferCapacity)
			, MessageBuffer(InBufferCapacity)
		{
		}

		virtual ~LEditorConsoleSink() = default;

		LEditorConsoleSink(const LEditorConsoleSink& Other) = delete;
		LEditorConsoleSink& operator=(const LEditorConsoleSink& Other) = delete;

	protected:
		virtual void sink_it_(const spdlog::details::log_msg& InLogMessage) override
		{
			spdlog::memory_buf_t Formatted;
			spdlog::sinks::base_sink<std::mutex>::formatter_->format(InLogMessage, Formatted);

			const std::string LongMessage = std::format("{}", Formatted);
			std::string ShortMessage = LongMessage;

			static constexpr int MAX_LENGTH_MESSAGE = 100;
			if (ShortMessage.length() > MAX_LENGTH_MESSAGE)
			{
				const std::size_t SpacePos = ShortMessage.find_first_of(' ', MAX_LENGTH_MESSAGE);
				if (SpacePos != std::string::npos)
				{
					ShortMessage.replace(SpacePos, ShortMessage.length() - 1, "...");
				}
			}

			MessageBuffer[MessageCount++] = FConsoleMessage(
				ShortMessage,
				LongMessage,
				GetMessageFlags(InLogMessage.level),
				std::chrono::system_clock::to_time_t(InLogMessage.time)
			);

			if (MessageCount == MessageBufferCapacity)
			{
				flush_();
			}
		}

		virtual void flush_() override
		{
			for (const FConsoleMessage& Message : MessageBuffer)
			{
				LEditorConsolePanel::PushMessage(Message);
			}

			MessageCount = 0;
		}

	private:
		static int16_t GetMessageFlags(spdlog::level::level_enum level)
		{
			int16_t Flags = 0;

			switch (level)
			{
				case spdlog::level::trace:
				case spdlog::level::debug:
				{
					Flags |= EConsoleVerbosity::Debug;
					break;
				}
				case spdlog::level::info:
				{
					Flags |= EConsoleVerbosity::Info;
					break;
				}
				case spdlog::level::warn:
				{
					Flags |= EConsoleVerbosity::Warning;
					break;
				}
				case spdlog::level::err:
				case spdlog::level::critical:
				{
					Flags |= EConsoleVerbosity::Error;
					break;
				}
			}

			return Flags;
		}

	private:
		uint32_t MessageCount = 0;
		std::vector<FConsoleMessage> MessageBuffer;
		uint32_t MessageBufferCapacity;
	};

}
