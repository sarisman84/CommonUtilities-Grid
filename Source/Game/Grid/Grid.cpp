#include "stdafx.h"
#include "Grid.h"
#include <iostream>
#include <tga2d/engine.h>

#pragma warning(disable: 4244)
#pragma warning(disable: 4189)

Grid::~Grid()
{
	for (size_t y = 0; y < myCells.size(); y++)
	{
		for (size_t x = 0; x < myCells[y].size(); x++)
		{
			for (size_t e = 0; e < myCells[y][x].myContents.size(); e++)
			{
				if (myCells[y][x].myContents[e])
					delete myCells[y][x].myContents[e];
			}

			myCells[y][x].myContents.clear();
		}
	}
}

Cell* Grid::GetCellAtPos(Tga2D::Vector2f aPosition)
{
	auto rez = Tga2D::Engine::GetInstance()->GetTargetSize();

	return &myCells[floor(static_cast<int>((aPosition.x * rez.x) / (myCellSize.x * rez.x)))][floor(static_cast<int>((aPosition.y * rez.y) / (myCellSize.y * rez.y)))];
}

void Grid::InsertObject(GridObject* anObject)
{
	auto cell = GetCellAtPos(anObject->myPosition);

	cell->myContents.push_back(anObject);

	InsertObjectInGrid(anObject);
}

void Grid::InsertObjectInGrid(GridObject* anObject)
{
	auto rez = Tga2D::Engine::GetInstance()->GetTargetSize();

	auto flooredPos = Tga2D::Vector2i(floor(static_cast<int>((anObject->myPosition.x * rez.x) / (myCellSize.x * rez.x))), floor(static_cast<int>((anObject->myPosition.y * rez.y) / (myCellSize.y * rez.y))));
	auto originator = GetCellAtPos(anObject->myPosition);

	auto halfExtendsX = (anObject->mySize.x) / 2.f;
	auto halfExtendsY = (anObject->mySize.y) / 2.f;

	std::cout << "Added Obj at " << flooredPos << std::endl;

	for (int y = -floor(halfExtendsY); y <= ceil(halfExtendsY); y++)
	{
		for (int x = -floor(halfExtendsY); x <= ceil(halfExtendsX); x++)
		{
			int xOffset = x + flooredPos.x;
			int yOffset = y + flooredPos.y;

			if (yOffset < 0 || yOffset >= myCells.size()) continue;
			if (xOffset < 0 || xOffset >= myCells[yOffset].size()) continue;
			if (xOffset == 0 || yOffset == 0) continue;
			if (xOffset == flooredPos.x && yOffset == flooredPos.y) continue;

			if (!myCells[yOffset][xOffset].Contains(anObject))
			{
				myCells[yOffset][xOffset].myContents.push_back(anObject);
				std::cout << " -> And on Cell " << Tga2D::Vector2i(xOffset, yOffset) << std::endl;
			}
		}
	}




}
