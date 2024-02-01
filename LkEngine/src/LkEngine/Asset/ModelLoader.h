#pragma once

#include "LkEngine/Scene/Model.h"

#include <filesystem>


namespace LkEngine {

	class ModelLoader
	{
	public:
		ModelLoader(std::filesystem::path filepath);
		~ModelLoader() = default;

		Ref<Model> Load(std::filesystem::path filepath);
		
	};

}