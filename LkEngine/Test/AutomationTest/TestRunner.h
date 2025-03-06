#pragma once

#include "AutomationTest/TestMacros.h"
#include "AutomationTest/AutomationTest.h"


namespace LkEngine {

	/**
	 * LTestRunner
	 */
	class LTestRunner 
	{
	public:
		LTestRunner();
		virtual ~LTestRunner() = default;

		void Run();

	public:
		std::filesystem::path ResultsDir{};
	};

}
