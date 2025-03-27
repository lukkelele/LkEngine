/**
 * @file
 * @brief Raycasting
 */
#pragma once

#include <glm/glm.hpp>

#include "LkEngine/Core/Math/Vector.h"
#include "LkEngine/Core/Math/AABB.h"

namespace LkEngine {

	/**
	 * @brief Raycast physics
	 *
	 * @ingroup Physics
	 * @defgroup Raycast
	 * @{
	 */

	/**
	 * @struct FRayCast
	 * @deprecated To be removed soon.
	 */
	struct FRayCast
	{
		LVector Pos{};
		LVector Dir{};

		FRayCast() = default;
		FRayCast(const LVector& InPos, const LVector& InDir)
			: Pos(InPos)
			, Dir(InDir) 
		{
		}
	};

	/**
	 * @struct FRay
	 * 
	 * @details Individual ray with an origin and direction.
	 */
	struct FRay
    {
		LVector Origin;
        LVector Direction;

        FRay(const LVector& InOrigin, const LVector& InDirection)
			: Origin(InOrigin)
			, Direction(InDirection)
        {
        }

        FRay(const glm::vec3& InOrigin, const glm::vec3& InDirection)
			: Origin(InOrigin)
			, Direction(InDirection)
        {
        }

        /**
		 * @brief Get a zero-initialized ray.
         */
        static FRay Zero()
        {
            return { LVector(0.0f, 0.0f, 0.0f), LVector(0.0f, 0.0f, 0.0f) };
        }

		/**
		 * @brief Checks if a ray intersects an axis-aligned bounding box (AABB) in 3D space.
		 * 
		 * @details The distance from the ray's origin to the nearest intersection point is set in 't', 
		 *          if any intersection is found.
		 * @return true if an intersection occured, else false.
		 */
        FORCEINLINE bool IntersectsAABB(const FAABB& AABB, float& t) const
        {
			/* The direction vector is inverted to calculate the intersection points. */
			LVector DirFraction;
            DirFraction.X = (1.0f / Direction.X);
            DirFraction.Y = (1.0f / Direction.Y);
            DirFraction.Z = (1.0f / Direction.Z);

            const LVector& LeftBottom = AABB.Min;
            const LVector& RightTop = AABB.Max;

            const float t1 = (LeftBottom.X - Origin.X) * DirFraction.X;
            const float t2 = (RightTop.X - Origin.X) * DirFraction.X;

            const float t3 = (LeftBottom.Y - Origin.Y) * DirFraction.Y;
            const float t4 = (RightTop.Y - Origin.Y) * DirFraction.Y;

            const float t5 = (LeftBottom.Z - Origin.Z) * DirFraction.Z;
            const float t6 = (RightTop.Z - Origin.Z) * DirFraction.Z;

			/* Point where the ray first enters the AABB. */
            const float tmin = glm::max(
				glm::max(glm::min(t1, t2), glm::min(t3, t4)), 
				glm::min(t5, t6)
			);

			/* Point where the ray exits the AABB. */
            const float tmax = glm::min(
				glm::min(glm::max(t1, t2), glm::max(t3, t4)), 
				glm::max(t5, t6)
			);

			/* Ray is intersecting with the AABB behind us. */
            if (tmax < 0)
            {
                t = tmax;
                return false;
            }

			/* Ray does not intersect the AABB. */
            if (tmin > tmax)
            {
                t = tmax;
                return false;
            }

			/* When t == 0, t is at the ray's origin. */

            t = tmin;
            return true;
        }

		/**
		 * @brief Checks if a ray intersects with a triangle in 3D space.
		 * 
		 * @details The distance from the ray's origin to the intersection point 
		 *          is set in 't' if an intersection occurs.
		 * @return true if an intersection occured, else false.
		 */
		FORCEINLINE bool IntersectsTriangle(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, float& t) const
		{
			const glm::vec3 E1 = B - A;
			const glm::vec3 E2 = C - A;

			const glm::vec3 N = cross(E1, E2);
			const float Det = -glm::dot(glm::vec3(Direction.X, Direction.Y, Direction.Z), N);

			const float InvDet = 1.0f / Det;

			const glm::vec3 AO = Origin.As<glm::vec3>() - A;
			const glm::vec3 DAO = glm::cross(AO, Direction.As<glm::vec3>());
			const float u = glm::dot(E2, DAO) * InvDet;
			const float v = -glm::dot(E1, DAO) * InvDet;

			t = glm::dot(AO, N) * InvDet;

			return ((Det >= 1e-6f) 
					&& (t >= 0.0f) 
					&& (u >= 0.0f) 
					&& (v >= 0.0f) 
					&& ((u + v) <= 1.0f));
		}

		/**
		 * @brief Checks if a ray intersects with a triangle in 3D space.
		 * 
		 * @details The distance from the ray's origin to the intersection point 
		 *          is set in 't' if an intersection occurs.
		 * @return true if an intersection occured, else false.
		 */
		FORCEINLINE bool IntersectsTriangle(const LVector& A, const LVector& B, const LVector& C, float& t) const
		{
			const LVector E1 = B - A;
			const LVector E2 = C - A;

			const LVector N = LVector::Cross(E1, E2);
			const float Det = -LVector::Dot(Direction, N);

			const float InvDet = 1.0f / Det;

			const LVector AO = Origin - LVector(A);
			const LVector DAO = LVector::Cross(AO, Direction);
			const float u = LVector::Dot(E2, DAO) * InvDet;
			const float v = -LVector::Dot(E1, DAO) * InvDet;

			t = LVector::Dot(AO, N) * InvDet;

			return ((Det >= 1e-6f) 
					&& (t >= 0.0f) 
					&& (u >= 0.0f) 
					&& (v >= 0.0f) 
					&& ((u + v) <= 1.0f));
		}

    };

	/** @} */

}
