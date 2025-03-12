#pragma once

#include <type_traits>
#include <ostream>

#include <glm/glm.hpp>

#include "LkEngine/Core/CoreMacros.h"


namespace LkEngine {

	template<typename SizeType>
	struct TVector3;

	namespace Math::Internal::Vector3Impl
	{
		/** 
		 * Helper: Min
		 */
		template<typename SizeType, typename VectorTypeA, typename VectorTypeB, typename ReturnType>
		struct Min
		{
			FORCEINLINE static ReturnType Calculate(const VectorTypeA& A, const VectorTypeB& B)
			{
				static_assert((sizeof(VectorTypeA) > 0) && (sizeof(VectorTypeB) > 0), "Unsupported vector type");
				return ReturnType();
			}
		};

		/**
		 * Helper: Min
		 * A: TVector3<SizeType>
		 * B: TVector3<SizeType>
		 * ReturnType: TVector3<SizeType>
		 */
		template<typename SizeType>
		struct Min<SizeType, TVector3<SizeType> /*== A*/, TVector3<SizeType> /*== B*/, TVector3<SizeType> /*== ReturnType*/>
		{
			FORCEINLINE static TVector3<SizeType> Calculate(const TVector3<SizeType>& A, const TVector3<SizeType>& B)
			{
				return TVector3<SizeType>(std::min(A.X, B.X), 
										  std::min(A.Y, B.Y), 
										  std::min(A.Z, B.Z));
			}
		};

		/**
		 * Helper: Min
		 * A: TVector3<SizeType>
		 * B: glm::vec3
		 * ReturnType: TVector3<SizeType>
		 */
		template<typename SizeType>
		struct Min<SizeType, TVector3<SizeType> /*== A*/, glm::vec3 /*== B*/, TVector3<SizeType> /*== ReturnType*/>
		{
			FORCEINLINE static TVector3<SizeType> Calculate(const TVector3<SizeType>& A, const glm::vec3& B)
			{
				return TVector3<SizeType>(std::min(A.X, B.x), 
										  std::min(A.Y, B.y), 
										  std::min(A.Z, B.z));
			}
		};

		/**
		 * Helper: Min
		 * A: TVector3<SizeType>
		 * B: glm::vec3
		 * ReturnType: SizeType
		 */
		template<typename SizeType>
		struct Min<SizeType, TVector3<SizeType> /*== A*/, glm::vec3 /*== B*/, SizeType /*== ReturnType*/>
		{
			FORCEINLINE static SizeType Calculate(const TVector3<SizeType>& A, const glm::vec3& B)
			{
				return std::min({
						A.X, A.Y, A.Z,
						B.x, B.y, B.z
					});
			}
		};

		/**
		 * Helper: Min
		 * A: glm::vec3
		 * B: TVector3<SizeType>
		 * ReturnType: TVector3<SizeType>
		 */
		template<typename SizeType>
		struct Min<SizeType, glm::vec3 /*== A*/, TVector3<SizeType> /*== B*/, TVector3<SizeType> /*== ReturnType*/>
		{
			FORCEINLINE static TVector3<SizeType> Calculate(const glm::vec3& A, const TVector3<SizeType>& B) 
			{
				return TVector3<SizeType>(std::min(A.x, B.X), 
										  std::min(A.y, B.Y), 
										  std::min(A.z, B.Z));
			}
		};

		/**
		 * Helper: Min
		 * A: glm::vec3
		 * B: glm::vec3
		 * ReturnType: TVector3<SizeType>
		 */
		template<typename SizeType>
		struct Min<SizeType, glm::vec3 /*== A*/, glm::vec3 /*== B*/, TVector3<SizeType> /*== ReturnType*/>
		{
			FORCEINLINE static TVector3<SizeType> Calculate(const glm::vec3& A, const glm::vec3& B) 
			{
				return TVector3<SizeType>(std::min(A.x, B.x), 
										  std::min(A.y, B.y), 
										  std::min(A.z, B.z));
			}
		};


		/**
		 * Helper: Max
		 */
		template<typename SizeType, typename VectorTypeA, typename VectorTypeB, typename ReturnType>
		struct Max
		{
			FORCEINLINE static ReturnType Calculate(const VectorTypeA& A, const VectorTypeB& B)
			{
				static_assert((sizeof(VectorTypeA) > 0) && (sizeof(VectorTypeB) > 0), "Unsupported vector type");
				return ReturnType();
			}
		};

