#pragma once

#include "Event.h"

#include "LkEngine/Input/MouseCodes.h"


namespace LkEngine {

	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(const float x, const float y) 
			: m_MouseX(x), m_MouseY(y) {}

		float GetX() const { return m_MouseX; }
		float GetY() const { return m_MouseY; }

		const char* GetName() const override { return "MouseMoved"; }
		EventType GetEventType() const override { return EventType::MouseMoved; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
			return ss.str();
		}

	private:
		float m_MouseX, m_MouseY;
	};

	class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(const float xOffset, const float yOffset)
			: m_XOffset(xOffset)
			, m_YOffset(yOffset) {}

		float GetXOffset() const { return m_XOffset; }
		float GetYOffset() const { return m_YOffset; }

		const char* GetName() const override { return "MouseScrolled"; }
		EventType GetEventType() const override { return EventType::MouseScrolled; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset();
			return ss.str();
		}

	private:
		float m_XOffset, m_YOffset;
	};

	class MouseButtonEvent : public Event
	{
	protected:
		MouseButtonEvent(MouseButton button)
			: m_Button(button) {}

	public:
		inline MouseButton GetMouseButton() const { return m_Button; }
		const char* GetName() const override { return "MouseButton"; }

	protected:
		MouseButton m_Button;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(MouseButton button)
			: MouseButtonEvent(button) {}

		const char* GetName() const override { return "MouseButtonPressed"; }
		EventType GetEventType() const override { return EventType::MouseButtonPressed; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << (int)m_Button;
			return ss.str();
		}
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(MouseButton button)
			: MouseButtonEvent(button) {}

		const char* GetName() const override { return "MouseButtonReleased"; }
		EventType GetEventType() const override { return EventType::MouseButtonPressed; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << (int)m_Button;
			return ss.str();
		}
	};

	class MouseButtonDownEvent : public MouseButtonEvent
	{
	public:
		MouseButtonDownEvent(MouseButton button)
			: MouseButtonEvent(button) {}

		const char* GetName() const override { return "MouseButtonDown"; }
		EventType GetEventType() const override { return EventType::MouseButtonDown; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonDownEvent: " << (int)m_Button;
			return ss.str();
		}
	};


}
