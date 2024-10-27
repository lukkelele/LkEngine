/******************************************************************
 * LClass
 *
 *
 *******************************************************************/
#pragma once

#include "LkEngine/Core/CoreMacros.h"
#include "LkEngine/Core/CoreTypes.h"
#include "LkEngine/Core/TypeTrait.h"
#include "LkEngine/Core/LObject/Enum.h"


namespace LkEngine {
	
	class LObject;

	class LClass
	{
		/**
		 * @brief Cast function used to cast from LObject to derived type.
		 */
		using FObjectCastFunction = std::function<void*(LObject*)>;

	public:
		explicit LClass(std::string_view InName, const std::type_index& InTypeID, FObjectCastFunction InCastFunction)
			: Name(InName)
			, TypeID(InTypeID)
			, CastFunction(std::move(InCastFunction))
		{
		}

		LClass() = delete;
		virtual ~LClass() = default;

		/**
		 * @brief Get name of class.
		 */
		FORCEINLINE const std::string& GetName() const 
		{ 
			return Name; 
		}

		/**
		 * @brief Get type ID.
		 */
		[[nodiscard]] FORCEINLINE const std::type_index& GetTypeID() const 
		{ 
			return TypeID; 
		}

		/**
		 * @brief Check if class is of a specific type.
		 */
		template<typename T>
		bool IsA() const
		{
			static_assert(sizeof(T) > 0, "IsA failed, incomplete type T");
			return (TypeID == std::type_index(typeid(T)));
		}

		/**
		 * @brief Attempt to cast object to type T.
		 */
		template <typename T>
		T* CastTo(void* InObject) const
		{
			static_assert(sizeof(T) > 0, "CastTo failed, incomplete type T");
			return (IsA<T>() ? static_cast<T*>(CastFunction(static_cast<LObject*>(InObject))) : nullptr);
		}

		/**
		 * @brief Register metadata for a LClass.
		 */
		template<typename T>
		static void Register(std::string_view ClassName)
		{
			static_assert(sizeof(T) > 0, "Register failed, incomplete type T");
			ClassMetadataMap[typeid(T)] = MakeUnique<LClass>(ClassName, typeid(T), [](LObject* ClassObject) -> void* 
			{ 
				return static_cast<T*>(ClassObject); 
			});
		}

		/**
		 * @brief Retrieve metadata for a LClass.
		 */
		static const LClass* Get(const std::type_index& ClassType)
		{
			if (auto Iter = ClassMetadataMap.find(ClassType); Iter != ClassMetadataMap.end())
			{
				return Iter->second.get();
			}

			LK_CORE_ASSERT(false, "LClass::Get failed, \"{}\" is not in class map", ClassType.name());
			return nullptr;
		}
		
		bool operator==(const LClass& Other) const
		{
			return (TypeID == Other.TypeID);
		}

	private:
		std::string Name{};
		std::type_index TypeID;
		FObjectCastFunction CastFunction;

		/** Storage for all class metadata. */
		inline static std::unordered_map<std::type_index, TUniquePtr<LClass>> ClassMetadataMap{};
	};

}
