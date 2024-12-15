#include "LKpch.h"
#include "PanelManager.h"

#include "LkEngine/Core/IO/FileSystem.h"
#include "LkEngine/Core/Globals.h"


namespace LkEngine {

	LPanelManager::LPanelManager()
	{
		LOBJECT_REGISTER();
	}

	LPanelManager::~LPanelManager()
	{
		for (auto& PanelMap : Panels)
		{
			PanelMap.clear();
		}
	}

	void LPanelManager::Initialize()
	{
		for (auto& PanelMap : Panels)
		{
			for (auto& [ID, PanelData] : PanelMap)
			{
				if (PanelData.Panel)
				{
					PanelData.Panel->Initialize();
				}
			}
		}
	}

	void LPanelManager::OnRenderUI()
	{
		for (auto& PanelMap : Panels)
		{
			for (auto& [ID, PanelData] : PanelMap)
			{
				bool bClosedThisFrame = false;

				if (PanelData.bIsOpen)
				{
					PanelData.Panel->OnRenderUI(PanelData.bIsOpen);
					bClosedThisFrame = !PanelData.bIsOpen;
				}

				if (bClosedThisFrame)
				{
					Serialize();
				}
			}
		}
	}

	void LPanelManager::RemovePanel(const char* PanelID)
	{
		const uint32_t ID = LHash::GenerateFNVHash(PanelID);
		for (auto& PanelMap : Panels)
		{
			if (PanelMap.find(ID) != PanelMap.end())
			{
				PanelMap.erase(ID);
				return;
			}
		}

		LK_CORE_ERROR_TAG("PanelManager", "Couldn't find Panel with id '{0}'", PanelID);
	}

	void LPanelManager::Serialize()
	{
		YAML::Emitter Out;
		Out << YAML::BeginMap;

		Out << YAML::Key << "Panels" << YAML::Value << YAML::BeginSeq;
		{
			for (std::size_t Category = 0; Category < Panels.size(); Category++)
			{
				for (const auto& [PanelID, PanelData] : Panels[Category])
				{
					LK_CORE_INFO_TAG("PanelManager", "Serializing: {}", PanelData.Name);
					Out << YAML::BeginMap;
					Out << YAML::Key << "ID" << YAML::Value << PanelID;
					Out << YAML::Key << "Name" << YAML::Value << PanelData.Name;
					Out << YAML::Key << "IsOpen" << YAML::Value << PanelData.bIsOpen;
					PanelData.Panel->SerializeToYaml(Out);

					Out << YAML::EndMap;
				}
			}
		}
		Out << YAML::EndSeq;
		Out << YAML::EndMap;

		std::ofstream FileOut(LFileSystem::GetConfigDir() / "EditorLayout.yaml");
		FileOut << Out.c_str();
		FileOut.close();
	}

	void LPanelManager::Deserialize()
	{
		const std::filesystem::path EditorLayoutPath = LFileSystem::GetConfigDir() / "EditorLayout.yaml";
		if (!LFileSystem::Exists(EditorLayoutPath))
		{
			LK_CORE_ERROR_TAG("PanelManager", "Editor layout cannot be found ({})", EditorLayoutPath.string());
			return;
		}

		std::ifstream Stream(EditorLayoutPath);
		LK_CORE_VERIFY(Stream);
		std::stringstream StringStream;
		StringStream << Stream.rdbuf();

		YAML::Node Data = YAML::Load(StringStream.str());
		if (!Data["Panels"])
		{
			LK_ERROR("Failed to load EditorLayout.yaml from {}", EditorLayoutPath.parent_path().string());
			return;
		}

		for (auto PanelNode : Data["Panels"])
		{
			FPanelData* PanelData = GetPanelData(PanelNode["ID"].as<uint32_t>(0));
			if (!PanelData)
			{
				continue;
			}

			PanelData->bIsOpen = PanelNode["IsOpen"].as<bool>(PanelData->bIsOpen);
			PanelData->Panel->DeserializeFromYaml(PanelNode);
		}
	}

}
