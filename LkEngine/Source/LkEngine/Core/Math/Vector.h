#pragma once

#include <type_traits>
#include <ostream>

#include "LkEngine/Core/String.h"


namespace LkEngine {

	/**
	 * TVector2
	 */
	template<typename T>
	class TVector2
	{
	public:
		TVector2()
			: X(T())
			, Y(T())
		{
		} 

		TVector2(const T InX, const T InY)
			: X(InX)
			, Y(InY)
		{
		} 

		template<typename R, typename = std::enable_if_t<std::is_convertible_v<R, T>>>
		TVector2(const R InX, const R InY)
			: X(static_cast<T>(InX))
			, Y(static_cast<T>(InY))
		{
		} 

		template<typename R, typename = std::enable_if_t<std::is_convertible_v<R, T>>>
		TVector2(const TVector2<R>& Other)
			: X(static_cast<T>(Other.X))
			, Y(static_cast<T>(Other.Y))
		{
		}

		TVector2(const glm::vec2& InVec)
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

		template<typename R>
		TVector2 operator-(const TVector2<R>& Other) const 
		{
			static_assert(std::is_convertible_v<R, T>, "Narrowing conversion, R -> T");
			return TVector2((X - static_cast<T>(Other.X)), (Y - static_cast<T>(Other.Y)));
		}

		TVector2 operator+(const TVector2& Other) const 
		{
			return TVector2((X + Other.X), (Y + Other.Y));
		}

		template<typename R>
		TVector2 operator+(const TVector2<R>& Other) const 
		{
			static_assert(std::is_convertible_v<R, T>, "Narrowing conversion, R -> T");
			return TVector2((X + static_cast<T>(Other.X)), (Y + static_cast<T>(Other.Y)));
		}

		template<typename R>
		TVector2& operator=(const TVector2<R> Other)
		{
			static_assert(std::is_convertible_v<R, T>, "Narrowing conversion, R -> T");
			X = static_cast<T>(Other.X);
			Y = static_cast<T>(Other.Y);
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

		template<typename R>
		bool operator==(const TVector2<R>& Other) const 
		{
			return ((X == static_cast<T>(Other.X))
					&& (Y == static_cast<T>(Other.Y)));
		}

		/** Implicit conversion to glm::vec2. */
		operator glm::vec2()
		{
			return glm::vec2(X, Y);
		}

		static float Distance(const TVector2& A, const TVector2& B)
		{
			return 0.0f;
		}

		FORCEINLINE std::string ToString() const
		{
			if constexpr (std::is_floating_point_v<T>)
			{
				return LK_FORMAT_STRING("({:.2f}, {:.2f})", X, Y);
			}
			else
			{
				return LK_FORMAT_STRING("({}, {})", X, Y);
			}
		}

		friend std::ostream& operator<<(std::ostream& os, const TVector2& Vector) 
		{
			//os << *Vector.ToString(); /* LString     */
			os << Vector.ToString();    /* std::string */
			return os;
		}

	public:
		T X;
		T Y;

		static_assert(std::disjunction_v<
			std::is_same<T, int>, 
			std::is_same<T, uint16_t>, 
			std::is_same<T, uint32_t>, 
			std::is_same<T, uint64_t>, 
			std::is_same<T, float>, 
			std::is_same<T, double>
		>, "TVector2 can only be instantiated with int, float or double.");
	};

	using LVector2 = TVector2<float>;


	/**
	 * TVector
	 */
	template<typename T>
	class TVector3
	{
	public:
		TVector3()
			: X(T())
			, Y(T())
			, Z(T()) 
		{
			static_assert(std::disjunction_v<
				std::is_same<T, int>, 
				std::is_same<T, float>, 
				std::is_same<T, double>
			>, "TVector can only be instantiated with int, float or double.");
		} 

		TVector3(T InX, T InY, T InZ)
			: X(InX)
			, Y(InY)
			, Z(InZ) 
		{
		}

		TVector3(T InXYZ)
			: X(InXYZ)
			, Y(InXYZ)
			, Z(InXYZ) 
		{
		}

		TVector3(const glm::vec3& InVec)
			: X(InVec.x)
			, Y(InVec.y)
			, Z(InVec.z) 
		{
		}

		~TVector3() 
		{
		} 

		TVector3& operator+=(const TVector3& Other)
		{
			X += Other.X;
			Y += Other.Y;
			Z += Other.Z;
			return *this;
		}

		TVector3& operator-=(const TVector3& Other)
		{
			X -= Other.X;
			Y -= Other.Y;
			Z -= Other.Z;
			return *this;
		}

		TVector3 operator-(const TVector3& Other) const 
		{
			return TVector3((X - Other.X), (Y - Other.Y), (Z - Other.Z));
		}

		TVector3 operator+(const TVector3& Other) const 
		{
			return TVector3((X + Other.X), (Y + Other.Y), (Z + Other.Z));
		}

		bool operator==(const TVector3& Other) const 
		{
			return ((X == Other.X) && (Y == Other.Y) && (Z == Other.Z));
		}

		bool operator!=(const TVector3& Other) const 
		{
			return !(*this == Other);
		}

		static float Distance(const TVector3& A, const TVector3& B)
		{
			return 0.0f;
		}

		TVector3 Normalize()
		{
			/// @FIXME:
			return { };
		}

		LString ToString() const
		{
			return LString::Format("({:.2f}, {:.2f}, {:.2f})", X, Y, Z);
		}

		friend std::ostream& operator<<(std::ostream& os, const TVector3& Vector) 
		{
			os << *Vector.ToString();
			return os;
		}

		static_assert(std::disjunction_v<
			std::is_same<T, int>, 
			std::is_same<T, float>, 
			std::is_same<T, double>
		>, "TVector can only be instantiated with int, float or double.");

	public:
		T X;
		T Y;
		T Z;
	};

	using LVector = TVector3<float>;

}
