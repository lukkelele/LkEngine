#pragma once

#include <type_traits>
#include <ostream>

#include <glm/glm.hpp>
#include <imgui/imgui.h>

#include "LkEngine/Core/CoreMacros.h"


namespace LkEngine {

	template<typename SizeType>
	struct TVector2;

	namespace Math::Internal::Vector2Impl
	{
		/** 
		 * Helper: Dot product.
		 */
		template<typename SizeType, typename VectorTypeA, typename VectorTypeB>
		struct Dot
		{
			static SizeType Calculate(const VectorTypeA& A, const VectorTypeB& B)
			{
				static_assert((sizeof(VectorTypeA) > 0) && (sizeof(VectorTypeB) > 0), "Unsupported vector type");
				return SizeType();
			}
		};

		template<typename SizeType>
		struct Dot<SizeType, TVector2<SizeType>, TVector2<SizeType>>
		{
			static SizeType Calculate(const TVector2<SizeType>& A, const TVector2<SizeType>& B)
			{
				return (A.X * B.X) + (A.Y * B.Y);
			}
		};

		template<typename SizeType>
		struct Dot<SizeType, TVector2<SizeType>, glm::vec3>
		{
			static SizeType Calculate(const TVector2<SizeType>& A, const glm::vec3& B)
			{
				return (A.X * B.x) + (A.Y * B.y);
			}
		};

		template<typename SizeType>
		struct Dot<SizeType, glm::vec3, TVector2<SizeType>>
		{
			static SizeType Calculate(const glm::vec3& A, const TVector2<SizeType>& B)
			{
				return (A.x * B.X) + (A.y * B.Y);
			}
		};


		/** 
		 * Helper: Min.
		 */
		template<typename SizeType, typename VectorTypeA, typename VectorTypeB, typename ReturnType>
		struct Min
		{
			static ReturnType Calculate(const VectorTypeA& A, const VectorTypeB& B)
			{
				static_assert((sizeof(VectorTypeA) > 0) && (sizeof(VectorTypeB) > 0), "Unsupported vector type");
				return SizeType();
			}
		};

		/**
		 * Helper: Min.
		 * A: TVector2<SizeType>
		 * B: TVector2<SizeType>
		 */
		template<typename SizeType>
		struct Min<SizeType, TVector2<SizeType>, TVector2<SizeType>, SizeType>
		{
			static SizeType Calculate(const TVector2<SizeType>& A, const TVector2<SizeType>& B)
			{
				return std::min({
						A.X, A.Y,
						B.X, B.Y
					});
			}
		};

		/**
		 * Helper: Min.
		 * A: TVector2<SizeType>
		 * B: glm::vec3
		 */
		template<typename SizeType>
		struct Min<SizeType, TVector2<SizeType>, glm::vec3, SizeType>
		{
			static SizeType Calculate(const TVector2<SizeType>& A, const glm::vec3& B)
			{
				return std::min({
						A.X, A.Y,
						B.x, B.y
					});
			}
		};


		/**
		 * Helper: Max.
		 */
		template<typename SizeType, typename VectorTypeA, typename VectorTypeB, typename ReturnType>
		struct Max
		{
			static ReturnType Calculate(const VectorTypeA& A, const VectorTypeB& B)
			{
				static_assert((sizeof(VectorTypeA) > 0) && (sizeof(VectorTypeB) > 0), "Unsupported vector type");
				return SizeType();
			}
		};

		/**
		 * Helper: Max.
		 * A: TVector2<SizeType>
		 * B: TVector2<SizeType>
		 */
		template<typename SizeType>
		struct Max<SizeType, TVector2<SizeType>, TVector2<SizeType>, SizeType>
		{
			static SizeType Calculate(const TVector2<SizeType>& A, const TVector2<SizeType>& B)
			{
				return std::max({
						A.X, A.Y,
						B.X, B.Y
					});
			}
		};

		/**
		 * Helper: Max.
		 * A: TVector2<SizeType>
		 * B: glm::vec3
		 */
		template<typename SizeType>
		struct Max<SizeType, TVector2<SizeType>, glm::vec3, SizeType>
		{
			static SizeType Calculate(const TVector2<SizeType>& A, const glm::vec3& B)
			{
				return std::max({
						A.X, A.Y, A.Z,
						B.x, B.y, B.z
					});
			}
		};