		/**
		 * Helper: Max
		 * A: TVector3<SizeType>
		 * B: TVector3<SizeType>
		 * ReturnType: TVector3<SizeType>
		 */
		template<typename SizeType>
		struct Max<SizeType, TVector3<SizeType> /*== A*/, TVector3<SizeType> /*== B*/, TVector3<SizeType> /*== ReturnType*/>
		{
			FORCEINLINE static TVector3<SizeType> Calculate(const TVector3<SizeType>& A, const TVector3<SizeType>& B)
			{
				return TVector3<SizeType>(std::max(A.X, B.X), 
										  std::max(A.Y, B.Y), 
										  std::max(A.Z, B.Z));
			}
		};

		/**
		 * Helper: Max
		 * A: TVector3<SizeType>
		 * B: glm::vec3
		 * ReturnType: TVector3<SizeType>
		 */
		template<typename SizeType>
		struct Max<SizeType, TVector3<SizeType> /*== A*/, glm::vec3 /*== B*/, TVector3<SizeType> /*== ReturnType*/>
		{
			FORCEINLINE static TVector3<SizeType> Calculate(const TVector3<SizeType>& A, glm::vec3& B)
			{
				return TVector3<SizeType>(
					std::max(A.X, B.x),
					std::max(A.Y, B.y),
					std::max(A.Z, B.z)
				);
			}
		};

		/**
		 * Helper: Max
		 * A: glm::vec3
		 * B: TVector3<SizeType>
		 * ReturnType: TVector3<SizeType>
		 */
		template<typename SizeType>
		struct Max<SizeType, glm::vec3 /*== A*/, TVector3<SizeType> /*== B*/, TVector3<SizeType> /*== ReturnType*/>
		{
			FORCEINLINE static TVector3<SizeType> Calculate(const glm::vec3& A, const TVector3<SizeType>& B)
			{
				return TVector3<SizeType>(std::max(A.x, B.X), 
										  std::max(A.y, B.Y), 
										  std::max(A.z, B.Z));
			}
		};

		/**
		 * Helper: Max
		 * A: glm::vec3
		 * B: glm::vec3
		 * ReturnType: TVector3<SizeType>
		 */
		template<typename SizeType>
		struct Max<SizeType, glm::vec3 /*== A*/, glm::vec3 /*== B*/, TVector3<SizeType> /*== ReturnType*/>
		{
			FORCEINLINE static TVector3<SizeType> Calculate(const glm::vec3& A, const glm::vec3& B)
			{
				return TVector3<SizeType>(std::max(A.x, B.x), 
										  std::max(A.y, B.y),
										  std::max(A.z, B.z));
			}
		};


		/** 
		 * Helper: Dot
		 */
		template<typename SizeType, typename VectorTypeA, typename VectorTypeB, typename ReturnType>
		struct Dot
		{
			static_assert(!std::conjunction_v<
							std::is_same<ReturnType, VectorTypeA>, 
							std::is_same<ReturnType, VectorTypeB> 
						  >, "Dot failed, invalid return type");
			FORCEINLINE static ReturnType Calculate(const VectorTypeA& A, const VectorTypeB& B)
			{
				static_assert((sizeof(VectorTypeA) > 0) && (sizeof(VectorTypeB) > 0), "Unsupported vector type");
				return ReturnType();
			}
		};

		/**
		 * Helper: Dot
		 * A: TVector3<SizeType>
		 * B: TVector3<SizeType>
		 */
		template<typename SizeType>
		struct Dot<SizeType, TVector3<SizeType> /*== A*/, TVector3<SizeType> /*== B*/, SizeType /*== ReturnType*/>
		{
			FORCEINLINE static SizeType Calculate(const TVector3<SizeType>& A, const TVector3<SizeType>& B)
			{
				return (A.X * B.X) + (A.Y * B.Y) + (A.Z * B.Z);
			}
		};

		/**
		 * Helper: Dot
		 * A: TVector3<SizeType>
		 * B: glm::vec3
		 */
		template<typename SizeType>
		struct Dot<SizeType, TVector3<SizeType> /*== A*/, glm::vec3 /*== B*/, SizeType /*== ReturnType*/>
		{
			FORCEINLINE static SizeType Calculate(const TVector3<SizeType>& A, const glm::vec3& B)
			{
				return (A.X * B.x) + (A.Y * B.y) + (A.Z * B.z);
			}
		};

		/**
		 * Helper: Dot
		 * A: glm::vec3
		 * B: TVector3<SizeType>
		 */
		template<typename SizeType>
		struct Dot<SizeType, glm::vec3 /*== A*/, TVector3<SizeType> /*== B*/, SizeType /*== ReturnType*/>
		{
			FORCEINLINE static SizeType Calculate(const glm::vec3& A, const TVector3<SizeType>& B)
			{
				return (A.x * B.X) + (A.y * B.Y) + (A.z * B.Z);
			}
		};

