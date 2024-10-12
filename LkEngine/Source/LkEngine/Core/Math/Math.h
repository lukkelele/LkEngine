#pragma once

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif

#include <glm/glm.hpp>
#include <glm/ext/matrix_common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/norm.hpp>

#include <random>
#include <time.h>

#include "LkEngine/Core/Core.h"

#include "LkEngine/Core/Math/Vector.h"
#include "LkEngine/Core/Math/Rotator.h"
#include "LkEngine/Core/Math/Quaternion.h"


namespace LkEngine 
{
    struct LTransformComponent;
}

/* Box2D. */
struct b2Vec2;
struct b2Vec3;
/* ImGui. */
struct ImVec2;
struct ImVec4;

namespace LkEngine::Math {

    enum class Shape
    {
        None = 0,
        Rectangle,
        Circle,
        Triangle,
        Hexagon,
    };

	FORCEINLINE static glm::vec3 Scale(glm::vec3& Vector, float ScaleFactor)
	{
		return (Vector * ScaleFactor) / glm::length(Vector);
	}

	FORCEINLINE static bool DecomposeTransform(const glm::mat4& Transform, 
											   glm::vec3& Translation, 
											   glm::quat& Rotation, 
											   glm::vec3& Scale)
	{
		using T = float;

		glm::mat4 LocalMatrix(Transform);

		if (glm::epsilonEqual(LocalMatrix[3][3], static_cast<T>(0), glm::epsilon<T>()))
		{
			return false;
		}

		// Next take care of translation (easy).
		Translation = glm::vec3(LocalMatrix[3]);
		LocalMatrix[3] = glm::vec4(0, 0, 0, LocalMatrix[3].w);

		glm::vec3 Row[3] = {};
		// Now get scale and shear.
		for (glm::length_t i = 0; i < 3; ++i)
		{
			for (glm::length_t j = 0; j < 3; ++j)
			{
				Row[i][j] = LocalMatrix[i][j];
			}
		}

		// Compute X scale factor and normalize first row.
		Scale.x = glm::length(Row[0]);
		Row[0] = LkEngine::Math::Scale(Row[0], static_cast<T>(1));

		// Now, compute Y scale and normalize 2nd row.
		Scale.y = glm::length(Row[1]);
		Row[1] = LkEngine::Math::Scale(Row[1], static_cast<T>(1));

		// Next, get Z scale and normalize 3rd row.
		Scale.z = glm::length(Row[2]);
		Row[2] = LkEngine::Math::Scale(Row[2], static_cast<T>(1));

		// Rotation as quaternion.
		int i, j, k = 0;
		T SquareRoot, Trace = Row[0].x + Row[1].y + Row[2].z;
		if (Trace > static_cast<T>(0))
		{
			SquareRoot = sqrt(Trace + static_cast<T>(1));
			Rotation.w = static_cast<T>(0.5) * SquareRoot;
			SquareRoot = static_cast<T>(0.5) / SquareRoot;

			Rotation.x = SquareRoot * (Row[1].z - Row[2].y);
			Rotation.y = SquareRoot * (Row[2].x - Row[0].z);
			Rotation.z = SquareRoot * (Row[0].y - Row[1].x);
		} // End if > 0
		else
		{
			static int Next[3] = { 1, 2, 0 };
			i = 0;
			if (Row[1].y > Row[0].x)
			{
				i = 1;
			}
			if (Row[2].z > Row[i][i])
			{
				i = 2;
			}

			j = Next[i];
			k = Next[j];

			SquareRoot = sqrt(Row[i][i] - Row[j][j] - Row[k][k] + static_cast<T>(1.0));

			Rotation[i] = static_cast<T>(0.5) * SquareRoot;
			SquareRoot = static_cast<T>(0.5) / SquareRoot;
			Rotation[j] = SquareRoot * (Row[i][j] + Row[j][i]);
			Rotation[k] = SquareRoot * (Row[i][k] + Row[k][i]);
			Rotation.w = SquareRoot * (Row[j][k] - Row[k][j]);
		} // End if <= 0

		return true;
	}

