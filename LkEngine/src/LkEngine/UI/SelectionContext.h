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

		static void SelectEntity(Entity e)
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
		inline static Entity SelectedEntity{};
		inline static Entity LastSelectedEntity{};

		inline static SelectionContext* Context = nullptr;

		inline static constexpr Entity NULL_ENTITY{};
	};

	using SELECT = SelectionContext;
	using SELECTION = SelectionContext;

}