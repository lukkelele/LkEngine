#pragma once


namespace LkEngine {

	class ScriptEngineConfig
	{
		bool EnableDebugging;
		bool EnableProfiling;
	};

    class ScriptEngine
    {
    public:
        static void Init(ScriptEngineConfig& scriptEngineConfig);
		static const ScriptEngineConfig& GetConfig();

    private:
    };

}