    FORCEINLINE glm::mat4 TransformMatrix(glm::vec3& Translation, glm::quat& Rotation, glm::vec3& Scale)
    {
		return glm::translate(glm::mat4(1.0f), Translation) 
               * glm::toMat4(Rotation) 
			   * glm::scale(glm::mat4(1.0f), Scale);
    }

    glm::mat4 TransformMatrix2D(const glm::vec3& translation, float rot, const glm::vec3& scale);

    /// @TODO: documentation
	// [0,  width] -> [-1,1]
	// [0, height] -> [-1,1]
	// The near plane maps to Z = -1.
    template<typename T = float>
    glm::vec2 ConvertToWorldCoordinates(glm::vec2 NdcCoordinates, T WindowWidth, T WindowHeight)
    {
        static_assert(std::is_floating_point_v<T> || std::is_integral_v<T>);
        using F = std::conditional_t<std::is_floating_point_v<T>, T, float>;

		glm::vec4 ConvertedCoordinates = glm::vec4(
			(static_cast<F>(NdcCoordinates.x) / static_cast<F>(WindowWidth) - 0.5f) * 2.0f,  
			(static_cast<F>(NdcCoordinates.y) / static_cast<F>(WindowHeight) - 0.5f) * 2.0f, 
			-1.0, 
			1.0f
		);

		return glm::vec2(ConvertedCoordinates.x, ConvertedCoordinates.y);
    }

    //FORCEINLINE static T ScreenToWorldCoordinates2D(const glm::vec2& ScreenCoordinates, 
	template<typename TVector>
    FORCEINLINE static TVector ScreenToWorldCoordinates2D(const TVector& ScreenCoordinates, 
														  const glm::mat4& InverseProjectionMatrix, 
														  const glm::mat4& InverseViewMatrix, 
														  const glm::vec4& Viewport)
    {
		// Convert screen coordinates to normalized device coordinates.
        static_assert(std::disjunction_v<
                        std::is_same<TVector, glm::vec2>, 
                        std::is_same<TVector, LVector2>>, "TVector must valid vector type");

        const int WindowWidth = Viewport.z;
        const int WindowHeight = Viewport.w;

		glm::vec4 ClipCoordinates = glm::vec4(
			((2.0f * ScreenCoordinates.x) / (WindowWidth - 1.0f)),
			(1.0f - 2.0f * ScreenCoordinates.y) / WindowHeight,
			0.0f,
			1.0f
		);

		// Multiply by inverse projection matrix.
		glm::vec4 EyeCoordinates = InverseProjectionMatrix * ClipCoordinates;
		EyeCoordinates.z = -1.0f; // Point into the scene.
		EyeCoordinates.w = 0.0f;

		// Convert to World Coordinates.
		glm::vec4 WorldCoordinates = InverseViewMatrix * EyeCoordinates;

		//return TVector(glm::vec2(WorldCoordinates.x, WorldCoordinates.y));
		return TVector(WorldCoordinates.x, WorldCoordinates.y);
    }

