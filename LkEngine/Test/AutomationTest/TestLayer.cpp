#include "TestLayer.h"


namespace LkEngine {

	LTestLayer::LTestLayer(const std::filesystem::path& ProjectPath)
	{
		LCLASS_REGISTER();
		LK_UNUSED(ProjectPath);
	}

	LTestLayer::~LTestLayer()
	{
	}

	void LTestLayer::OnAttach()
	{
		/* Setup all test instances.*/
		LK_CORE_INFO_TAG("TestLayer", "Setting up test instances");
	}

	void LTestLayer::OnDetach()
	{
	}

	void LTestLayer::OnUpdate(const float Timestep)
	{
		LK_UNUSED(Timestep);
	}

}