#pragma once
#include <tga2d/math/Transform.h>
#include <vector>

struct GridObject
{
	Tga2D::Vector2f myPosition;
	Tga2D::Vector2f mySize;
};

struct Cell
{
	std::vector<GridObject*> myContents;
	Tga2D::Vector2f myPosition;
	Tga2D::Vector2f myWorldPosition;
	Tga2D::Vector2f myCellSize;
	inline bool IsEmpty()
	{
		return myContents.size() == 0;
	}

	inline bool Contains(GridObject* anObject)
	{
		return std::find(myContents.begin(), myContents.end(), anObject) != myContents.end();
	}

	inline const bool IsPosInsideCell(const Tga2D::Vector2f aPosition)
	{
		float minX = myWorldPosition.x - myCellSize.x / 2.f;
		float minY = myWorldPosition.y - myCellSize.y / 2.f;

		float maxX = myWorldPosition.x + myCellSize.x / 2.f;
		float maxY = myWorldPosition.y + myCellSize.y / 2.f;
		return aPosition.x >= minX && aPosition.x <= maxX && aPosition.y >= minY && aPosition.y <= maxY;
	}
};


struct Grid
{
public:
	~Grid();
	void Initialize();
	void Clear();
	std::vector<std::vector<Cell>> myCells;
	Tga2D::Vector2f myCellSize;
	Tga2D::Vector2i myGridSize;

	Cell* GetCellAtPos(Tga2D::Vector2f aPosition);

	void InsertObject(GridObject* anObject);


	Cell* Raycast(Tga2D::Vector2f aStartPos, Tga2D::Vector2f anEndPos);
	Tga2D::Vector2i* BoundBoxToIndex(Tga2D::Vector2f aMinPos, Tga2D::Vector2f aMaxPos);
private:
	const bool IsPosOutOfBounds(const Tga2D::Vector2f aPosition);

	void InsertObjectInGrid(GridObject* anObject, Cell* aCell);
	const bool AABBCheck(Tga2D::Vector2f aPos1, Tga2D::Vector2f aSize1, Tga2D::Vector2f aPos2, Tga2D::Vector2f aSize2);

};



