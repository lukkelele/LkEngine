#pragma once

#include <type_traits>
#include <ostream>

#include <glm/glm.hpp>

#include "LkEngine/Core/CoreMacros.h"


namespace LkEngine {

	template<typename SizeType>
	struct TVector4;

	namespace Math::Internal::Vector4Impl 
	{
		
	}

	/**
	 * TVector4
	 */
	template<typename SizeType>
	struct TVector4
	{
	public:
		TVector4()
			: X(SizeType())
			, Y(SizeType())
			, Z(SizeType()) 
			, W(SizeType()) 
		{
		} 

		TVector4(SizeType InX, SizeType InY, SizeType InZ, SizeType InW)
			: X(InX)
			, Y(InY)
			, Z(InZ) 
			, W(InW)
		{
		}

		TVector4(SizeType InXYZW)
			: X(InXYZW)
			, Y(InXYZW)
			, Z(InXYZW) 
			, W(InXYZW)
		{
		}

		TVector4(const TVector3<SizeType>& InVec3, SizeType InW)
			: X(InVec3.X)
			, Y(InVec3.Y)
			, Z(InVec3.Z) 
			, W(InW)
		{
		}

		TVector4(const glm::vec4& InVec)
			: X(InVec.x)
			, Y(InVec.y)
			, Z(InVec.z) 
			, W(InVec.w) 
		{
		}

		TVector4(const ImVec4& InVec)
			: X(InVec.x)
			, Y(InVec.y)
			, Z(InVec.z) 
			, W(InVec.w) 
		{
		}

		~TVector4() 
		{
		} 

		TVector4& operator+=(const TVector4 Other)
		{
			X += Other.X;
			Y += Other.Y;
			Z += Other.Z;
			W += Other.W;
			return *this;
		}

		TVector4& operator-=(const TVector4& Other)
		{
			X -= Other.X;
			Y -= Other.Y;
			Z -= Other.Z;
			W -= Other.W;
			return *this;
		}

		TVector4 operator-(const TVector4& Other) const 
		{
			return TVector4((X - Other.X), (Y - Other.Y), (Z - Other.Z), (W - Other.W));
		}

		TVector4 operator+(const TVector4& Other) const 
		{
			return TVector4((X + Other.X), (Y + Other.Y), (Z + Other.Z), (W + Other.W));
		}

		bool operator==(const TVector4& Other) const 
		{
			return ((X == Other.X) && (Y == Other.Y) && (Z == Other.Z) && (W == Other.W));
		}

		bool operator!=(const TVector4& Other) const 
		{
			return !(*this == Other);
		}

		static float Distance(const TVector4& A, const TVector4& B)
		{
			return 0.0f;
		}

		TVector4 Normalize()
		{
			return { };
		}

		std::string ToString() const
		{
			return std::format("({:.2f}, {:.2f}, {:.2f}, {:.2f})", X, Y, Z, W);
		}

		friend std::ostream& operator<<(std::ostream& os, const TVector4& Vector) 
		{
			os << *Vector.ToString();
			return os;
		}

		operator glm::vec4() { return glm::vec4(X, Y, Z, W); }
		operator ImVec4() { return ImVec4(X, Y, Z, W); }

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
		>, "TVector4 can only be instantiated with int, float or double types");

	public:
	#if LK_VECTOR_ANONYMOUS_STRUCT
		union
		{
			struct { SizeType X, Y, Z, W; };
			struct { SizeType R, G, B, A; };
			struct { SizeType S, T, P, Q; };
		};
	#else
		union { SizeType X, R, S; };
		union { SizeType Y, G, T; };
		union { SizeType Z, B, P; };
		union { SizeType W, A, Q; };
	#endif
	};

}
