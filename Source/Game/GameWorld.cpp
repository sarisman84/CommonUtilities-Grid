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

GameWorld::GameWorld()
{}

GameWorld::~GameWorld() 
{}

void GameWorld::Init()  
{
	myTga2dLogoTexture = Tga2D::Engine::GetInstance()->GetTextureManager().GetTexture(L"sprites/tga_logo.dds");
	
	//model = Tga2D::ModelFactory::GetInstance().GetAnimatedModel(L"playerCharacter_sk.fbx");

	//std::vector<std::wstring> animations;
	//animations.push_back(L"playerCharacter_runAnim.fbx");
	//model.InitAnimations(animations);
	//model.SetCurrentAnimationIndex((unsigned short)0);

	myModelInstancer = std::make_shared<Tga2D::ModelInstancer>(Tga2D::ModelFactory::GetInstance().GetModelInstancer(L"Cube"));
	Tga2D::DX11::RenderStateManager->SetDepthStencilState(Tga2D::DepthStencilState::Write);

	for(int x = -600; x <= 600; x += 200)
	{
		for(int y = -600; y <= 600; y+= 200 )
		{
			for(int z = -400; z <= 400; z+= 200)
			{
				myModelInstancer->AddInstance({{ static_cast<float>(x), static_cast<float>(y), static_cast<float>(z) }});	
			}			
		}
	}

	myModelInstancer->RebuildInstances();

	Tga2D::Vector2ui resolution = Tga2D::Engine::GetInstance()->GetRenderSize();
    std::shared_ptr<Tga2D::Camera> camera = std::make_shared<Tga2D::Camera>(Tga2D::Camera());
    camera->SetPerspectiveProjection(
        90,
        {
            (float)resolution.x,
            (float)resolution.y
        },
        0.1f,
        50000.0f)
        ;

    camera->SetPosition(Tga2D::Vector3f(0.0f, 250.0f, -250.0f));
    camera->SetRotation(Tga2D::Rotator(45, 0, 0));
	Tga2D::Engine::GetInstance()->GetGraphicsEngine().SetCamera(*camera);
}
void GameWorld::Update(float aTimeDelta)
{
	UNREFERENCED_PARAMETER(aTimeDelta);
	//model.Update(aTimeDelta); // model.Update(1.0f / 60.0f);
}

void GameWorld::Render()
{
	Tga2D::SpriteDrawer& spriteDrawer(Tga2D::Engine::GetInstance()->GetGraphicsEngine().GetSpriteDrawer());
	Tga2D::SpriteSharedData sharedData = {};
	sharedData.myTexture = myTga2dLogoTexture;

	// Create instance data. 
	Tga2D::Sprite2DInstanceData spriteInstance = {};
	spriteInstance.myPivot = { 0.5f, 0.5f };
	spriteInstance.myPosition = { 0.5f, 0.5f };

	spriteDrawer.Draw(sharedData, spriteInstance);

	auto& fx = Tga2D::Engine::GetInstance()->GetGraphicsEngine().GetFullscreenEffectPixelation();
	fx.SetPixelSize(64.0f);
	fx.Activate(Tga2D::DX11::DepthBuffer);

	//Tga2D::DX11::BackBuffer->SetAsActiveTarget(Tga2D::DX11::DepthBuffer);

	Tga2D::ModelDrawer& modelDrawer(Tga2D::Engine::GetInstance()->GetGraphicsEngine().GetModelDrawer());
	modelDrawer.Draw(*myModelInstancer);

	//modelDrawer.Draw(model);

	fx.Render();

	Tga2D::DX11::BackBuffer->SetAsActiveTarget();
}