#include "stdafx.h"
#include "GameWorld.h"
#include <tga2d/engine.h>
#include <tga2d/error/ErrorManager.h>
#include <tga2d/graphics/GraphicsEngine.h>
#include <tga2d/sprite/sprite.h>
#include <tga2d/drawers/SpriteDrawer.h>
#include <tga2d/texture/TextureManager.h>

#include "tga2d/drawers/ModelDrawer.h"
#include "tga2d/graphics/Camera.h"
#include "tga2d/graphics/DX11.h"
#include "tga2d/model/ModelFactory.h"

#include <tga2d/text/text.h>

#include <vector>
#include <Custom/Input.h>
#include <imgui/imgui.h>


GameWorld::GameWorld()
{
}

GameWorld::~GameWorld()
{
	for (size_t i = 0; i < myTextElements.size(); i++)
	{
		if (myTextElements[i])
			delete myTextElements[i];
	}

	myTextElements.clear();

}

void GameWorld::Init()
{
	/*Tga2D::Engine::GetInstance()->SetResolution({ 800, 800 });*/
	auto a = Tga2D::Engine::GetInstance()->GetTargetSize();
	Tga2D::Vector2f resolution = { static_cast<float>(a.x),static_cast<float>(a.y) };


	myGrid.myGridSize = { 20, 20 };

	myGrid.myCellSize = { 40, 40 };
	myGrid.myCellSize.x /= resolution.x;
	myGrid.myCellSize.y /= resolution.y;

	myGrid.Initialize();

	myTextElements.resize(myGrid.myGridSize.x * myGrid.myGridSize.y);
	for (int i = 0; i < myGrid.myGridSize.x * myGrid.myGridSize.y; i++)
	{
		float x = static_cast<float>(i % myGrid.myGridSize.x) * myGrid.myCellSize.x;
		float y = static_cast<float>(i / myGrid.myGridSize.y) * myGrid.myCellSize.y;
		myTextElements[i] = new Tga2D::Text();
		myTextElements[i]->SetText(std::to_string(i));
		auto foundPos = Tga2D::Vector2f{ 0.025f + x - myTextElements[i]->GetWidth() / 2.f, y + 0.02f };
		myTextElements[i]->SetPosition(foundPos);

		myTextElements[i]->SetColor({ 1,1,1,1 });

		myGrid.myCells[static_cast<int>(y)][static_cast<int>(x)].myPosition = { x,y };
	}

	myViewDistance = 300 / resolution.x;



}

void GameWorld::Update(float aTimeDelta)
{
	static bool showIndexes = false;
	static bool gridMapState = false;
	static bool clearState = false;
	static bool showSearchArea = false;

	ImGui::BeginMainMenuBar();
	ImGui::MenuItem("Show Index", NULL, &showIndexes);
	ImGui::MenuItem("Grid Map", NULL, &gridMapState);
	ImGui::MenuItem("Clear Objects", NULL, &clearState);
	ImGui::MenuItem("Show Search Area", NULL, &showSearchArea);
	ImGui::EndMainMenuBar();


	if (showIndexes)
	{
		myShowIndexFlag = !myShowIndexFlag;
		showIndexes = false;
	}

	if (gridMapState)
	{

		myShowCountFlag = !myShowCountFlag;
		gridMapState = false;
	}

	if (showSearchArea)
	{
		myShowSearchAreaFlag = !myShowSearchAreaFlag;
		showSearchArea = false;
	}

	if (clearState)
	{
		myGrid.Clear();
		clearState = false;
	}


	for (int i = 0; i < myTextElements.size(); i++)
	{
		int x = i % myGrid.myGridSize.x;
		int y = i / myGrid.myGridSize.y;

		std::string output;

		if (myShowIndexFlag)
		{
			output += std::to_string(x) + "," + std::to_string(y) + "\n";
			myTextElements[i]->SetScale(0.75f);
		}


		if (myShowCountFlag)
		{
			output += std::to_string(myGrid.myCells[y][x].myContents.size());


			myTextElements[i]->SetScale(0.5f);
		}

		myTextElements[i]->SetText(output);

	}


	RECT rect;
	GetClientRect(*Tga2D::Engine::GetInstance()->GetHWND(), &rect);

	auto a = Tga2D::Engine::GetInstance()->GetTargetSize();
	Tga2D::Vector2f targetRez = { static_cast<float>(a.x),static_cast<float>(a.y) };
	Tga2D::Vector2f resolution = { static_cast<float>(rect.right),static_cast<float>(rect.bottom) };

	aTimeDelta;

	using namespace CommonUtilities;

	auto pos = Mouse::GetMousePosition();

	myMousePosition = { pos.x / targetRez.x, pos.y / targetRez.y };


	if (Mouse::GetButtonDown(Mouse::Key::LeftMouseButton))
	{
		auto obj = new GridObject();
		obj->myPosition = myMousePosition;
		obj->mySize = { 20.f / targetRez.x, 20.f / targetRez.y };
		myGrid.InsertObject(obj);
	}






}

