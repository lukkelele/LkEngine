#pragma once

#include <type_traits>
#include <ostream>

#include "LkEngine/Core/String.h"


namespace LkEngine {

	template<typename T>
	class TVector2
	{
	public:
		T X;
		T Y;

		static_assert(std::disjunction_v<
			std::is_same<T, int>, 
			std::is_same<T, float>, 
			std::is_same<T, double>
		>, "TVector2 can only be instantiated with int, float or double.");
	};

	using LVector2 = TVector2<float>;

	template<typename T>
	class TVector
	{
	public:
		TVector()
			: X(T())
			, Y(T())
			, Z(T()) 
		{
		} 

		TVector(T InX, T InY, T InZ)
			: X(InX)
			, Y(InY)
			, Z(InZ) 
		{
		}

		TVector(T InXYZ)
			: X(InXYZ)
			, Y(InXYZ)
			, Z(InXYZ) 
		{
		}

		~TVector() 
		{
		} 

		TVector& operator+=(const TVector& Other)
		{
			X += Other.X;
			Y += Other.Y;
			Z += Other.Z;
			return *this;
		}

		TVector& operator-=(const TVector& Other)
		{
			X -= Other.X;
			Y -= Other.Y;
			Z -= Other.Z;
			return *this;
		}

		TVector operator-(const TVector& Other) const 
		{
			return TVector(X - Other.X, Y - Other.Y, Z - Other.Z);
		}

		TVector operator+(const TVector& Other) const 
		{
			return TVector(X + Other.X, Y + Other.Y, Z + Other.Z);
		}

		static float Distance(const TVector& A, const TVector& B)
		{
			return 0.0f;
		}

		TVector Normalize()
		{
			/// @FIXME:
			return { };
		}

		LString ToString() const
		{
			return LString::Format("{{ {0}, {1}, {2} }}", X, Y, Z);
		}

		friend std::ostream& operator<<(std::ostream& os, const TVector& Vector) 
		{
			//os << "{ " << Vector.X << ", " << Vector.Y << ", " << Vector.Z << " }";
			os << *Vector.ToString();
			return os;
		}

		static_assert(std::disjunction_v<
			std::is_same<T, int>, 
			std::is_same<T, float>, 
			std::is_same<T, double>
		>, "TVector can only be instantiated with int, float or double.");

	public:
		T X, Y, Z;
	};

	using LVector = TVector<float>;

}