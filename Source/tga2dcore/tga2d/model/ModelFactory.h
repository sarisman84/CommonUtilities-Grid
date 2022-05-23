#pragma once
#include <map>
#include <string>
#include <vector>
#include <tga2d/graphics/Vertex.h>
#include <tga2d/model/model.h>
#include <tga2d/model/AnimatedModelInstance.h>

#include "ModelInstancer.h"

struct ID3D11Device;

namespace Tga2D
{

class Texture;
class AnimatedModel;

// This doesn't appear to be used?
//struct MeshData
//{
//	std::vector<Vertex> myVertices;
//	std::vector<unsigned int> myIndices;
//	BoxSphereBounds myBounds;
//};

class ModelInstance;
class Model;

class ModelFactory
{
	// I really hate to do this but there's no logical way to load animations for a
	// model right now.
	friend Model;

	bool InitUnitCube();
	bool InitUnitPlane();
	bool InitPrimitives();
	
public:
	ModelFactory();
	~ModelFactory();
	static ModelFactory& GetInstance() { if (!myInstance) { myInstance = new ModelFactory(); } return *myInstance; }
	static void DestroyInstance() { if (myInstance) { delete myInstance; myInstance = nullptr; } }

	AnimatedModelInstance GetAnimatedModel(const std::wstring& someFilePath);
	ModelInstance GetModel(const std::wstring& someFilePath);

	ModelInstancer GetModelInstancer(const std::wstring& someFilePath);

	ModelInstance GetUnitCube();
	ModelInstance GetUnitPlane();
	bool ModelHasMesh(const std::wstring& someFilePath);
protected:
	std::shared_ptr<Model> GetModelInternal(const std::wstring& someFilePath);
	std::shared_ptr<Model> LoadModel(const std::wstring& someFilePath);
	bool LoadAnimation(const std::wstring& someFilePath, Model& aModel);

private:	
	std::map<std::wstring, Model*> myLoadedModels;
	std::vector<std::shared_ptr<Model>> myModels;	
	static ModelFactory* myInstance;
};

} // namespace Tga2D
