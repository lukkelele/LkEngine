/**
 * @file
 * @brief Viewport.
 */
#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "LkEngine/Core/Delegate/Delegate.h"
#include "LkEngine/Core/Math/Math.h"

namespace LkEngine {

	/**
	 * @class LViewport
	 *
	 * @ingroup Window
	 */
	class LViewport : public LObject
	{
		LK_DECLARE_MULTICAST_DELEGATE(FOnViewportSizeUpdated, const uint16_t, const uint16_t);
	public:
		LViewport();
		~LViewport() = default;

		/**
		 * @brief Update the viewport.
		 *
		 * @details Broadcast FOnViewportSizeUpdated on detected changes since last tick.
		 */
		void Update();

		/**
		 * @brief Get viewport size.
		 */
		FORCEINLINE LVector2 GetSize() const { return Size; }

		/**
		 * @brief Get viewport position.
		 */
		FORCEINLINE LVector2 GetPosition() const { return Position; }

		/**
		 * @brief Get the viewport scalers.
		 */
		FORCEINLINE LVector2 GetScalers() const { return Scalers; }

		FORCEINLINE void SetSize(const LVector2& NewSize) 
		{ 
			if (Size != NewSize)
			{
				Size = NewSize;
				bDirty = true;
			}
		}

		FORCEINLINE void SetSize(const float InX, const float InY) 
		{ 
			if ((Size.X != InX) || (Size.Y != InY))
			{
				Size.X = InX;
				Size.Y = InY;
				bDirty = true;
			}
		}

		template<typename T>
		FORCEINLINE void SetSizeX(const T InX)
		{
			static_assert(std::is_floating_point_v<T>, "Invalid type, cannot set size");
			if (Size.X != InX)
			{
				Size.X = InX;
				bDirty = true;
			}
		}

		template<typename T>
		FORCEINLINE void SetSizeY(const T InY)
		{
			static_assert(std::is_floating_point_v<T>, "Invalid type, cannot set size");
			if (Size.Y != InY)
			{
				Size.Y = InY;
				bDirty = true;
			}
		}

		/**
		 * @brief Set viewport position.
		 */
		FORCEINLINE void SetPosition(const LVector2& NewPosition) 
		{ 
			if (Position != NewPosition)
			{
				Position = NewPosition;
			}
		}

		/**
		 * @brief Sey position in the X-axis.
		 */
		FORCEINLINE void SetPositionX(const float NewPosX) 
		{ 
			Position.X = NewPosX;
			bDirty = true;
		}

		/**
		 * @brief Sey position in the Y-axis.
		 */
		FORCEINLINE void SetPositionY(const float NewPosY) 
		{ 
			Position.Y = NewPosY;
			bDirty = true;
		}

		/**
		 * Set viewport bounds for top-left or bottom-right bound.
		 */
		FORCEINLINE void SetViewportBounds(const uint8_t Index, const LVector2& Bounds)
		{ 
			LK_CORE_ASSERT((Index >= 0) && (Index <= 1), "Invalid viewport index: {}", Index); 
			if (PrimaryViewportBounds[Index] != Bounds)
			{
				PrimaryViewportBounds[Index] = Bounds;
				//LK_CORE_TRACE_TAG("Viewport", "Set viewport bounds {} to {}", Index, Bounds.ToString());
				bDirty = true;
			}
		}

		/**
		 * @brief Set viewport bounds on the X axis for top-left or bottom-right bound.
		 */
		FORCEINLINE void SetViewportBoundsX(const uint8_t Index, const float Bound)
		{ 
			LK_CORE_ASSERT((Index >= 0) && (Index <= 1), "Invalid viewport index: {}", Index); 
			if (PrimaryViewportBounds[Index].X != Bound)
			{
				PrimaryViewportBounds[Index].X = Bound;
				//LK_CORE_TRACE_TAG("Viewport", "Set viewport bounds {} on the X-axis to {}", Index, Bound);
				bDirty = true;
			}
		}

		/**
		 * @brief Set viewport bounds on the Y axis for top-left or bottom-right bound.
		 */
		FORCEINLINE void SetViewportBoundsY(const uint8_t Index, const float Bound)
		{ 
			LK_CORE_ASSERT((Index >= 0) && (Index <= 1), "Invalid viewport index: {}", Index); 
			if (PrimaryViewportBounds[Index].Y != Bound)
			{
				PrimaryViewportBounds[Index].Y = Bound;
				//LK_CORE_TRACE_TAG("Viewport", "Set viewport bounds {} on the Y-axis to {}", Index, Bound);
				bDirty = true;
			}
		}

		/**
		 * @brief Return viewport bounds array.
		 */
		FORCEINLINE const LVector2* GetViewportBounds() const
		{
			return PrimaryViewportBounds.data();
		}

		FORCEINLINE LVector2& GetViewportBounds(const uint8_t Index) 
		{
			LK_VERIFY((Index >= 0) && (Index <= 1), "Invalid viewport index");
			return PrimaryViewportBounds[Index];
		}

		FORCEINLINE const LVector2& GetViewportBounds(const uint8_t Index) const
		{
			LK_VERIFY((Index >= 0) && (Index <= 1), "Invalid viewport index");
			return PrimaryViewportBounds[Index];
		}

		FORCEINLINE void SetScalers(const LVector2& NewScalers) 
		{ 
			if (!NewScalers.IsNull() && (Scalers != NewScalers))
			{
				Scalers = NewScalers;
			}
		}

		/**
		 * @brief Set the viewport as dirty.
		 */
		FORCEINLINE void SetDirty(const bool Dirty)
		{
			bDirty = Dirty;
		}

		static void SetViewport(const int PosX, const int PosY, const int Width, const int Height);

	public:
		FOnViewportSizeUpdated OnSizeUpdated;
	private:
		TVector2<uint16_t> Size = { 0.0f, 0.0f };
		TVector2<uint16_t> Position = { 0.0f, 0.0f };
		TVector2<float> Scalers = { 1.0f, 1.0f };

		std::array<LVector2, 2> PrimaryViewportBounds = {
			LVector2(0.0f, 0.0f), 
			LVector2(0.0f, 0.0f) 
		};

		bool bDirty = true;

		LCLASS(LViewport);
	};

}
