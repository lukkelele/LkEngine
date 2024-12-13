#pragma once

#include "LkEngine/Core/CoreTypes.h"
#include "LkEngine/Core/CoreMacros.h"
#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"


namespace LkEngine::Test {

	/**
	 * @brief Dummy test class.
	 * LObject is an abstract class so this is used as an object in tests.
	 */
	class LDummyTestClass : public LObject
	{
	public:
		LDummyTestClass()
		{
			LOBJECT_REGISTER();
		}

		~LDummyTestClass() = default;

	private:
		LCLASS(LDummyTestClass);
	};

}

