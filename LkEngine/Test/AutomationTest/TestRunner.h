#pragma once

#include <vector>

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

		void RegisterTests();
		void Run();

	private:
		std::vector<std::shared_ptr<Test::LAutomationTestBase>> Tests{};
	};

}
