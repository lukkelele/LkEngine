#include "LKpch.h"
#include "Project.h"

#include "LkEngine/Scene/SceneSerializer.h"


namespace LkEngine {

	Project::Project(const ProjectSpecification& specification)
		: m_Specification(specification)
		, m_Name(specification.Name)
	{

	}

	Project::Project(const std::string& name)
		: m_Name(name)
	{
	}

	void Project::OnRender()
	{
	}

	void Project::OnImGuiRender()
	{
		ImGui::Text("Project: %s", m_Name.c_str());
	}

	// Save project data, aka serialize to disk
	// We can render imgui in this function because the main menubar is invoking this
	bool Project::Save()
	{
		LK_CORE_INFO_TAG("Project", "Saving project {}", m_Name);

		if (Data.TargetScene != nullptr)
		{
			SceneSerializer sceneSerializer(Data.TargetScene);
		}

		return true;
	}

	uint64_t Project::GetSize() const
	{
		return 0;
	}


}