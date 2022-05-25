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

void Grid::Initialize()
{
	myCells.resize(myGridSize.y);


	for (float y = 0; y < myCells.size(); y++)
	{
		myCells[y].resize(myGridSize.x);
		for (float x = 0; x < myCells[y].size(); x++)
		{
			myCells[y][x].myPosition = { x,y };

			float xPos = x * myCellSize.x + myCellSize.x / 2.f;
			float yPos = y * myCellSize.y + myCellSize.y / 2.f;
			myCells[y][x].myWorldPosition = { xPos, yPos };
			myCells[y][x].myCellSize = myCellSize;
		}
	}
}

void Grid::Clear()
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
	Initialize();
}

Cell* Grid::GetCellAtPos(Tga2D::Vector2f aPosition)
{

	auto rez = Tga2D::Engine::GetInstance()->GetTargetSize();
	auto flooredPos = Tga2D::Vector2i(floor(static_cast<int>((aPosition.x * rez.x) / (myCellSize.x * rez.x))), floor(static_cast<int>((aPosition.y * rez.y) / (myCellSize.y * rez.y))));

	if (IsPosOutOfBounds({ static_cast<float>(flooredPos.x), static_cast<float>(flooredPos.y) })) return nullptr;

	return &myCells[flooredPos.y][flooredPos.x];
}

void Grid::InsertObject(GridObject* anObject)
{
	auto cell = GetCellAtPos(anObject->myPosition);
	if (!cell) return;
	cell->myContents.push_back(anObject);
	std::cout << "Added Obj at " << cell->myPosition << std::endl;

	InsertObjectInGrid(anObject, cell);
}

Cell* Grid::Raycast(Tga2D::Vector2f aStartPos, Tga2D::Vector2f anEndPos)
{
	auto rez = Tga2D::Engine::GetInstance()->GetTargetSize();
	Tga2D::Vector2f delta = (anEndPos - aStartPos).GetNormalized() * Tga2D::Engine::GetInstance()->GetDeltaTime();
	//delta.x /= rez.x;
	//delta.y /= rez.y;

	Cell* startingCell = GetCellAtPos(aStartPos);
	Cell* endingCell = GetCellAtPos(anEndPos);
	if (!startingCell || !endingCell) return nullptr;

	Cell* currentCell = GetCellAtPos(anEndPos + delta);



	while (true)
	{
		if (currentCell == startingCell) return endingCell;
		if (currentCell && currentCell->IsEmpty() && !IsPosOutOfBounds(currentCell->myWorldPosition - delta))
			currentCell = GetCellAtPos(currentCell->myWorldPosition - delta);
		else
			break;
	}
	return nullptr;
}

Tga2D::Vector2i* Grid::BoundBoxToIndex(Tga2D::Vector2f aMinPos, Tga2D::Vector2f aMaxPos)
{
	static Tga2D::Vector2i result[2];

	Cell* a = GetCellAtPos(aMinPos);
	Cell* b = GetCellAtPos(aMaxPos);

	if (a)
		result[0] = { static_cast<int>(a->myPosition.x), static_cast<int>(a->myPosition.y) };
	if (b)
		result[1] = { static_cast<int>(b->myPosition.x), static_cast<int>(b->myPosition.y) };

	return result;
}

const bool Grid::IsPosOutOfBounds(const Tga2D::Vector2f aPosition)
{
	return aPosition.x < 0 || aPosition.x >= myGridSize.x || aPosition.y < 0 || aPosition.y >= myGridSize.y;
}

void Grid::InsertObjectInGrid(GridObject* anObject, Cell* aCell)
{
	auto rez = Tga2D::Engine::GetInstance()->GetTargetSize();


	auto pos = Tga2D::Vector2f((anObject->myPosition.x * rez.x) / (myCellSize.x * rez.x), (anObject->myPosition.y * rez.y) / (myCellSize.y * rez.y));



	/*int yes = false;

	++++++--++--yes += -1 + true - false * true / false / -1;*/

	for (int y = -1; y <= 1; ++y)
	{
		for (int x = -1; x <= 1; ++x)
		{
			if (x == 0 && y == 0) continue;

			int yOffset = aCell->myPosition.y + y;
			int xOffset = aCell->myPosition.x + x;


			if (yOffset < 0 || yOffset >= myCells.size()) continue;
			if (xOffset < 0 || xOffset >= myCells[yOffset].size()) continue;


			if (AABBCheck(myCells[yOffset][xOffset].myWorldPosition, myCellSize, anObject->myPosition, anObject->mySize) && !myCells[yOffset][xOffset].Contains(anObject))
			{
				myCells[yOffset][xOffset].myContents.push_back(anObject);
				std::cout << " -> And on Cell " << Tga2D::Vector2i(xOffset, yOffset) << std::endl;
			}



		}
	}




}

const bool Grid::AABBCheck(Tga2D::Vector2f aPos1, Tga2D::Vector2f aSize1, Tga2D::Vector2f aPos2, Tga2D::Vector2f aSize2)
{
	float minA_X = aPos1.x - aSize1.x / 2.f;
	float minA_Y = aPos1.y - aSize1.y / 2.f;

	float maxA_X = aPos1.x + aSize1.x / 2.f;
	float maxA_Y = aPos1.y + aSize1.y / 2.f;



	float minB_X = aPos2.x - aSize2.x / 2.f;
	float minB_Y = aPos2.y - aSize2.y / 2.f;

	float maxB_X = aPos2.x + aSize2.x / 2.f;
	float maxB_Y = aPos2.y + aSize2.y / 2.f;


	return minA_X <= maxB_X && maxA_X >= minB_X
		&& minA_Y <= maxB_Y && maxA_Y >= minB_Y;
}