		/**
		 * Helper: Dot
		 * A: glm::vec3
		 * B: glm::vec3
		 */
		template<typename SizeType>
		struct Dot<SizeType, glm::vec3 /*== A*/, glm::vec3 /*== B*/, SizeType /*== ReturnType*/>
		{
			FORCEINLINE static SizeType Calculate(const glm::vec3& A, const glm::vec3& B)
			{
				return (A.x * B.x) + (A.y * B.y) + (A.z * B.z);
			}
		};


		/** 
		 * Helper: Inverse
		 */
		template<typename SizeType, typename VectorType, typename ReturnType, bool DivisionByZeroGuard>
		struct Inverse
		{
			FORCEINLINE static ReturnType Calculate(const VectorType& Vector)
			{
				static_assert((sizeof(VectorType) > 0), "Unsupported vector type");
				return ReturnType();
			}
		};

		/**
		 * Helper: Inverse (SAFE)
		 *  A: TVector3<SizeType>
		 *  B: TVector3<SizeType>
		 * 
		 *  Guarded against division by zero.
		 */
		template<typename SizeType>
		struct Inverse<SizeType, TVector3<SizeType> /*== Target*/, TVector3<SizeType> /*== ReturnType*/, true /*== Safe*/>
		{
			FORCEINLINE static TVector3<SizeType> Calculate(const TVector3<SizeType>& Vector)
			{
				return TVector3<SizeType>(
					(Vector.X != static_cast<SizeType>(0)) ? (static_cast<SizeType>(1) / Vector.X) : static_cast<SizeType>(0),
					(Vector.Y != static_cast<SizeType>(0)) ? (static_cast<SizeType>(1) / Vector.Y) : static_cast<SizeType>(0),
					(Vector.Z != static_cast<SizeType>(0)) ? (static_cast<SizeType>(1) / Vector.Z) : static_cast<SizeType>(0)
				);
			}
		};

		/**
		 * Helper: Inverse (UNSAFE)
		 *  A: TVector3<SizeType>
		 *  B: TVector3<SizeType>
		 * 
		 *  Not guarded against division by zero.
		 */
		template<typename SizeType>
		struct Inverse<SizeType, TVector3<SizeType> /*== Target*/, TVector3<SizeType> /*== ReturnType*/, false /*== Safe*/>
		{
			FORCEINLINE static TVector3<SizeType> Calculate(const TVector3<SizeType>& Vector)
			{
				return TVector3<SizeType>(
					(static_cast<SizeType>(1) / Vector.X),
					(static_cast<SizeType>(1) / Vector.Y),
					(static_cast<SizeType>(1) / Vector.Z)
				);
			}
		};

		/**
		 * Helper: Normalize
		 */
		template<typename SizeType, typename VectorType, typename ReturnType, bool Safe>
		struct Normalize
		{
			FORCEINLINE static ReturnType Calculate(const VectorType& Vector)
			{
				static_assert((sizeof(VectorType) > 0), "Unsupported vector type");
				return ReturnType();
			}
		};

		/**
		 * Helper: Normalize (UNSAFE)
		 * Target: LVector
		 *
		 *  Not safeguarded. Assumes the magnitude is greater than 0.
		 */
		template<typename SizeType>
		struct Normalize<SizeType, TVector3<SizeType> /*== Target*/, TVector3<SizeType> /*== ReturnType*/, false /*== Safe*/>
		{
			FORCEINLINE static TVector3<SizeType> Calculate(const TVector3<SizeType>& Vector)
			{
				const SizeType Magnitude = std::sqrt((Vector.X * Vector.X) + (Vector.Y * Vector.Y) + (Vector.Z * Vector.Z));
				return TVector3<SizeType>(Vector / Magnitude);
			}
		};

		/**
		 * Helper: Normalize (UNSAFE)
		 * Target: glm::vec3
		 *
		 *  Not safeguarded. Assumes the magnitude is greater than 0.
		 */
		template<typename SizeType>
		struct Normalize<SizeType, glm::vec3 /*== Target*/, TVector3<SizeType> /*== ReturnType*/, false /*== Safe*/>
		{
			FORCEINLINE static TVector3<SizeType> Calculate(const glm::vec3& Vector)
			{
				const SizeType Magnitude = std::sqrt((Vector.x * Vector.x) + (Vector.y * Vector.y) + (Vector.z * Vector.z));
				return TVector3<SizeType>(Vector / Magnitude);
			}
		};

