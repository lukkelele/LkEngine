/**
 * @file
 * @brief Mathematics used by the engine.
 */
#pragma once

/**
 * @defgroup Math
 */

#include <random>
#include <time.h>

#include <glm/glm.hpp>
#include <glm/ext/matrix_common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/norm.hpp>

#include "LkEngine/Core/Core.h"

#include "LkEngine/Core/Math/Vector.h"
#include "LkEngine/Core/Math/Rotator.h"
#include "LkEngine/Core/Math/Quaternion.h"

namespace LkEngine 
{
    struct LTransformComponent;

	static std::string ToString(const LVector2& Vector, const uint8_t F = 2)
	{
		return Vector.ToString();
	}

	static std::string ToString(const LVector& Vector, const uint8_t F = 2)
	{
		return Vector.ToString();
	}

	static std::string ToString(const LVector4& Vector, const uint8_t F = 2)
	{
		return Vector.ToString();
	}

	static std::string ToString(const glm::vec3& Vector, const uint8_t F = 2)
	{
		return LK_FMT_LIB::format("({:.2f}, {:.2f}, {:.2f})", Vector.x, Vector.y, Vector.z);
	}
}

struct b2Vec2;
struct b2Vec3;

namespace LkEngine::Math {

	/**
	 * @brief Engine Mathematics.
	 *
	 * @note Most mathematical functions with set vector types are to become templated in the future.
	 *
	 * @ingroup Math
	 * @{
	 */

    /**
     * @enum EShape
	 * @details Geometrical shape.
     */
    enum class Shape
    {
        None = 0,
        Rectangle,
        Circle,
        Triangle,
        Hexagon,
    };

	/**
	 * @brief Scale a vector.
	 */
	FORCEINLINE glm::vec3 Scale(glm::vec3& Vector, const float ScaleFactor)
	{
		return (Vector * ScaleFactor) / glm::length(Vector);
	}

	/**
	 * @brief Decompose a transform.
	 */
	FORCEINLINE bool DecomposeTransform(const glm::mat4& Transform, glm::vec3& Translation, glm::quat& Rotation, glm::vec3& Scale)
	{
		using T = float;

		glm::mat4 LocalMatrix(Transform);

		if (glm::epsilonEqual(LocalMatrix[3][3], static_cast<T>(0), glm::epsilon<T>()))
		{
			return false;
		}

		Translation = glm::vec3(LocalMatrix[3]);
		LocalMatrix[3] = glm::vec4(0, 0, 0, LocalMatrix[3].w);

		glm::vec3 Row[3] = {};
		/* Scale and shear. */
		for (glm::length_t i = 0; i < 3; ++i)
		{
			for (glm::length_t j = 0; j < 3; ++j)
			{
				Row[i][j] = LocalMatrix[i][j];
			}
		}

		/* Compute the X-scale and normalize the first row. */
		Scale.x = glm::length(Row[0]);
		Row[0] = LkEngine::Math::Scale(Row[0], static_cast<T>(1));

		/* Compute the Y-scale and normalize the second row. */
		Scale.y = glm::length(Row[1]);
		Row[1] = LkEngine::Math::Scale(Row[1], static_cast<T>(1));

		/* Get the Z-scale and normalize the third row. */
		Scale.z = glm::length(Row[2]);
		Row[2] = LkEngine::Math::Scale(Row[2], static_cast<T>(1));

		/* Get the rotation as a quaternion. */
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
		}
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
		}

		return true;
	}

    /**
     * @brief Calculate a transform matrix.
     */
    FORCEINLINE glm::mat4 TransformMatrix(glm::vec3& Translation, glm::quat& Rotation, glm::vec3& Scale)
    {
		return glm::translate(glm::mat4(1.0f), Translation) 
               * glm::toMat4(Rotation) 
			   * glm::scale(glm::mat4(1.0f), Scale);
    }

    glm::mat4 TransformMatrix2D(const glm::vec3& translation, float rot, const glm::vec3& scale);

    /**
     * @brief Calculate world coordinates.
     */
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

    /**
     * @brief Convert screen coordinates to its world counterpart in 2D.
	 *
	 * @deprecated No longer used.
     */
	template<typename TVector>
    FORCEINLINE TVector ScreenToWorldCoordinates2D(const TVector& ScreenCoordinates, 
		                                           const glm::mat4& InverseProjectionMatrix, 
												   const glm::mat4& InverseViewMatrix, 
												   const glm::vec4& Viewport)
    {
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

		/* Multiply by the inverse projection. */
		glm::vec4 EyeCoordinates = InverseProjectionMatrix * ClipCoordinates;
		EyeCoordinates.z = -1.0f; // Point into the scene.
		EyeCoordinates.w = 0.0f;

		glm::vec4 WorldCoordinates = InverseViewMatrix * EyeCoordinates;

		return TVector(WorldCoordinates.x, WorldCoordinates.y);
    }

    /**
     * @brief Convert screen coordinates to its world counterpart.
     */
    template<typename TVector = glm::vec3, typename TVec2>
    FORCEINLINE TVector ConvertScreenToWorldCoordinates(const TVec2& ScreenCoordinates, 
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
		LK_CORE_DEBUG_TAG("MathLibrary" "ConvertScreenToWorldCoordinates  Type='{}'", Core::TypeName<TVector>());

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

    glm::vec2 ScreenToWorld2D(const glm::vec2& ndc, const glm::mat4& inv_proj, const LTransformComponent& Transform);
    float Get2DRotationFromQuaternion(const glm::quat& quat);

	/**
	 * @brief Calculate the forward vector from a transform matrix.
	 * @return The calculated forward vector of type TVector.
	 */
	template<typename TVector>
	FORCEINLINE TVector GetForwardVector(const glm::mat4& TransformMatrix)
	{
        static_assert(std::disjunction_v<
					  std::is_same<TVector, glm::vec3>, 
					  std::is_same<TVector, LVector>>, 
					  "TVector must valid vector type");
		return TVector(glm::normalize(glm::vec3(-TransformMatrix[2][0], -TransformMatrix[2][1], -TransformMatrix[2][2])));
	}

	/**
	 * @brief Calculate the right vector from a transform matrix.
	 * @return The calculated right vector of type TVector.
	 */
	template<typename TVector>
	FORCEINLINE TVector GetRightVector(const glm::mat4& TransformMatrix)
	{
        static_assert(std::disjunction_v<
					  std::is_same<TVector, glm::vec3>, 
					  std::is_same<TVector, LVector>>, 
					  "TVector must valid vector type");
		return glm::normalize(glm::vec3(TransformMatrix[0][0], TransformMatrix[0][1], TransformMatrix[0][2]));
	}

	template<typename TVector>
	FORCEINLINE TVector Normalize(const TVector& InVector)
	{
        static_assert(std::disjunction_v<
					  std::is_same<TVector, LVector>,
					  std::is_same<TVector, LVector2>, 
					  std::is_same<TVector, glm::vec3>, 
					  std::is_same<TVector, glm::vec2>>, 
					  "TVector must valid vector type");
		return TVector(glm::normalize(InVector));
	}

	/** @} */

}

