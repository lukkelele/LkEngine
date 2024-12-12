#include "LKpch.h"
#include "AssetManager.h"


namespace LkEngine {

	void LAssetManager::Initialize()
	{
	}

	void LAssetManager::Destroy()
	{
		/* FIXME */
		LProject::GetRuntimeAssetManager()->Destroy();
	}

}