		/**
		 * Helper: Normalize (SAFE)
		 * Target: LVector
		 * 
		 *  Safeguarded, makes sure the magnitude is greater than 0 before dividing.
		 */
		template<typename SizeType>
		struct Normalize<SizeType, TVector3<SizeType> /*== Target*/, TVector3<SizeType> /*== ReturnType*/, true /*== Safe*/>
		{
			FORCEINLINE static TVector3<SizeType> Calculate(const TVector3<SizeType>& Vector)
			{
				const SizeType Magnitude = std::sqrt((Vector.X * Vector.X) + (Vector.Y * Vector.Y) + (Vector.Z * Vector.Z));
				if (Magnitude > static_cast<SizeType>(0))
				{
					return TVector3<SizeType>(Vector / Magnitude);
				}

				return Vector;
			}
		};

		/**
		 * Helper: Normalize (SAFE)
		 * Target: glm::vec3
		 *
		 *  Not safeguarded. Assumes the magnitude is greater than 0.
		 */
		template<typename SizeType>
		struct Normalize<SizeType, glm::vec3 /*== Target*/, TVector3<SizeType> /*== ReturnType*/, true /*== Safe*/>
		{
			FORCEINLINE static TVector3<SizeType> Calculate(const glm::vec3& Vector)
			{
				const SizeType Magnitude = std::sqrt((Vector.x * Vector.x) + (Vector.y * Vector.y) + (Vector.z * Vector.z));
				if (Magnitude > static_cast<SizeType>(0))
				{
					return TVector3<SizeType>(Vector / Magnitude);
				}

				return TVector3<SizeType>(Vector);
			}
		};

		
		/**
		 * Helper: Cross
		 */
		template<typename SizeType, typename VectorTypeA, typename VectorTypeB, typename ReturnType = TVector3<SizeType>>
		struct Cross
		{
			FORCEINLINE static ReturnType Calculate(const TVector3<SizeType>& A, const TVector3<SizeType>& B)
			{
				static_assert((sizeof(VectorTypeA) > 0) && (sizeof(VectorTypeB) > 0), "Unsupported vector type");
				return ReturnType();
			}
		};

		/**
		 * Helper: Cross
		 *  A: TVector3<SizeType>
		 *  B: TVector3<SizeType>
		 */
		template<typename SizeType>
		struct Cross<SizeType, TVector3<SizeType> /*== A*/, TVector3<SizeType> /*== B*/, TVector3<SizeType> /*== ReturnType*/>
		{
			FORCEINLINE static TVector3<SizeType> Calculate(const TVector3<SizeType>& A, const TVector3<SizeType>& B)
			{
				return TVector3<SizeType>((A.Y * B.Z - A.Z * B.Y),  /* X */ 
										  (A.Z * B.X - A.X * B.Z),  /* Y */ 
										  (A.X * B.Y - A.Y * B.X)); /* Z */
			}
		};

		/**
		 * Helper: Cross
		 *  A: TVector3<SizeType>
		 *  B: glm::vec3
		 */
		template<typename SizeType>
		struct Cross<SizeType, TVector3<SizeType> /*== A*/, glm::vec3 /*== B*/, TVector3<SizeType> /*== ReturnType*/>
		{
			FORCEINLINE static TVector3<SizeType> Calculate(const TVector3<SizeType>& A, const glm::vec3& B)
			{
				return TVector3<SizeType>(
					(A.Y * B.z - A.Z * B.y), /* X */
					(A.Z * B.x - A.X * B.z), /* Y */
					(A.X * B.y - A.Y * B.x)  /* Z */
				);
			}
		};

		/**
		 * Helper: Cross
		 *  A: glm::vec3
		 *  B: TVector3<SizeType>
		 */
		template<typename SizeType>
		struct Cross<SizeType, glm::vec3 /*== A*/, TVector3<SizeType> /*== B*/, TVector3<SizeType> /*== ReturnType*/>
		{
			FORCEINLINE static TVector3<SizeType> Calculate(const glm::vec3& A, const TVector3<SizeType>& B)
			{
				return TVector3<SizeType>(
					(A.y * B.Z - A.z * B.Y), /* X */
					(A.z * B.X - A.x * B.Z), /* Y */
					(A.x * B.Y - A.y * B.X)  /* Z */
				);
			}
		};

