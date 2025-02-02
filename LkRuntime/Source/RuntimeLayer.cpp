#include "RuntimeLayer.h"


namespace LkEngine {

	LRuntimeLayer::LRuntimeLayer(const std::filesystem::path& ProjectPath)
	{
		LOBJECT_REGISTER();
		LK_UNUSED(ProjectPath);
	}

	LRuntimeLayer::~LRuntimeLayer()
	{
	}

	void LRuntimeLayer::OnAttach()
	{
	}

	void LRuntimeLayer::OnDetach()
	{
	}

	void LRuntimeLayer::Tick(const float Timestep)
	{
		LK_UNUSED(Timestep);
	}

	void LRuntimeLayer::OpenProject()
	{
	}

	void LRuntimeLayer::OpenScene(const std::string& InFilepath)
	{
		LK_UNUSED(InFilepath);
	}

	void LRuntimeLayer::LoadScene(const LUUID SceneHandle)
	{
		LK_UNUSED(SceneHandle);
	}

	void LRuntimeLayer::OnScenePlay()
	{
	}

	void LRuntimeLayer::OnSceneStop()
	{
	}

	void LRuntimeLayer::LoadSceneAssets()
	{
	}

}
