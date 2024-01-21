#pragma once

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Components.h"

#include "LkEngine/Asset/Asset.h"

#include "LkEngine/Serialization/FileStream.h"

#include <yaml-cpp/yaml.h>


namespace LkEngine {

	class SceneLoader
	{
	public:
		Ref<Scene> LoadScene(const std::string& filepath); // e.g scene.lukkelele
		Ref<Scene> LoadScene(AssetHandle handle);

		template<typename T>
		T ParseComponent(const YAML::Node& node);

	private:
		std::filesystem::path m_Path;
	};

}