		/**
		 * Helper: Cross
		 *  A: glm::vec3
		 *  B: glm::vec3
		 */
		template<typename SizeType>
		struct Cross<SizeType, glm::vec3 /*== A*/, glm::vec3 /*== B*/, TVector3<SizeType> /*== ReturnType*/>
		{
			FORCEINLINE static TVector3<SizeType> Calculate(const glm::vec3& A, const glm::vec3& B)
			{
				return TVector3<SizeType>((A.y * B.z - A.z * B.y),  /* X */
										  (A.z * B.x - A.x * B.z),  /* Y */
										  (A.x * B.y - A.y * B.x)); /* Z */
			}
		};


		/**
		 * Helper: Distance
		 */
		template<typename SizeType, typename VectorTypeA, typename VectorTypeB, typename ReturnType = SizeType>
		struct Distance
		{
			//FORCEINLINE static ReturnType Calculate(const TVector3<SizeType>& A, const TVector3<SizeType>& B)
			FORCEINLINE static ReturnType Calculate(const VectorTypeA& A, const VectorTypeB& B)
			{
				static_assert((sizeof(VectorTypeA) > 0) && (sizeof(VectorTypeB) > 0), "Unsupported vector type");
				return ReturnType();
			}
		};

		/**
		 * Helper: Distance
		 *  A: TVector3<SizeType>
		 *  B: TVector3<SizeType>
		 */
		template<typename SizeType>
		struct Distance<SizeType, TVector3<SizeType> /*== A*/, TVector3<SizeType> /*== B*/, SizeType /*== ReturnType*/>
		{
			FORCEINLINE static SizeType Calculate(const TVector3<SizeType>& A, const TVector3<SizeType>& B)
			{
				const float DX = B.X - A.X;
				const float DY = B.Y - A.Y;
				const float DZ = B.Z - A.Z;
				return std::sqrt((DX * DX) + (DY * DY) + (DZ * DZ));
			}
		};

		/**
		 * Helper: Distance
		 *  A: TVector3<SizeType>
		 *  B: glm::vec3
		 */
		template<typename SizeType>
		struct Distance<SizeType, TVector3<SizeType> /*== A*/, glm::vec3 /*== B*/, SizeType /*== ReturnType*/>
		{
			FORCEINLINE static SizeType Calculate(const TVector3<SizeType>& A, const glm::vec3& B)
			{
				const float DX = B.x - A.X;
				const float DY = B.y - A.Y;
				const float DZ = B.z - A.Z;
				return std::sqrt((DX * DX) + (DY * DY) + (DZ * DZ));
			}
		};

		/**
		 * Helper: Distance
		 *  A: glm::vec3
		 *  B: TVector3<SizeType>
		 */
		template<typename SizeType>
		struct Distance<SizeType, glm::vec3 /*== A*/, TVector3<SizeType> /*== B*/, SizeType /*== ReturnType*/>
		{
			FORCEINLINE static SizeType Calculate(const glm::vec3& A, const TVector3<SizeType>& B)
			{
				const float DX = B.X - A.x;
				const float DY = B.Y - A.y;
				const float DZ = B.Z - A.z;
				return std::sqrt((DX * DX) + (DY * DY) + (DZ * DZ));
			}
		};

		/**
		 * Helper: Distance
		 *  A: glm::vec3
		 *  B: glm::vec3
		 */
		template<typename SizeType>
		struct Distance<SizeType, glm::vec3 /*== A*/, glm::vec3 /*== B*/, SizeType /*== ReturnType*/>
		{
			FORCEINLINE static SizeType Calculate(const glm::vec3& A, const glm::vec3& B)
			{
				const float DX = B.x - A.x;
				const float DY = B.y - A.y;
				const float DZ = B.z - A.z;
				return std::sqrt((DX * DX) + (DY * DY) + (DZ * DZ));
			}
		};


		/**
		 * Helper: DistanceSquared
		 */
		template<typename SizeType, typename VectorTypeA, typename VectorTypeB, typename ReturnType = SizeType>
		struct DistanceSquared
		{
			FORCEINLINE static ReturnType Calculate(const VectorTypeA& A, const VectorTypeB& B)
			{
				static_assert((sizeof(VectorTypeA) > 0) && (sizeof(VectorTypeB) > 0), "Unsupported vector type");
				return ReturnType();
			}
		};

		/**
		 * Helper: DistanceSquared
		 *  A: TVector3<SizeType>
		 *  B: TVector3<SizeType>
		 */
		template<typename SizeType>
		struct DistanceSquared<SizeType, TVector3<SizeType> /*== A*/, TVector3<SizeType> /*== B*/, SizeType /*== ReturnType*/>
		{
			FORCEINLINE static SizeType Calculate(const TVector3<SizeType>& A, const TVector3<SizeType>& B)
			{
				const float DX = B.X - A.X;
				const float DY = B.Y - A.Y;
				const float DZ = B.Z - A.Z;
				return ((DX * DX) + (DY * DY) + (DZ * DZ));
			}
		};

