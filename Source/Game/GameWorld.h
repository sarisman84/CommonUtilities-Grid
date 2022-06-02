#pragma once
#include "tga2d/render/RenderCommon.h"
#include "tga2d/model/AnimatedModelInstance.h"
#include <tga2d/math/color.h>

#include "Grid/Grid.h"

namespace Tga2D
{
	class ModelInstancer;
	class Texture;
	class Text;
}

struct ObjectDrawInfo
{
	std::shared_ptr<GridObject> myObj;
	Tga2D::Color myColor;

	inline const bool operator==(ObjectDrawInfo aLhs)
	{
		return myObj == aLhs.myObj;
	}
};


struct LineDrawInfo
{
	Tga2D::Vector2f myStartPos, myEndPos;
	Tga2D::Color myColor;

	inline const bool operator==(LineDrawInfo aLhs)
	{
		return myStartPos == aLhs.myStartPos && myEndPos == aLhs.myEndPos;
	}
};


class GameWorld
{


public:
	GameWorld();
	~GameWorld();

	void Init();
	void Update(float aTimeDelta);
	void Render();

private:


	const bool IsInViewRadius(std::array<Tga2D::Vector2i, 2> someBounds, size_t someX, size_t someY);
	const bool IsAlreadyMarkedForDraw(std::shared_ptr<GridObject> anObj);
	const bool IsAlreadyMarkedForDraw(LineDrawInfo someLineDrawInfo);

	std::array<Tga2D::Vector2i, 2> FetchBoundBox();
	void CollisionUpdate();
	void TextElementUpdate();
	void EditorUpdate();
	void DrawGrid();
	void DrawObjects();
	void DrawTextElements();


	Grid myGrid;
	Tga2D::Color myLineColor = { 1,1,1,1 };
	float myViewDistance;

	std::vector<Tga2D::Text*> myTextElements;
	Tga2D::Vector2f myMousePosition;
	bool myShowIndexFlag = false;
	bool myShowCountFlag = false;
	bool myShowSearchAreaFlag = false;
	std::vector<std::shared_ptr<GridObject>> myObjectsToCollide;
	std::vector<ObjectDrawInfo> myObjectsToDraw;
	std::vector<LineDrawInfo> myLinesToDraw;
	//Tga2D::AnimatedModelInstance model;
};