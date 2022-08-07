#include "stdafx.h"
#include "Grid.h"
#include <iostream>
#include <tga2d/engine.h>
#include <algorithm>

#pragma warning(disable: 4244)
#pragma warning(disable: 4189)



Grid::~Grid()
{
	for (size_t y = 0; y < myCells.size(); y++)
	{
		for (size_t x = 0; x < myCells[y].size(); x++)
		{
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
			myCells[y][x].myContents.clear();
		}
	}
	Initialize();
}

Tga2D::Vector2i Grid::GetCellPosAtPos(Tga2D::Vector2f aPosition)
{
	auto rez = Tga2D::Engine::GetInstance()->GetTargetSize();
	return Tga2D::Vector2i(floor(static_cast<int>((aPosition.x * rez.x) / (myCellSize.x * rez.x))), floor(static_cast<int>((aPosition.y * rez.y) / (myCellSize.y * rez.y))));
}

Cell* Grid::GetCellAtPos(Tga2D::Vector2f aPosition)
{
	auto flooredPos = GetCellPosAtPos(aPosition);

	if (IsPosOutOfBounds({ static_cast<float>(flooredPos.x), static_cast<float>(flooredPos.y) })) return nullptr;

	return &myCells[flooredPos.y][flooredPos.x];
}

Cell* Grid::GetCellAtPos(Tga2D::Vector2i aPosition)
{
	return &myCells[aPosition.y][aPosition.x];
}

void Grid::InsertObject(GridObject* anObject)
{
	auto cell = GetCellAtPos(anObject->myPosition);
	if (!cell) return;
	std::shared_ptr<GridObject> obj(anObject);
	cell->myContents.push_back(obj);
	std::cout << "Added Obj at " << cell->myPosition << std::endl;

	InsertObjectInGrid(obj, cell);
}

std::vector<std::shared_ptr<GridObject>> Grid::Raycast(Tga2D::Vector2f aStartPos, Tga2D::Vector2f anEndPos)
{
	std::vector<std::shared_ptr<GridObject>> r;

	//Taken from this article: http://playtechs.blogspot.com/2007/03/raytracing-on-grid.html
	auto startPos = GetCellPosAtPos(aStartPos);
	auto endPos = GetCellPosAtPos(anEndPos);

	Tga2D::Vector2i delta = { abs(endPos.x - startPos.x), abs(endPos.y - startPos.y) };
	Tga2D::Vector2i pos = startPos;
	int n = 1 + delta.x + delta.y;
	Tga2D::Vector2i increment = { anEndPos.x > aStartPos.x ? 1 : -1,anEndPos.y > aStartPos.y ? 1 : -1 };
	int error = delta.x - delta.y;
	delta = delta * Tga2D::Vector2i{ 2, 2 };
	bool isIntersectingCircle;

	for (; n > 0; --n)
	{
		r.clear();
		isIntersectingCircle = false;
		Tga2D::Vector2f d = Tga2D::Vector2f(anEndPos - aStartPos).GetNormalized();

		//visit
		auto cell = GetCellAtPos(pos);
		if (!cell->IsEmpty())
		{
			for (size_t i = 0; i < cell->myContents.size(); i++)
			{
				Tga2D::Vector2f u = cell->myContents[i]->myPosition - aStartPos;
				Tga2D::Vector2f pU = (u.Dot(d)) * d;
				Tga2D::Vector2f u2 = u - pU;
				float nD = u2.Length();
				isIntersectingCircle = nD <= cell->myContents[i]->mySize.x;

				r.push_back(cell->myContents[i]);

			}
		}

		if (isIntersectingCircle)
		{
			return r;
		}

		if (error > 0)
		{
			pos.x += increment.x;
			error -= delta.y;
		}
		else
		{
			pos.y += increment.y;
			error += delta.x;
		}

	}

	return r;
}

std::array<Tga2D::Vector2i, 2> Grid::BoundBoxToIndex(Tga2D::Vector2f aMinPos, Tga2D::Vector2f aMaxPos)
{
	std::array<Tga2D::Vector2i, 2> r;


	Tga2D::Vector2i minCell = GetCellPosAtPos(aMinPos);
	Tga2D::Vector2i maxCell = GetCellPosAtPos(aMaxPos);


	r[0] = { std::clamp(minCell.x, 0, 19), std::clamp(minCell.y, 0, 19) };

	r[1] = { std::clamp(maxCell.x, 0, 19), std::clamp(maxCell.y, 0, 19) };

	return r;
}

const bool Grid::IsPosOutOfBounds(const Tga2D::Vector2f aPosition)
{
	return aPosition.x < 0 || aPosition.x >= myGridSize.x || aPosition.y < 0 || aPosition.y >= myGridSize.y;
}

void Grid::InsertObjectInGrid(std::shared_ptr<GridObject> anObject, Cell* aCell)
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