		/**
		 * Helper: DistanceSquared
		 *  A: TVector3<SizeType>
		 *  B: glm::vec3
		 */
		template<typename SizeType>
		struct DistanceSquared<SizeType, TVector3<SizeType> /*== A*/, glm::vec3 /*== B*/, SizeType /*== ReturnType*/>
		{
			FORCEINLINE static SizeType Calculate(const TVector3<SizeType>& A, const glm::vec3& B)
			{
				const float DX = B.x - A.X;
				const float DY = B.y - A.Y;
				const float DZ = B.z - A.Z;
				return ((DX * DX) + (DY * DY) + (DZ * DZ));
			}
		};

		/**
		 * Helper: DistanceSquared
		 *  A: glm::vec3
		 *  B: TVector3<SizeType>
		 */
		template<typename SizeType>
		struct DistanceSquared<SizeType, glm::vec3 /*== A*/, TVector3<SizeType> /*== B*/, SizeType /*== ReturnType*/>
		{
			FORCEINLINE static SizeType Calculate(const glm::vec3& A, const TVector3<SizeType>& B)
			{
				const float DX = B.X - A.x;
				const float DY = B.Y - A.y;
				const float DZ = B.Z - A.z;
				return ((DX * DX) + (DY * DY) + (DZ * DZ));
			}
		};

		/**
		 * Helper: DistanceSquared
		 *  A: glm::vec3
		 *  B: glm::vec3
		 */
		template<typename SizeType>
		struct DistanceSquared<SizeType, glm::vec3 /*== A*/, glm::vec3 /*== B*/, SizeType /*== ReturnType*/>
		{
			FORCEINLINE static SizeType Calculate(const glm::vec3& A, const glm::vec3& B)
			{
				const float DX = B.x - A.x;
				const float DY = B.y - A.y;
				const float DZ = B.z - A.z;
				return ((DX * DX) + (DY * DY) + (DZ * DZ));
			}
		};

	}

	/**
	 * TVector3
	 */
	template<typename SizeType>
	struct TVector3
	{
	public:
		TVector3()
			: X(SizeType())
			, Y(SizeType())
			, Z(SizeType()) 
		{
		} 

		TVector3(const SizeType InX, const SizeType InY, const SizeType InZ)
			: X(InX)
			, Y(InY)
			, Z(InZ) 
		{
		}

		TVector3(const SizeType InXYZ)
			: X(InXYZ)
			, Y(InXYZ)
			, Z(InXYZ) 
		{
		}

	#if LK_MATH_VECTOR3_EXPLICIT_GLM_CONSTRUCTOR
		explicit TVector3(const glm::vec3& InVec)
	#else
		TVector3(const glm::vec3& InVec)
	#endif
			: X(static_cast<SizeType>(InVec.x))
			, Y(static_cast<SizeType>(InVec.y))
			, Z(static_cast<SizeType>(InVec.z)) 
		{
		}

		~TVector3() = default;

	#if LK_MATH_VECTOR3_EXPLICIT_GLM_CONSTRUCTOR
		TVector3& operator=(const TVector3& Other)
		{
			X = static_cast<SizeType>(Other.X);
			Y = static_cast<SizeType>(Other.Y);
			Z = static_cast<SizeType>(Other.Z);
			return *this;
		}

		TVector3& operator=(const glm::vec3& Other)
		{
			X = static_cast<SizeType>(Other.x);
			Y = static_cast<SizeType>(Other.y);
			Z = static_cast<SizeType>(Other.z);
			return *this;
		}
	#endif

		TVector3& operator+=(const TVector3& Other)
		{
			X += Other.X;
			Y += Other.Y;
			Z += Other.Z;
			return *this;
		}

		TVector3& operator+=(const glm::vec3& Other)
		{
			X += Other.x;
			Y += Other.y;
			Z += Other.z;
			return *this;
		}

		TVector3& operator-=(const TVector3& Other)
		{
			X -= Other.X;
			Y -= Other.Y;
			Z -= Other.Z;
			return *this;
		}

		TVector3& operator-=(const glm::vec3& Other)
		{
			X -= Other.x;
			Y -= Other.y;
			Z -= Other.z;
			return *this;
		}

		TVector3 operator+(const TVector3& Other) const 
		{
			return TVector3((X + Other.X), (Y + Other.Y), (Z + Other.Z));
		}

