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
	inline bool IsEmpty()
	{
		return myContents.size() == 0;
	}

	inline bool Contains(GridObject* anObject)
	{
		return std::find(myContents.begin(), myContents.end(), anObject) != myContents.end();
	}
};


struct Grid
{
public:
	~Grid();
	std::vector<std::vector<Cell>> myCells;
	Tga2D::Vector2f myCellSize;
	Tga2D::Vector2i myGridSize;

	Cell* GetCellAtPos(Tga2D::Vector2f aPosition);

	void InsertObject(GridObject* anObject);
private:
	void InsertObjectInGrid(GridObject* anObject);

};



