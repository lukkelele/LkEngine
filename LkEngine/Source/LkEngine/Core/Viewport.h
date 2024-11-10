#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "LkEngine/Core/Delegate/Delegate.h"
#include "LkEngine/Core/Math/Math.h"


namespace LkEngine {


	class LViewport : public LObject
	{
		LK_DECLARE_MULTICAST_DELEGATE(FOnViewportSizeUpdated, const uint16_t, const uint16_t);
	public:
		LViewport();
		~LViewport() = default;

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

		/// @Marked: Makes no sense to use this on a member variable really.
		/// Should modify the viewport bounds array instead.
		FORCEINLINE void SetSize(const LVector2& NewSize) 
		{ 
			if (Size != NewSize)
			{
				Size = NewSize;
				bDirty = true;
			}
		}

		/// @Marked: Makes no sense to use this on a member variable really.
		/// Should modify the viewport bounds array instead.
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

		/// @Marked: Makes no sense to use this on a member variable really.
		/// Should modify the viewport bounds array instead.
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

		/// @TODO: This should modify the viewport bounds (indexed 0), or?
		/**
		 * @brief Set viewport position.
		 */
		FORCEINLINE void SetPosition(const LVector2& NewPosition) 
		{ 
			if (Position != NewPosition)
			{
				Position = NewPosition;
				bDirty = true;
			}
		}

		/**
		 * @brief Set viewport bounds for top-left or bottom-right bound.
		 */
		FORCEINLINE void SetViewportBounds(const uint8_t Index, const LVector2& Bounds)
		{ 
			LK_VERIFY((Index >= 0) && (Index <= 1), "Invalid viewport index: {}", Index); 
			if (ViewportBounds[Index] != Bounds)
			{
				ViewportBounds[Index] = Bounds;
				LK_CORE_DEBUG_TAG("Viewport", "Set viewport bounds {} to {}", Index, Bounds.ToString());

				Size.X = (ViewportBounds[1].X - ViewportBounds[0].X); 
				Size.Y = (ViewportBounds[1].Y - ViewportBounds[0].Y);
				LK_CORE_DEBUG_TAG("Viewport", "Size: {}", Size.ToString());

				/// Do not calculate scalers here.
			#if 0
				Scalers.X = (Size.X / ViewportBounds[1].X);
				Scalers.Y = (Size.Y / ViewportBounds[1].Y);
			#endif

				/* Always require full when viewport bounds are modified. */
				bDirty = true;
			}
		}

		/**
		 * @brief Return viewport bounds array.
		 */
		FORCEINLINE const LVector2* GetViewportBounds() const
		{
			return ViewportBounds;
		}

		FORCEINLINE LVector2& GetViewportBounds(const uint8_t Index) 
		{
			LK_VERIFY((Index >= 0) && (Index <= 1), "Invalid viewport index");
			return ViewportBounds[Index];
		}

		FORCEINLINE const LVector2& GetViewportBounds(const uint8_t Index) const
		{
			LK_VERIFY((Index >= 0) && (Index <= 1), "Invalid viewport index");
			return ViewportBounds[Index];
		}

		FORCEINLINE void SetScalers(const LVector2& NewScalers) 
		{ 
			if (Scalers != NewScalers)
			{
				Scalers = NewScalers;
				bDirty = true;
			}
		}

	public:
		FOnViewportSizeUpdated OnSizeUpdated;
	private:
		TVector2<uint16_t> Size = { 0.0f, 0.0f };
		TVector2<uint16_t> Position = { 0.0f, 0.0f };
		TVector2<uint16_t> Scalers = { 1.0f, 1.0f };

		LVector2 ViewportBounds[2] = { 
			{ 0.0f, 0.0f }, 
			{ 0.0f, 0.0f } 
		};

		bool bDirty = true;

		LCLASS(LViewport);
	};

}
