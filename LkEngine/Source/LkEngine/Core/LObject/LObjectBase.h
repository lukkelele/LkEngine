/******************************************************************
 * LObjectBase
 * 
 *
 *******************************************************************/
#pragma once

#include <concepts>

#include "LkEngine/Core/CoreTypes.h"
#include "LkEngine/Core/String.h"


namespace LkEngine {

	/**
	 * EInitFlag
	 */
	enum class EInitFlag
	{
		NoInit = 0,  /* Do not initialize object. */
		True,        /* Initialize object. */
	};

	/**
	 * LObjectFlag 
	 * 
	 *  Used for object initialization, loading and garbage collection.
	 */
	using LObjectFlag = uint32_t;
	namespace EObjectFlag
	{
		enum Flag : LObjectFlag
		{
			None = 0,              /* No flags. */
			NeedInitialization,    /* Need to be initialized. */
			NeedLoad,              /* Needs to get loaded to memory. */
			BeginDestroy,          /* Begin object destruction. */
			FinishDestroy,         /* Object destroyed. */
			Garbage,               /* Object deemed garbage and should get deleted. */
		};
	}

	/// TODO: Implement this
	template<typename TObject>
	concept HasCoreObjectFunctionality = requires(TObject Object)
	{
		Object.GetName();
	};

}
