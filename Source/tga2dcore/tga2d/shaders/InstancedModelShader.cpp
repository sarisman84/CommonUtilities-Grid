#include "stdafx.h"
#include "InstancedModelShader.h"

#include "tga2d/graphics/DX11.h"
#include "tga2d/graphics/Vertex.h"
#include "tga2d/model/ModelInstancer.h"

Tga2D::InstancedModelShader::InstancedModelShader(Engine* anEngine)
	: Shader(anEngine)
{
}

Tga2D::InstancedModelShader::~InstancedModelShader()
{
}

bool Tga2D::InstancedModelShader::Init()
{
	return Init(L"shaders/instanced_model_shader_VS.cso", L"shaders/instanced_model_shader_PS.cso");
}

bool Tga2D::InstancedModelShader::Init(const wchar_t* aVertexShaderFile, const wchar_t* aPixelShaderFile)
{
	return Shader::CreateShaders(aVertexShaderFile, aPixelShaderFile, nullptr);
}

void Tga2D::InstancedModelShader::Render(const Tga2D::RenderObjectSharedData& sharedData,
     const Tga2D::TextureResource* const* someTextures, const Tga2D::Model::MeshData& aModelData, const Tga2D::Matrix4x4f&
     aObToWorld,
     const Tga2D::ModelInstancer& aModelInstancer, const Tga2D::Matrix4x4f* someBones)
{
#pragma message("Spaghetti Mode Go!")

	UNREFERENCED_PARAMETER(aObToWorld);
	UNREFERENCED_PARAMETER(someBones);
	UNREFERENCED_PARAMETER(aModelData);

	{
		ID3D11ShaderResourceView* resourceViews[4];
		int i = 0;
		while (i < 4 && someTextures[i] != nullptr)
		{
			resourceViews[i] = someTextures[i]->GetShaderResourceView();
			i++;
		}

		DX11::Context->PSSetShaderResources(4, i, resourceViews);
	}

	if (!myIsReadyToRender)
	{
		return;
	}

	Shader::PrepareRender(sharedData);

	ID3D11Buffer* mdlBuffers[2]{ nullptr, nullptr };
	mdlBuffers[1] = aModelInstancer.GetInstanceBuffer();
	UINT strides[2] = { sizeof(Vertex), sizeof(ModelInstancer::InstanceBufferData)};
	UINT offsets[2] = { 0, 0};

	const std::shared_ptr<Model> model = aModelInstancer.myModel;
	const std::vector<Model::MeshData>& meshDataList = model->GetMeshDataList();
	const size_t meshCount = model->GetMeshCount();

	for (int j = 0; j < meshCount; j++)
	{
		const Model::MeshData& meshData = meshDataList[j];
		
		mdlBuffers[0] = meshData.myVertexBuffer;

		DX11::Context->IASetVertexBuffers(0, 2, mdlBuffers, strides, offsets);
		DX11::Context->IASetIndexBuffer(meshData.myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		DX11::Context->VSSetConstantBuffers((int)ConstantBufferSlot::Object, 1, &myObjectBuffer);
		DX11::Context->PSSetConstantBuffers((int)ConstantBufferSlot::Object, 1, &myObjectBuffer);

		{
			const TextureResource* const* textures = aModelInstancer.GetTextures(j);
			ID3D11ShaderResourceView* resourceViews[4];
			int i = 0;
			while (i < 4 && textures[i] != nullptr)
			{
				resourceViews[i] = textures[i]->GetShaderResourceView();
				i++;
			}

			DX11::Context->PSSetShaderResources(1, i, resourceViews);
		}

		DX11::LogDrawCall();
		DX11::Context->DrawIndexedInstanced(meshData.myNumberOfIndices, aModelInstancer.myBufferNumInstances, 0, 0, 0);
	}
}

bool Tga2D::InstancedModelShader::CreateInputLayout(const std::string& aVS)
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"UV", 1, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"UV", 2, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"UV", 3, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BONES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{"WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{"WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{"WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};

	unsigned int numElements = sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	HRESULT result = DX11::Device->CreateInputLayout(&layout[0], numElements, aVS.data(), aVS.size(), myLayout.ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		ERROR_PRINT("%s", "Layout error");
	}
	return true;
}
