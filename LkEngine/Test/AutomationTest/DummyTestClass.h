#pragma once

#include "LkEngine/Core/CoreTypes.h"
#include "LkEngine/Core/CoreMacros.h"
#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"


namespace LkEngine::Test {

	/**
	 * LDummyTestClass
	 *
	 *  Dummy class.
	 *  LObject is an abstract class so this is used as an object in test scenarios.
	 */
	class LDummyTestClass : public LObject
	{
	public:
		LDummyTestClass() { LOBJECT_REGISTER(); }
		~LDummyTestClass() = default;

	private:
		LCLASS(LDummyTestClass);
	};


	/**
	 * LDummerTestClass
	 *
	 *  Dummy class that derives from LDummyTestClass.
	 */
	class LDummerTestClass : public LDummyTestClass
	{
	public:
		LDummerTestClass() { LOBJECT_REGISTER(); }
		~LDummerTestClass() = default;

	private:
		LCLASS(LDummerTestClass);
	};


	/**
	 * LDummyTestClassExpanded
	 *
	 *  LDummyTestClass but with some member variables as well.
	 */
	class LDummyTestClassExpanded : public LObject
	{
	public:
		LDummyTestClassExpanded() { LOBJECT_REGISTER(); }
		~LDummyTestClassExpanded() = default;

	public:
		std::string Member_String{};
	protected:
		uint16_t IntMember1 = 0;
		float FloatMember1 = 0.0f;
	private:
		LCLASS(LDummyTestClassExpanded);
	};


	/**
	 * LDummerTestClassExpanded
	 *
	 *  Dummy class that derives from LDummyTestClassExpanded.
	 */
	class LDummerTestClassExpanded : public LDummyTestClassExpanded
	{
	public:
		LDummerTestClassExpanded() { LOBJECT_REGISTER(); }
		~LDummerTestClassExpanded() = default;

	private:
		LCLASS(LDummerTestClassExpanded);
	};

}

