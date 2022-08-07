#pragma once
#include "Ray.h"
#include "../Grid/Grid.h"

namespace Physics
{
	bool Raycast2D(Ray2D<float> aRay, Grid* aGrid, Cell* aFoundCell)
	{
		if (!aGrid) return false;

		if (!aGrid->GetCellAtPos(aRay.GetOrigin())->IsEmpty())
			return true;

		int x = floor(aRay.GetOrigin().x), y = floor(aRay.GetOrigin().y);

		x += floor(aRay.GetDirection().GetNormalized().x);
		y += floor(aRay.GetDirection().GetNormalized().y);

		bool hasFoundSomething;
		auto cell = aGrid->GetCellAtPos(Tga2D::Vector2i{ x,y });
		while (cell->IsEmpty() && x >= 0 && x < aGrid->myGridSize.x && y >= 0 && y < aGrid->myGridSize.y)
		{
			if (!cell->IsEmpty())
			{
				hasFoundSomething = true;
				aFoundCell = cell;
				break;
			}

			x += floor(aRay.GetDirection().GetNormalized().x);
			y += floor(aRay.GetDirection().GetNormalized().y);

			cell = aGrid->GetCellAtPos(Tga2D::Vector2i{ x,y });
		}


		return hasFoundSomething;
	}
}