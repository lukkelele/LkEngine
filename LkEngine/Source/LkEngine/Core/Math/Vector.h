/**
 * @file
 * @brief Mathematical vectors.
 */
#pragma once

/**
 * @ingroup Math
 *
 * @defgroup Vector
 */

#include <type_traits>
#include <ostream>

#define LK_MATH_VECTOR_ANONYMOUS_STRUCT           0
#define LK_MATH_VECTOR3_EXPLICIT_GLM_CONSTRUCTOR  0

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

namespace LkEngine {

	/**
	 * @ingroup Vector
	 * @{
	 */

	/**
	 * @enum EVectorAxis
	 */
	enum class EVectorAxis : uint32_t
	{
		X = LK_BIT(0),
		Y = LK_BIT(1),
		Z = LK_BIT(2),
		W = LK_BIT(3),

		/* Aliases. */
		R = X,
		G = Y,
		B = Z,
		A = W,
	};
	LK_ENUM_CLASS(EVectorAxis);

	/**
	 * @typedef LVector2
	 * Alias for TVector2<float>.
	 */
	using LVector2 = TVector2<float>;

	/**
	 * @typedef LVector3
	 * Alias for TVector3<float>.
	 */
	using LVector3 = TVector3<float>;

	/**
	 * @typedef LVector4
	 * Alias for TVector4<float>.
	 */
	using LVector4 = TVector4<float>;

	/**
	 * @typedef LVector
	 * Alias for TVector3<float>.
	 *
	 * @details The most commonly used vector in 3D contexts is the three-component vector.
	 */
	using LVector  = LVector3;

	/** @} */

}
