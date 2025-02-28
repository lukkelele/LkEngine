#pragma once

#include "Event.h"

#include "LkEngine/Core/Input/MouseCodes.h"
#include "LkEngine/Core/Math/Vector.h"


namespace LkEngine {

	class LMouseButtonPressedEvent : public LEvent
	{
	public:
		LMouseButtonPressedEvent() = delete;
		LMouseButtonPressedEvent(const EMouseButton InButton) : Button(InButton) {}

		FORCEINLINE EMouseButton GetMouseButton() const { return Button; }

		FORCEINLINE std::string ToString() const override
		{
			return std::format("MouseButtonPressed: {} ({})", Enum::ToString(Button), static_cast<int>(Button));
		}

	private:
		EMouseButton Button = EMouseButton::None;

		LEVENT(MouseButtonPressed);
	};

	class LMouseButtonReleasedEvent : public LEvent
	{
	public:
		LMouseButtonReleasedEvent() = delete;
		LMouseButtonReleasedEvent(const EMouseButton InButton) : Button(InButton) {}

		FORCEINLINE EMouseButton GetMouseButton() const { return Button; }

		FORCEINLINE std::string ToString() const override
		{
			return std::format("MouseButtonReleased: {} ({})", Enum::ToString(Button), static_cast<int>(Button));
		}

	private:
		EMouseButton Button = EMouseButton::None;

		LEVENT(MouseButtonReleased);
	};

	class LMouseMovedEvent : public LEvent
	{
	public:
		LMouseMovedEvent(const uint16_t InX, const uint16_t InY) 
			: PosX(InX)
			, PosY(InY) 
		{
		}

		FORCEINLINE float GetX() const { return PosX; }
		FORCEINLINE float GetY() const { return PosY; }
		FORCEINLINE std::pair<uint16_t, uint16_t> GetPos() const { return std::make_pair(PosX, PosY); }

		FORCEINLINE std::string ToString() const override
		{
			return std::format("MouseMovedEvent: ({}, {})", PosX, PosY);
		}

	private:
		uint16_t PosX = 0.0f;
		uint16_t PosY = 0.0f;

		LEVENT(MouseMoved);
	};

	class LMouseScrolledEvent : public LEvent
	{
	public:
		LMouseScrolledEvent() = delete;
		LMouseScrolledEvent(const float InOffsetX, const float InOffsetY)
			: OffsetX(InOffsetX)
			, OffsetY(InOffsetY) 
		{
		}

		FORCEINLINE float GetXOffset() const { return OffsetX; }
		FORCEINLINE float GetYOffset() const { return OffsetY; }

		FORCEINLINE std::string ToString() const override
		{
			return std::format("MouseScrolledEvent: ({}, {})", OffsetX, OffsetY);
		}

	private:
		float OffsetX = 0.0f;
		float OffsetY = 0.0f;

		LEVENT(MouseScrolled);
	};

}
