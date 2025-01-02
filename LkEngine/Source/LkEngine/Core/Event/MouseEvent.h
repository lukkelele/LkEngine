#pragma once

#include "Event.h"

#include "LkEngine/Core/Input/MouseCodes.h"


namespace LkEngine {

	class MouseMovedEvent : public LEvent
	{
	public:
		MouseMovedEvent(const float x, const float y) 
			: m_MouseX(x), m_MouseY(y) {}

		float GetX() const { return m_MouseX; }
		float GetY() const { return m_MouseY; }

		const char* GetName() const override { return "MouseMoved"; }
		EEventType GetEventType() const override { return EEventType::MouseMoved; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
			return ss.str();
		}

	private:
		float m_MouseX, m_MouseY;
	};

	class MouseScrolledEvent : public LEvent
	{
	public:
		MouseScrolledEvent(const float xOffset, const float yOffset)
			: m_XOffset(xOffset)
			, m_YOffset(yOffset) {}

		float GetXOffset() const { return m_XOffset; }
		float GetYOffset() const { return m_YOffset; }

		const char* GetName() const override { return "MouseScrolled"; }
		EEventType GetEventType() const override { return EEventType::MouseScrolled; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset();
			return ss.str();
		}

	private:
		float m_XOffset, m_YOffset;
	};

	class LMouseButtonEvent : public LEvent
	{
	protected:
		LMouseButtonEvent(EMouseButton Button)
			: m_Button(Button) {}

	public:
		FORCEINLINE EMouseButton GetMouseButton() const 
		{ 
			return m_Button; 
		}

		const char* GetName() const override 
		{ 
			return "MouseButton"; 
		}

		glm::vec2 GetCoordinates() const
		{
			return Coordinates;
		}

	protected:
		EMouseButton m_Button;
		glm::vec2 Coordinates = { 0.0f, 0.0f };
	};

	class LMouseButtonPressedEvent : public LMouseButtonEvent
	{
	public:
		LMouseButtonPressedEvent() = delete;
		LMouseButtonPressedEvent(const EMouseButton Button, const glm::vec2& InCoordinates)
			: LMouseButtonEvent(Button) 
		{
			Coordinates = InCoordinates;
		}

		FORCEINLINE const char* GetName() const override { return "MouseButtonPressed"; }
		FORCEINLINE EEventType GetEventType() const override { return EEventType::MouseButtonPressed; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << static_cast<int>(m_Button) 
				<< ", Coords: [" << Coordinates.x << ", " << Coordinates.y << "]";
			return ss.str();
		}
	};

	class LMouseButtonReleasedEvent : public LMouseButtonEvent
	{
	public:
		LMouseButtonReleasedEvent() = delete;
		LMouseButtonReleasedEvent(const EMouseButton Button, const glm::vec2& InCoordinates)
			: LMouseButtonEvent(Button) 
		{
			Coordinates = InCoordinates;
		}

		FORCEINLINE const char* GetName() const override { return "MouseButtonReleased"; }
		FORCEINLINE EEventType GetEventType() const override { return EEventType::MouseButtonPressed; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << static_cast<int>(m_Button) 
				<< ", Coords: [" << Coordinates.x << ", " << Coordinates.y << "]";

			return ss.str();
		}
	};

	class LMouseButtonDownEvent : public LMouseButtonEvent
	{
	public:
		LMouseButtonDownEvent(EMouseButton Button)
			: LMouseButtonEvent(Button) {}

		const char* GetName() const override 
		{
			return "MouseButtonDown"; 
		}

		EEventType GetEventType() const override 
		{ 
			return EEventType::MouseButtonDown; 
		}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonDownEvent: " << (int)m_Button;

			return ss.str();
		}
	};


}
