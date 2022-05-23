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

	myGrid.myCells.resize(myGrid.myGridSize.y);
	for (size_t i = 0; i < myGrid.myCells.size(); i++)
	{
		myGrid.myCells[i].resize(myGrid.myGridSize.x);
	}
	myTextElements.resize(myGrid.myGridSize.x * myGrid.myGridSize.y);
	for (int i = 0; i < myGrid.myGridSize.x * myGrid.myGridSize.y; i++)
	{
		float x = static_cast<float>(i % myGrid.myGridSize.x) * myGrid.myCellSize.x;
		float y = static_cast<float>(i / myGrid.myGridSize.y) * myGrid.myCellSize.y;
		myTextElements[i] = new Tga2D::Text();
		myTextElements[i]->SetText(std::to_string(i));
		auto foundPos = Tga2D::Vector2f{ 0.025f + x - myTextElements[i]->GetWidth() / 2.f, 0.03f + y };
		myTextElements[i]->SetPosition(foundPos);

		myTextElements[i]->SetColor({ 1,1,1,1 });

		myGrid.myCells[static_cast<int>(y)][static_cast<int>(x)].myPosition = { x,y };
	}

	myViewDistance = 300 / resolution.x;



}

void GameWorld::Update(float aTimeDelta)
{
	bool usedEditor = false;
	ImGui::Begin("Editor");

	if (ImGui::Button("Show Index"))
	{
		myShowIndexFlag = !myShowIndexFlag;
		usedEditor = true;
	}


	for (float y = 0; y < myGrid.myCells.size(); y++)
	{

		for (float x = 0; x < myGrid.myCells[static_cast<int>(y)].size(); x++)
		{
			float i = (x + myGrid.myGridSize.x * y);
			ImGui::SameLine();
			ImGui::InputFloat("", &i);
		}
		ImGui::NewLine();
	}


	ImGui::End();

	if (usedEditor) return;

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
			if (!myGrid.myCells[y][x].IsEmpty())
			{
				for (size_t i = 0; i < myGrid.myCells[y][x].myContents.size(); i++)
				{
					drawer.DrawCircle(myGrid.myCells[y][x].myContents[i]->myPosition, myGrid.myCells[y][x].myContents[i]->mySize.Length());
				}
			}
		}
	}

	if (!myShowIndexFlag)
		for (size_t i = 0; i < myTextElements.size(); i++)
		{
			myTextElements[i]->Render();
		}
}