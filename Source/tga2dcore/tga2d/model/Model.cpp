#include "stdafx.h"
#include <tga2d/graphics/DX11.h>
#include <tga2d/math/Transform.h>
#include <tga2d/model/Model.h>
#include <tga2d/shaders/ModelShader.h>

#include "ModelFactory.h"

using namespace Tga2D;

void Model::Init(MeshData& aMeshData, const std::wstring& aPath)
{
	myMeshData. push_back(std::move(aMeshData));
	myPath = aPath;
}

void Model::Init(std::vector<MeshData>& someMeshData, const std::wstring& aPath)
{
	assert(someMeshData.size() <= MAX_MESHES_PER_MODEL);

	myMeshData = std::move(someMeshData);
	myPath = aPath;
}

bool Model::LoadAnimation(const AnimationImportDefinition& aDefinition)
{
	if(Tga2D::ModelFactory::GetInstance().LoadAnimation(aDefinition.Path, *this))
	{
		Animation& anim = mySkeleton.myAnimations.back();
		anim.IsLooping = aDefinition.ShouldLoop;
		anim.IsInterpolating = aDefinition.ShouldInterpolate;
		if(aDefinition.OverrideFPS > 0)
			anim.FramesPerSecond = aDefinition.OverrideFPS;

		return true;
	}

	return false;
}