		TVector3 operator+(const glm::vec3& Other) const 
		{
			return TVector3((X + Other.x), (Y + Other.y), (Z + Other.z));
		}

		TVector3 operator-(const TVector3& Other) const 
		{
			return TVector3((X - Other.X), (Y - Other.Y), (Z - Other.Z));
		}

		TVector3 operator-(const glm::vec3& Other) const 
		{
			return TVector3((X - Other.x), (Y - Other.y), (Z - Other.z));
		}

		TVector3 operator/(const float Value) const 
		{
			LK_CORE_ASSERT(Value != 0.0f, "Division by zero");
			return TVector3((X / Value), (Y / Value), (Z / Value));
		}

		TVector3 operator/(const TVector3& Other) const 
		{
			LK_CORE_ASSERT((Other.X != 0.0f) && (Other.Y != 0.0f) && (Other.Z != 0.0f), "Division by zero");
			return TVector3((X / Other.X), (Y / Other.Y), (Z / Other.Z));
		}

		TVector3 operator/(const glm::vec3& Other) const 
		{
			LK_CORE_ASSERT((Other.x != 0.0f) && (Other.y != 0.0f) && (Other.z != 0.0f), "Division by zero");
			return TVector3((X / Other.x), (Y / Other.y), (Z / Other.z));
		}

		TVector3 operator*(const float Value) const 
		{
			/* 
			 * TODO: Might do some warning for multiplication by zero since 
			 *       it is not that usual that outcome is desired. 
			 */
			return TVector3((X * Value), (Y * Value), (Z * Value));
		}

		TVector3 operator*(const TVector3& Other) const 
		{
			return TVector3((X * Other.X), (Y * Other.Y), (Z * Other.Z));
		}

		TVector3 operator*(const glm::vec3& Other) const 
		{
			return TVector3((X * Other.x), (Y * Other.y), (Z * Other.z));
		}

		bool operator==(const TVector3& Other) const 
		{
		#if defined(LK_MATH_VECTOR3_BOOL_OPERATOR_USE_EPSILON)
			static constexpr SizeType Epsilon = static_cast<SizeType>(1e-5);
			return ((std::abs(X - Other.X) < Epsilon) 
					&& (std::abs(Y - Other.Y)) < Epsilon)
					&& (std::abs(Z - Other.Z) < Epsilon));
		#else
			return ((X == Other.X) && (Y == Other.Y) && (Z == Other.Z));
		#endif
		}

		bool operator!=(const TVector3& Other) const 
		{
			return !(*this == Other);
		}

