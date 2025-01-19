#pragma once

#include <type_traits>
#include <ostream>

#define LK_MATH_VECTOR_ANONYMOUS_STRUCT           0
#define LK_MATH_VECTOR3_EXPLICIT_GLM_CONSTRUCTOR  0

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"


namespace LkEngine {

	/* Typedefs. */
	using LVector2 = TVector2<float>;
	using LVector3 = TVector3<float>;
	using LVector4 = TVector4<float>;
	using LVector  = LVector3;

}
