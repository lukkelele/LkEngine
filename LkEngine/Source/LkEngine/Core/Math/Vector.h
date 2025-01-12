#pragma once

#include <type_traits>
#include <ostream>

/* TODO: Forward declare ImVec2 and ImVec4 and do not directly include imgui.h, 
 *       need to fix the inclusion hierarchy before that is possible though. */
#if 1
#	include <imgui/imgui.h>
#else
namespace ImGui 
{
	struct ImVec2;
	struct ImVec4;
}
#endif

#define LK_VECTOR_ANONYMOUS_STRUCT 0


namespace LkEngine {

	/**
	 * TVector2
	 */
	template<typename VecType>
	struct TVector2
	{
	public:
		TVector2()
			: X(VecType())
			, Y(VecType())
		{
		} 

		TVector2(const VecType InX, const VecType InY)
			: X(InX)
			, Y(InY)
		{
		} 

		template<typename OtherType, typename = std::enable_if_t<std::is_convertible_v<OtherType, VecType>>>
		TVector2(const OtherType InX, const OtherType InY)
			: X(static_cast<VecType>(InX))
			, Y(static_cast<VecType>(InY))
		{
		} 

		template<typename OtherType, typename = std::enable_if_t<std::is_convertible_v<OtherType, VecType>>>
		TVector2(const TVector2<OtherType>& Other)
			: X(static_cast<VecType>(Other.X))
			, Y(static_cast<VecType>(Other.Y))
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
			static_assert(std::is_convertible_v<OtherType, VecType>, "Narrowing conversion");
			return TVector2((X - static_cast<VecType>(Other.X)), (Y - static_cast<VecType>(Other.Y)));
		}

		TVector2 operator+(const TVector2& Other) const 
		{
			return TVector2((X + Other.X), (Y + Other.Y));
		}

		template<typename OtherType>
		TVector2 operator+(const TVector2<OtherType>& Other) const 
		{
			static_assert(std::is_convertible_v<OtherType, VecType>, "Narrowing conversion");
			return TVector2((X + static_cast<VecType>(Other.X)), (Y + static_cast<VecType>(Other.Y)));
		}

		template<typename OtherType>
		TVector2& operator=(const TVector2<OtherType> Other)
		{
			static_assert(std::is_convertible_v<OtherType, VecType>, "Narrowing conversion");
			X = static_cast<VecType>(Other.X);
			Y = static_cast<VecType>(Other.Y);
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
			return ((X == static_cast<VecType>(Other.X)) && (Y == static_cast<VecType>(Other.Y)));
		}

		FORCEINLINE bool IsNull() const
		{
			return ((X == 0) && (Y == 0));
		}

		static float Distance(const TVector2& A, const TVector2& B)
		{
			return 0.0f;
		}

		template<typename StringType = std::string>
		FORCEINLINE StringType ToString() const
		{
			if constexpr (std::is_same_v<StringType, std::string>)
			{
				if constexpr (std::is_floating_point_v<VecType>)
				{
					return LK_FORMAT_STRING("({:.2f}, {:.2f})", X, Y);
				}
				else
				{
					return LK_FORMAT_STRING("({}, {})", X, Y);
				}
			}
			else if constexpr (std::is_same_v<StringType, const char*>)
			{
				static constexpr uint16_t BufSize = 256;
				if constexpr (std::is_floating_point_v<VecType>)
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

		operator glm::vec2() { return glm::vec2(X, Y); }
		operator ImVec2() { return ImVec2(X, Y); }

		static_assert(std::disjunction_v<
			std::is_same<VecType, int>, 
			std::is_same<VecType, uint16_t>, 
			std::is_same<VecType, uint32_t>, 
			std::is_same<VecType, uint64_t>, 
			std::is_same<VecType, int16_t>, 
			std::is_same<VecType, int32_t>, 
			std::is_same<VecType, int64_t>, 
			std::is_same<VecType, float>, 
			std::is_same<VecType, double>
		>, "TVector2 can only be instantiated with int, float or double types");

	public:
	#if LK_VECTOR_ANONYMOUS_STRUCT
		union
		{
			struct { VecType X, Y; };
			struct { VecType R, G; };
			struct { VecType S, T; };
		};
	#else
		union { VecType X, R, S; };
		union { VecType Y, G, T; };
	#endif
	};


	/**
	 * TVector3
	 */
	template<typename VecType>
	struct TVector3
	{
	public:
		TVector3()
			: X(VecType())
			, Y(VecType())
			, Z(VecType()) 
		{
		} 

		TVector3(VecType InX, VecType InY, VecType InZ)
			: X(InX)
			, Y(InY)
			, Z(InZ) 
		{
		}

		TVector3(VecType InXYZ)
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
			return { };
		}

		std::string ToString() const
		{
			return std::format("({:.2f}, {:.2f}, {:.2f})", X, Y, Z);
		}

		friend std::ostream& operator<<(std::ostream& os, const TVector3& Vector) 
		{
			os << *Vector.ToString();
			return os;
		}

		operator glm::vec3() { return glm::vec3(X, Y, Z); }

		static_assert(std::disjunction_v<
			std::is_same<VecType, int>, 
			std::is_same<VecType, uint16_t>, 
			std::is_same<VecType, uint32_t>, 
			std::is_same<VecType, uint64_t>, 
			std::is_same<VecType, int16_t>, 
			std::is_same<VecType, int32_t>, 
			std::is_same<VecType, int64_t>, 
			std::is_same<VecType, float>, 
			std::is_same<VecType, double>
		>, "TVector3 can only be instantiated with int, float or double types");

	public:
	#if LK_VECTOR_ANONYMOUS_STRUCT
		union
		{
			struct { VecType X, Y, Z; };
			struct { VecType R, G, B; };
			struct { VecType S, T, P; };
		};
	#else
		union { VecType X, R, S; };
		union { VecType Y, G, T; };
		union { VecType Z, B, P; };
	#endif
	};


	/**
	 * TVector4
	 */
	template<typename VecType>
	struct TVector4
	{
	public:
		TVector4()
			: X(VecType())
			, Y(VecType())
			, Z(VecType()) 
			, W(VecType()) 
		{
		} 

		TVector4(VecType InX, VecType InY, VecType InZ, VecType InW)
			: X(InX)
			, Y(InY)
			, Z(InZ) 
			, W(InW)
		{
		}

		TVector4(VecType InXYZW)
			: X(InXYZW)
			, Y(InXYZW)
			, Z(InXYZW) 
			, W(InXYZW)
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
			std::is_same<VecType, int>, 
			std::is_same<VecType, uint16_t>, 
			std::is_same<VecType, uint32_t>, 
			std::is_same<VecType, uint64_t>, 
			std::is_same<VecType, int16_t>, 
			std::is_same<VecType, int32_t>, 
			std::is_same<VecType, int64_t>, 
			std::is_same<VecType, float>, 
			std::is_same<VecType, double>
		>, "TVector4 can only be instantiated with int, float or double types");

	public:
	#if LK_VECTOR_ANONYMOUS_STRUCT
		union
		{
			struct { VecType X, Y, Z, W; };
			struct { VecType R, G, B, A; };
			struct { VecType S, T, P, Q; };
		};
	#else
		union { VecType X, R, S; };
		union { VecType Y, G, T; };
		union { VecType Z, B, P; };
		union { VecType W, A, Q; };
	#endif
	};


	/* Typedefs. */
	using LVector2 = TVector2<float>;
	using LVector3 = TVector3<float>;
	using LVector4 = TVector4<float>;
	using LVector  = LVector3;

}