		bool operator==(const glm::vec3& Other) const 
		{
		#if defined(LK_MATH_VECTOR3_BOOL_OPERATOR_USE_EPSILON)
			static constexpr SizeType Epsilon = static_cast<SizeType>(1e-5);
			return ((std::abs(X - Other.X) < Epsilon) 
					&& (std::abs(Y - Other.Y)) < Epsilon)
					&& (std::abs(Z - Other.Z) < Epsilon));
		#else
			return ((X == Other.x) && (Y == Other.y) && (Z == Other.z));
		#endif
		}

		bool operator!=(const glm::vec3& Other) const 
		{
			return !(*this == Other);
		}

		/**
		 * Calculate the dot product.
		 */
		template<typename VectorTypeA, typename VectorTypeB>
		static SizeType Dot(const VectorTypeA& A, const VectorTypeB& B)
		{
			using ReturnType = SizeType;
			return Math::Internal::Vector3Impl::Dot<SizeType, VectorTypeA, VectorTypeB, ReturnType>::Calculate(A, B);
		}

		/**
		 * Calculate the distance between two vectors.
		 */
		template<typename VectorTypeA, typename VectorTypeB>
		static SizeType Distance(const VectorTypeA& A, const VectorTypeB& B)
		{
			using ReturnType = SizeType;
			return Math::Internal::Vector3Impl::Distance<SizeType, VectorTypeA, VectorTypeB, ReturnType>::Calculate(A, B);
		}

		/**
		 * Calculate the squared distance between two vectors.
		 *
		 *  Prefer this over TVector3::Distance if possible (performance).
		 */
		template<typename VectorTypeA, typename VectorTypeB>
		static SizeType DistanceSquared(const VectorTypeA& A, const VectorTypeB& B)
		{
			using ReturnType = SizeType;
			return Math::Internal::Vector3Impl::DistanceSquared<SizeType, VectorTypeA, VectorTypeB, ReturnType>::Calculate(A, B);
		}

		template<typename VectorTypeA, typename VectorTypeB>
	    static TVector3<SizeType> Cross(const VectorTypeA& A, const VectorTypeB& B)
		{
			using ReturnType = TVector3<SizeType>;
			return Math::Internal::Vector3Impl::Cross<SizeType, VectorTypeA, VectorTypeB, ReturnType>::Calculate(A, B);
		}

		FORCEINLINE void Normalize()
		{
			const SizeType Magnitude = std::sqrt((X * X) + (Y * Y) + (Z * Z));
			X /= Magnitude;
			Y /= Magnitude;
			Z /= Magnitude;
		}

		FORCEINLINE void NormalizeSafe()
		{
			const SizeType Magnitude = std::sqrt((X * X) + (Y * Y) + (Z * Z));
			if (Magnitude > static_cast<SizeType>(0))
			{
				X /= Magnitude;
				Y /= Magnitude;
				Z /= Magnitude;
			}
			else
			{
				LK_CORE_WARN_TAG("Vector3", "Vector could not be normalized with magnitude: {}", Magnitude);
			}
		}

		template<typename VectorType>
	    static TVector3<SizeType> Normalize(const VectorType& Vector)
		{
			using ReturnType = TVector3<SizeType>;
			return Math::Internal::Vector3Impl::Normalize<SizeType, VectorType, ReturnType, false>::Calculate(Vector);
		}

		template<typename VectorType>
	    static TVector3<SizeType> NormalizeSafe(const VectorType& Vector)
		{
			using ReturnType = TVector3<SizeType>;
			return Math::Internal::Vector3Impl::Normalize<SizeType, VectorType, ReturnType, true>::Calculate(Vector);
		}

		template<typename VectorTypeA, typename VectorTypeB, typename ReturnType = TVector3<SizeType>>
	    static ReturnType Min(const VectorTypeA& A, const VectorTypeB& B)
		{
			return Math::Internal::Vector3Impl::Min<SizeType, VectorTypeA, VectorTypeB, ReturnType>::Calculate(A, B);
		}

		template<typename VectorTypeA, typename VectorTypeB, typename ReturnType = TVector3<SizeType>>
	    static ReturnType Max(const VectorTypeA& A, const VectorTypeB& B)
		{
			return Math::Internal::Vector3Impl::Max<SizeType, VectorTypeA, VectorTypeB, ReturnType>::Calculate(A, B);
		}

		template<typename VectorType>
		static VectorType Inverse(const VectorType& Vector)
		{
			return Math::Internal::Vector3Impl::Inverse<SizeType, VectorType, VectorType, false>::Calculate(Vector);
		}

		template<typename VectorType>
		static VectorType InverseSafe(const VectorType& Vector)
		{
			return Math::Internal::Vector3Impl::Inverse<SizeType, VectorType, VectorType, true>::Calculate(Vector);
		}

		FORCEINLINE std::string ToString() const
		{
			return LK_FMT_LIB::format("({:.2f}, {:.2f}, {:.2f})", X, Y, Z);
		}

		FORCEINLINE friend std::ostream& operator<<(std::ostream& os, const TVector3& Vector) 
		{
			os << *Vector.ToString();
			return os;
		}

	#if 0
		template<typename VectorType>
		VectorType As() const
		{
			return VectorType(X, Y, Z);
		}
	#else
		template<typename VectorType, typename = std::enable_if_t<std::is_same_v<VectorType, glm::vec3>>>
		VectorType As() const
		{
			return glm::vec3(X, Y, Z);
		}
	#endif

		static_assert(std::disjunction_v<
			std::is_same<SizeType, int>, 
			std::is_same<SizeType, uint16_t>, 
			std::is_same<SizeType, uint32_t>, 
			std::is_same<SizeType, uint64_t>, 
			std::is_same<SizeType, int16_t>, 
			std::is_same<SizeType, int32_t>, 
			std::is_same<SizeType, int64_t>, 
			std::is_same<SizeType, float>, 
			std::is_same<SizeType, double>
		>, "TVector3 can only be instantiated with int, float or double types");

	public:
	#if LK_MATH_VECTOR_ANONYMOUS_STRUCT
		union
		{
			struct { SizeType X, Y, Z; };
			struct { SizeType R, G, B; };
			struct { SizeType S, T, P; };
		};
	#else
		union { SizeType X, R, S; };
		union { SizeType Y, G, T; };
		union { SizeType Z, B, P; };
	#endif
	};

#if 0
	template<typename SizeType>
	template<>
	inline glm::vec3 TVector3<SizeType>::As<glm::vec3>() const
	{
		return glm::vec3(X, Y, Z);
	}
#endif

}
