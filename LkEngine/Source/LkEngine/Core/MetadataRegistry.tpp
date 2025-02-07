#pragma once


namespace LkEngine {

	class LObject;

	template<typename T>
	void LMetadataRegistry::RegisterObject(std::string_view ObjectVariableName, const T& ObjectToRegister)
	{
		static_assert(Core::IsBaseOf_v<LObject, T>, "Cannot register an object that isn't derived from LObject");
		//static_assert(HasCoreObjectFunctionality<T>, "Object does not implement all required functionality");

		// TODO: Use custom logging.
		const std::string ObjectTypeName = typeid(T).name();
		const std::time_t ObjectCreationTime = std::time(nullptr);
		std::printf("[MetadataRegistry] Registering object: \"%s\"\n", ObjectVariableName.data());

		if (LiveCollection.contains(ObjectVariableName.data()))
		{
		}
		else
		{
			LiveCollection[ObjectVariableName.data()] = { ObjectTypeName, ObjectCreationTime };
			LK_CORE_DEBUG_TAG("MetadataRegistry", "Added '{}' to collection", ObjectVariableName);
		}
	}

}
