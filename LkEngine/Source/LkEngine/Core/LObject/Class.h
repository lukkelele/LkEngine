/******************************************************************
 * LClass
 *
 *
 *******************************************************************/
#pragma once

#include "LkEngine/Core/CoreMacros.h"
#include "LkEngine/Core/CoreTypes.h"
#include "LkEngine/Core/Template/TypeTrait.h"
#include "LkEngine/Core/LObject/ObjectBase.h"


namespace LkEngine {

	class LClass
	{
		/**
		 * @brief Cast function used to cast from LObject to derived type.
		 */
		using FObjectCastFunction = std::function<void*(LObjectBase*)>;

	public:
		explicit LClass(std::string_view InName, const std::type_index& InTypeID, FObjectCastFunction InCastFunction)
			: Name(InName)
			, TypeID(InTypeID)
			, CastFunction(std::move(InCastFunction))
		{
		}

		LClass() = default;
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
		 * @brief Retrieve metadata for a LClass.
		 */
		static const LClass* Get(const std::type_index& ClassType)
		{
			if (auto Iter = ClassMetadataMap.find(ClassType); Iter != ClassMetadataMap.end())
			{
				return Iter->second.get();
			}

			return nullptr;
		}

		/**
		 * @brief Register metadata for a LClass.
		 */
		template<typename T>
		static LClass* Register(std::string_view ClassName)
		{
			static_assert(sizeof(T) > 0, "Register failed, incomplete type T");
			if (ClassMetadataMap.contains(typeid(T)))
			{
				return ClassMetadataMap[typeid(T)].get();
			}

			ClassMetadataMap[typeid(T)] = std::make_unique<LClass>(ClassName, typeid(T), [](LObjectBase* ClassObject) -> void* 
			{ 
				return static_cast<T*>(ClassObject); 
			});

			LClass* Class = ClassMetadataMap[typeid(T)].get();
			Class->bRegistered = true;

			return Class;
		}
	
		bool operator==(const LClass& Other) const
		{
			return (TypeID == Other.TypeID);
		}

		/**
		 * @brief Check if class is registered in the static storage.
		 */
		FORCEINLINE bool IsRegistered() const 
		{ 
			return bRegistered; 
		}

		/**
		 * @brief Check if class is child of another class.
		 */
		bool IsChildOf(const LClass* OtherClass) const;

		template<typename T>
		bool IsChildOf() const
		{
			static_assert(sizeof(T) > 0, "IsChildOf failed, incomplete type T");
			return IsChildOf(T::StaticClass());
		}

	private:
		std::string Name{};
		std::type_index TypeID;
		FObjectCastFunction CastFunction;

		bool bRegistered = false;

		/** Static storage for all LClasses. */
		inline static std::unordered_map<std::type_index, std::unique_ptr<LClass>> ClassMetadataMap{};
	};

}
