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

struct DrawInfo
{
	std::shared_ptr<GridObject> myObj;
	Tga2D::Color myColor;

	inline const bool operator==(DrawInfo aLhs)
	{
		return myObj == aLhs.myObj;
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
	void DrawGrid();
	void DrawObjects();


	Grid myGrid;
	Tga2D::Color myLineColor = { 1,1,1,1 };
	float myViewDistance;

	std::vector<Tga2D::Text*> myTextElements;
	Tga2D::Vector2f myMousePosition;
	bool myShowIndexFlag = false;
	bool myShowCountFlag = false;
	bool myShowSearchAreaFlag = false;

	std::vector<DrawInfo> myObjectsToDraw;
	//Tga2D::AnimatedModelInstance model;
};