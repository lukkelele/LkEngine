#pragma once

#include <vector>

#include "LkEngine/Test/TestMacros.h"
#include "LkEngine/Test/AutomationTest.h"


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
