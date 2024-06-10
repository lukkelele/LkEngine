#pragma once

#include "LkEngine/Scene/Entity.h"


namespace LkEngine {

	class SelectionContext
	{
	public:
		SelectionContext()
		{
			Context = this;
		}

		static void SelectEntity(LEntity e)
		{
			LastSelectedEntity = SelectedEntity;
			SelectedEntity = e;
		}

		static void Clear()
		{
			ClearEntitySelection();
		}

		static void ClearEntitySelection()
		{
			LastSelectedEntity = SelectedEntity;
			SelectedEntity = NULL_ENTITY;
		}

		static SelectionContext* Get() { return Context; }

	public:
		inline static LEntity SelectedEntity{};
		inline static LEntity LastSelectedEntity{};

		inline static SelectionContext* Context = nullptr;

		//inline static const LEntity NULL_ENTITY{};
		inline static LEntity NULL_ENTITY{};
	};

	/// @FIXME
	using SELECT = SelectionContext;
	using SELECTION = SelectionContext;

}