		/** 
		 * Helper: Inverse.
		 */
		template<typename SizeType, typename VectorType, typename ReturnType, bool DivisionByZeroGuard>
		struct Inverse
		{
			static ReturnType Calculate(const VectorType& Vector)
			{
				static_assert((sizeof(VectorType) > 0), "Unsupported vector type");
				return ReturnType();
			}
		};

		/**
		 * Helper: Inverse (SAFE).
		 *  A: TVector2<SizeType>
		 *  B: TVector2<SizeType>
		 * 
		 *  Guarded against division by zero.
		 */
		template<typename SizeType>
		struct Inverse<SizeType, TVector2<SizeType>, TVector2<SizeType>, true>
		{
			static TVector2<SizeType> Calculate(const TVector2<SizeType>& Vector)
			{
				return TVector2<SizeType>(
					(Vector.X != static_cast<SizeType>(0)) ? (static_cast<SizeType>(1) / Vector.X) : static_cast<SizeType>(0),
					(Vector.Y != static_cast<SizeType>(0)) ? (static_cast<SizeType>(1) / Vector.Y) : static_cast<SizeType>(0)
				);
			}
		};

		/**
		 * Helper: Inverse (UNSAFE).
		 *  A: TVector2<SizeType>
		 *  B: TVector2<SizeType>
		 * 
		 *  Not guarded against division by zero.
		 */
		template<typename SizeType>
		struct Inverse<SizeType, TVector2<SizeType>, TVector2<SizeType>, false>
		{
			static TVector2<SizeType> Calculate(const TVector2<SizeType>& Vector)
			{
				return TVector2<SizeType>(
					(static_cast<SizeType>(1) / Vector.X),
					(static_cast<SizeType>(1) / Vector.Y)
				);
			}
		};
	}

	/**
	 * TVector2
	 */
	template<typename SizeType>
	struct TVector2
	{
	public:
		TVector2()
			: X(SizeType())
			, Y(SizeType())
		{
		} 

		TVector2(const SizeType InX, const SizeType InY)
			: X(InX)
			, Y(InY)
		{
		} 

		template<typename OtherType, typename = std::enable_if_t<std::is_convertible_v<OtherType, SizeType>>>
		TVector2(const OtherType InX, const OtherType InY)
			: X(static_cast<SizeType>(InX))
			, Y(static_cast<SizeType>(InY))
		{
		} 

		template<typename OtherType, typename = std::enable_if_t<std::is_convertible_v<OtherType, SizeType>>>
		TVector2(const TVector2<OtherType>& Other)
			: X(static_cast<SizeType>(Other.X))
			, Y(static_cast<SizeType>(Other.Y))
		{
		}

		TVector2(const glm::vec2& InVec)
			: X(InVec.x)
			, Y(InVec.y)
		{
		} 

		TVector2(const ImVec2& InVec)
			: X(InVec.x)
			, Y(InVec.y)
		{
		} 

		TVector2& operator=(const TVector2& Other)
		{
			if (this != &Other)
			{
				X = Other.X;
				Y = Other.Y;
			}
			return *this;
		} 

		TVector2& operator+=(const TVector2& Other)
		{
			X += Other.X;
			Y += Other.Y;
			return *this;
		}

		TVector2& operator-=(const TVector2& Other)
		{
			X -= Other.X;
			Y -= Other.Y;
			return *this;
		}

		TVector2 operator-(const TVector2& Other) const 
		{
			return TVector2((X - Other.X), (Y - Other.Y));
		}

		template<typename OtherType>
		TVector2 operator-(const TVector2<OtherType>& Other) const 
		{
			static_assert(std::is_convertible_v<OtherType, SizeType>, "Narrowing conversion");
			return TVector2((X - static_cast<SizeType>(Other.X)), (Y - static_cast<SizeType>(Other.Y)));
		}

		TVector2 operator+(const TVector2& Other) const 
		{
			return TVector2((X + Other.X), (Y + Other.Y));
		}

		template<typename OtherType>
		TVector2 operator+(const TVector2<OtherType>& Other) const 
		{
			static_assert(std::is_convertible_v<OtherType, SizeType>, "Narrowing conversion");
			return TVector2((X + static_cast<SizeType>(Other.X)), (Y + static_cast<SizeType>(Other.Y)));
		}

