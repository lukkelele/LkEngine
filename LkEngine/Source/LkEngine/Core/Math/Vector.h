#pragma once

#include <type_traits>
#include <ostream>

#define LK_MATH_VECTOR_ANONYMOUS_STRUCT           0
#define LK_MATH_VECTOR3_EXPLICIT_GLM_CONSTRUCTOR  0

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"


namespace LkEngine {

	enum class EVectorAxis : uint32_t
	{
		X = LK_BIT(0),
		Y = LK_BIT(1),
		Z = LK_BIT(2),
		W = LK_BIT(3)
	};
	LK_ENUM_CLASS(EVectorAxis);

	/* Typedefs. */
	using LVector2 = TVector2<float>;
	using LVector3 = TVector3<float>;
	using LVector4 = TVector4<float>;
	using LVector  = LVector3;

}
