#include "LKpch.h"
#include "ApplicationSerializer.h"

#include "Application.h"

#include "LkEngine/Renderer/Renderer.h"


namespace LkEngine {

	LApplicationSerializer::LApplicationSerializer(LApplication* InApplication)
		: Application(InApplication)
	{
		LK_CORE_ASSERT(InApplication, "Application instance is nullptr");
	}

	LApplicationSerializer::LApplicationSerializer(LApplication* InApplication, const std::filesystem::path& InConfigFile)
		: Application(InApplication)
		, ConfigFile(InConfigFile)
	{
		LK_CORE_ASSERT(InApplication, "Application instance is nullptr");
		LK_CORE_ASSERT(!InConfigFile.empty() && InConfigFile.has_filename(), "Invalid filepath: {}", InConfigFile.string());
	}

	void LApplicationSerializer::Serialize(const std::filesystem::path& InConfigFile)
	{
		LK_CORE_ASSERT(!InConfigFile.empty(), "Invoked Serialize with no config");
		ConfigFile = InConfigFile;

		LK_CORE_DEBUG_TAG("ApplicationSerializer", "Serializing: \"{}\"", ConfigFile.string());
		YAML::Emitter Out;
		SerializeTo<ESerializeFormat::Yaml>(Out);

		std::ofstream FileOut(ConfigFile);
		if (FileOut.is_open() && FileOut.good())
		{
			LK_CORE_DEBUG_TAG("ApplicationSerializer", "Saving to file: {}", ConfigFile.string());
			FileOut << Out.c_str();
		}
		else
		{
			LK_CORE_ERROR_TAG("ApplicationSerializer", "Failed to open: {}", ConfigFile.string());
		}
	}

	bool LApplicationSerializer::Deserialize(const std::filesystem::path& InConfigFile)
	{
		ConfigFile = InConfigFile;
		if (!std::filesystem::exists(ConfigFile))
		{
			LK_CORE_WARN_TAG("ApplicationSerializer", "File \"{}\" does not exist", ConfigFile.string());
			return false;
		}

		std::ifstream InputStream(ConfigFile);
		std::stringstream StrStream;
		StrStream << InputStream.rdbuf();
		try
		{
			DeserializeFromYaml(StrStream.str());
		}
		catch (const YAML::Exception& Exception)
		{
			/* TODO: Remove assert here and just report error. */
			LK_CORE_ASSERT(false, "Failed to deserialize '{}', error: '{}'", ConfigFile.string(), Exception.what());
			return false;
		}

		return true;
	}

	void LApplicationSerializer::SerializeToYaml(YAML::Emitter& Out)
	{
		if (Application)
		{
			LWindow& Window = Application->GetWindow();

			Out << YAML::BeginMap;

			/* Version. */
			Out << YAML::Key << "Version" << YAML::Value << "1.0";

			/* Window. */
			Out << YAML::Key << "Window";
			Out << YAML::Value << YAML::BeginMap;
			{
				const int Width = Window.GetWidth();
				const int Height = Window.GetHeight();
				if ((Width > 0) && (Width <= 4000))
				{
					Out << YAML::Key << "Width" << YAML::Value << Width;
				}
				else
				{
					Out << YAML::Key << "Width" << YAML::Value << LWindow::DEFAULT_WIDTH;
				}

				if ((Height > 0) && (Height <= 3200))
				{
					Out << YAML::Key << "Height" << YAML::Value << Height;
				}
				else
				{
					Out << YAML::Key << "Height" << YAML::Value << LWindow::DEFAULT_HEIGHT;
				}
			}
			Out << YAML::Value << YAML::EndMap;

			/* Renderer. */
			Out << YAML::Key << "Renderer";
			Out << YAML::Value << YAML::BeginMap;
			{
				Out << YAML::Key << "ClearColor" << YAML::Value << LRenderer::ClearColor;
			}
			Out << YAML::Value << YAML::EndMap;

			Out << YAML::EndMap;
		}
	}

	bool LApplicationSerializer::DeserializeFromYaml(const std::string& YamlString)
	{
		LK_CORE_ASSERT(!YamlString.empty(), "Deserialization failed, yaml string is empty");
		YAML::Node Data = YAML::Load(YamlString);

		/* The version node is required. */
		if (!Data["Version"])
		{
			LK_CORE_ERROR_TAG("ApplicationSerializer", "Failed to deserialize YAML, format not recognized");
			return false;
		}

		/* Window. */
		if (YAML::Node WindowNode = Data["Window"])
		{
			LWindow& Window = Application->GetWindow();
			int Width = LWindow::DEFAULT_WIDTH;
			int Height = LWindow::DEFAULT_HEIGHT;

			if (WindowNode["Width"])
			{
				Width = WindowNode["Width"].as<int>();
			}
			if (WindowNode["Height"])
			{
				Height = WindowNode["Height"].as<int>();
			}

			LK_CORE_DEBUG("WindowNode  Width={}  Height={}", Width, Height);
			Window.SetSize({ Width, Height });
		}

		/* Renderer. */
		if (YAML::Node RendererNode = Data["Renderer"])
		{
			glm::vec4 ClearColor = LRenderer::DEFAULT_CLEARCOLOR;
			if (RendererNode["ClearColor"])
			{
				ClearColor = RendererNode["ClearColor"].as<glm::vec4>();
			}
			LRenderer::ClearColor = ClearColor;

		}

		return true;
	}

}