    /**
     * ScreenToWorld
     *
     *  Viewport, (x, y, Width, Height)
     */
    template<typename TVector = glm::vec3, typename TVec2>
    FORCEINLINE static TVector ConvertScreenToWorldCoordinates(const TVec2& ScreenCoordinates, 
															   const float Depth, 
															   const glm::mat4& ViewMatrix, 
															   const glm::mat4& ProjectionMatrix, 
															   const glm::vec4& Viewport)
    {
        static_assert(std::conjunction_v<
						std::disjunction<
							std::is_same<TVector, glm::vec3>, 
							std::is_same<TVector, LVector>
						>,
						std::disjunction<
							std::is_same<TVec2, glm::vec2>, 
					        std::is_same<TVec2, LVector2> 
						>>, "TVector and TVec2 must valid vector types");
        LK_CORE_ASSERT((Depth >= 0.0f && Depth <= 1.0f), "Depth not in range of 0.0 to 1.0");
		LK_CORE_DEBUG_TAG("MathLibrary" "ConvertScreenToWorldCoordinates  Type='{}'", Meta::TypeName<TVector>());

		glm::vec3 ScreenCoordinatesWithDepth{};

		if constexpr (std::is_same_v<TVec2, glm::vec2>)
		{
			ScreenCoordinatesWithDepth = { ScreenCoordinates.x, ScreenCoordinates.y, Depth };
		}
		else if constexpr (std::is_same_v<TVec2, LVector2>)
		{
			ScreenCoordinatesWithDepth = { ScreenCoordinates.X, ScreenCoordinates.Y, Depth };
		}
		else
		{
			LK_CORE_ASSERT(false, "ConvertScreenToWorldCoordinates failed");
		}

		return TVector(glm::unProject(ScreenCoordinatesWithDepth, ViewMatrix, ProjectionMatrix, Viewport));
    }

#if 0
    template<>
    FORCEINLINE static LVector ConvertScreenToWorldCoordinates(const glm::vec2& ScreenCoordinates, 
                                                               const float Depth, 
                                                               const glm::mat4& ViewMatrix, 
                                                               const glm::mat4& ProjectionMatrix, 
                                                               const glm::vec4& Viewport)
    {
        LK_CORE_ASSERT((Depth >= 0.0f && Depth <= 1.0f), "Depth invalid");
        const glm::vec3 ScreenCoordinatesWithDepth = { ScreenCoordinates, Depth };

        return LVector(glm::unProject(ScreenCoordinatesWithDepth, ViewMatrix, ProjectionMatrix, Viewport));
    }
#endif

    glm::vec2 ScreenToWorld2D(const glm::vec2& ndc, const glm::mat4& inv_proj, const LTransformComponent& Transform);
    float Get2DRotationFromQuaternion(const glm::quat& quat);

	template<typename TVector>
	TVector GetForwardVector(const glm::mat4& TransformMatrix)
	{
        static_assert(std::disjunction_v<
					  std::is_same<TVector, glm::vec3>, 
					  std::is_same<TVector, LVector>>, 
					  "TVector must valid vector type");
		return TVector(glm::normalize(glm::vec3(-TransformMatrix[2][0], -TransformMatrix[2][1], -TransformMatrix[2][2])));
	}

	template<typename TVector>
	TVector GetRightVector(const glm::mat4& TransformMatrix)
	{
        static_assert(std::disjunction_v<
					  std::is_same<TVector, glm::vec3>, 
					  std::is_same<TVector, LVector>>, 
					  "TVector must valid vector type");
		return glm::normalize(glm::vec3(TransformMatrix[0][0], TransformMatrix[0][1], TransformMatrix[0][2]));
	}

	template<typename TVector>
	TVector Normalize(const TVector& InVector)
	{
        static_assert(std::disjunction_v<
					  std::is_same<TVector, LVector>,
					  std::is_same<TVector, LVector2>, 
					  std::is_same<TVector, glm::vec3>, 
					  std::is_same<TVector, glm::vec2>>, 
					  "TVector must valid vector type");
		return TVector(glm::normalize(InVector));
	}

}

namespace LkEngine::Utils {

    glm::vec2 ConvertToGlm(const b2Vec2& vec2);
    glm::vec3 ConvertToGlm(const b2Vec3& vec3);

    b2Vec2 ConvertToB2(const glm::vec2& vec2);
    b2Vec3 ConvertToB2(const glm::vec3& vec3);

    ImVec2 ConvertToImVec2(const glm::vec2& vec2);
    ImVec4 ConvertToImVec4(const glm::vec4& vec4);

}
