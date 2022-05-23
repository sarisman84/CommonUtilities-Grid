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

class GameWorld
{


public:
	GameWorld();
	~GameWorld();

	void Init();
	void Update(float aTimeDelta);
	void Render();

private:
	Grid myGrid;
	Tga2D::Color myLineColor = { 1,1,1,1 };
	float myViewDistance;

	std::vector<Tga2D::Text*> myTextElements;
	Tga2D::Vector2f myMousePosition;
	bool myShowIndexFlag = true;
	//Tga2D::AnimatedModelInstance model;
};