		template<typename OtherType>
		TVector2& operator=(const TVector2<OtherType> Other)
		{
			static_assert(std::is_convertible_v<OtherType, SizeType>, "Narrowing conversion");
			X = static_cast<SizeType>(Other.X);
			Y = static_cast<SizeType>(Other.Y);
			return *this;
		} 

		bool operator==(const TVector2& Other) const 
		{
			return ((X == Other.X) && (Y == Other.Y));
		}

		bool operator!=(const TVector2& Other) const 
		{
			return !(*this == Other);
		}

		template<typename OtherType>
		bool operator==(const TVector2<OtherType>& Other) const 
		{
			return ((X == static_cast<SizeType>(Other.X)) && (Y == static_cast<SizeType>(Other.Y)));
		}

		FORCEINLINE bool IsNull() const
		{
			return ((X == 0) && (Y == 0));
		}

		static float Distance(const TVector2& A, const TVector2& B)
		{
			return 0.0f;
		}

		template<typename VectorTypeA, typename VectorTypeB>
	    static SizeType Min(const VectorTypeA& A, const VectorTypeB& B)
		{
			using ReturnType = SizeType;
			return Math::Internal::Vector2Impl::Min<SizeType, VectorTypeA, VectorTypeB, ReturnType>::Calculate(A, B);
		}

		template<typename VectorTypeA, typename VectorTypeB>
	    static SizeType Max(const VectorTypeA& A, const VectorTypeB& B)
		{
			using ReturnType = SizeType;
			return Math::Internal::Vector2Impl::Max<SizeType, VectorTypeA, VectorTypeB, ReturnType>::Calculate(A, B);
		}

		template<typename VectorType>
		static VectorType Inverse(const VectorType& Vector)
		{
			return Math::Internal::Vector2Impl::Inverse<SizeType, VectorType, VectorType, false>::Calculate(Vector);
		}

		template<typename VectorType>
		static VectorType InverseSafe(const VectorType& Vector)
		{
			return Math::Internal::Vector2Impl::Inverse<SizeType, VectorType, VectorType, true>::Calculate(Vector);
		}

		template<typename StringType = std::string>
		FORCEINLINE StringType ToString() const
		{
			if constexpr (std::is_same_v<StringType, std::string>)
			{
				if constexpr (std::is_floating_point_v<SizeType>)
				{
					return std::format("({:.2f}, {:.2f})", X, Y);
				}
				else
				{
					return std::format("({}, {})", X, Y);
				}
			}
			/* TODO: This does not work as it should, should be removed. */
			else if constexpr (std::is_same_v<StringType, const char*>)
			{
				static constexpr uint16_t BufSize = 256;
				if constexpr (std::is_floating_point_v<SizeType>)
				{
					static std::array<char, BufSize> Buffer;
					std::snprintf(Buffer.data(), Buffer.size(), "(%.2f, %.2f)", X, Y);
					return Buffer.data();
				}
				else
				{
					static std::array<char, BufSize> Buffer;
					std::snprintf(Buffer.data(), Buffer.size(), "(%d, %d)", X, Y);
					return Buffer.data();
				}
			}
		}

		friend std::ostream& operator<<(std::ostream& os, const TVector2& Vector) 
		{
			os << Vector.ToString();
			return os;
		}

		template<typename VectorType>
		FORCEINLINE VectorType As() const
		{
			return VectorType(X, Y);
		}

		template<>
		FORCEINLINE glm::vec2 As() const
		{
			return glm::vec2(X, Y);
		}

		template<>
		FORCEINLINE ImVec2 As() const
		{
			return ImVec2(X, Y);
		}

		operator glm::vec2() { return glm::vec2(X, Y); }
		operator ImVec2() { return ImVec2(X, Y); }

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
		>, "TVector2 can only be instantiated with int, float or double types");

	public:
	#if LK_VECTOR_ANONYMOUS_STRUCT
		union
		{
			struct { SizeType X, Y; };
			struct { SizeType R, G; };
			struct { SizeType S, T; };
		};
	#else
		union { SizeType X, R, S; };
		union { SizeType Y, G, T; };
	#endif
	};



}