void GameWorld::Render()
{
	int size = 1000;
	float offset = 0.001f;



	Tga2D::DebugDrawer& drawer = Tga2D::Engine::GetInstance()->GetDebugDrawer();
	std::vector<Tga2D::Vector2f> myStartPoint;
	std::vector<Tga2D::Vector2f> myEndPoint;
	std::vector<Tga2D::Color> myColor;
	for (float x = offset; x < 1; x += (myGrid.myCellSize.x))
	{
		if (myStartPoint.size() < size)
			myStartPoint.push_back({ x, 0.f });

		if (myEndPoint.size() < size)
			myEndPoint.push_back({ x,  1.f });

		if (myColor.size() < size)
			myColor.push_back(myLineColor);
	}


	for (float y = offset; y < 1; y += (myGrid.myCellSize.y))
	{
		if (myStartPoint.size() < size)
			myStartPoint.push_back({ 0.f,y });

		if (myEndPoint.size() < size)
			myEndPoint.push_back({ 1.f, y });

		if (myColor.size() < size)
			myColor.push_back(myLineColor);
	}


	drawer.DrawLines(&myStartPoint[0], &myEndPoint[0], &myColor[0], static_cast<unsigned int>(myStartPoint.size()));


	drawer.DrawCircle(myMousePosition, 0.01f);
	drawer.DrawCircle(myMousePosition, myViewDistance);


	for (size_t y = 0; y < myGrid.myCells.size(); y++)
	{
		for (size_t x = 0; x < myGrid.myCells[y].size(); x++)
		{
			myTextElements[x + myGrid.myGridSize.x * y]->SetColor({ 1,1,1,1 });



			if (!myGrid.myCells[y][x].IsEmpty())
			{
				for (size_t i = 0; i < myGrid.myCells[y][x].myContents.size(); i++)
				{
					drawer.DrawCircle(myGrid.myCells[y][x].myContents[i]->myPosition, myGrid.myCells[y][x].myContents[i]->mySize.myX);
				}
			}
		}
	}



	Tga2D::Vector2f minPos = { myMousePosition.x - myViewDistance, myMousePosition.y - myViewDistance };
	Tga2D::Vector2f maxPos = { myMousePosition.x + myViewDistance, myMousePosition.y + myViewDistance };
	if (minPos.y >= 0 && maxPos.y < myGrid.myGridSize.y && minPos.x >= 0 && maxPos.x < myGrid.myGridSize.x)
	{
		Tga2D::Vector2i* r = myGrid.BoundBoxToIndex(minPos, maxPos);

		for (int y = r[0].myY; y < r[1].myY; y++)
		{
			for (int x = r[0].myX; x < r[1].myX; x++)
			{
				
				if (myShowSearchAreaFlag)
					myTextElements[x + myGrid.myGridSize.x * y]->SetColor({ 0,1,0,1 });




				if (myGrid.myCells[y][x].IsEmpty()) continue;
				auto result = myGrid.Raycast(myMousePosition, myGrid.myCells[y][x].myWorldPosition);
				if (result && !result->IsEmpty())
				{
					for (size_t i = 0; i < result->myContents.size(); i++)
					{
						drawer.DrawLine(myMousePosition, result->myContents[i]->myPosition);
					}
				}
			}
		}
	}


	





	if (myShowIndexFlag || myShowCountFlag)
		for (size_t i = 0; i < myTextElements.size(); i++)
		{
			myTextElements[i]->Render();
		}
}