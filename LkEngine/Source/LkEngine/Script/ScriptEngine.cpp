#include "LKpch.h"
#include "LkEngine/Script/ScriptEngine.h"


namespace LkEngine {

    static ScriptEngineConfig Config;

    void ScriptEngine::Init(ScriptEngineConfig& config)
    {
        Config = config;
    }

    const ScriptEngineConfig& ScriptEngine::GetConfig()
    {
        return Config;
    }

}