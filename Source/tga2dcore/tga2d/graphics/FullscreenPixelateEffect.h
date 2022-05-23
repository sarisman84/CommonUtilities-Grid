#pragma once
#include "FullscreenEffect.h"
#include "RenderTarget.h"

class FullscreenPixelateEffect : public Tga2D::FullscreenEffect
{
	float myPixelSize = 16.0f;

	struct EffectBufferData
	{
		float PixelSize;
		Tga2D::Vector2ui Resolution;
		float garbage;
	} myEffectBufferData;

	ComPtr<ID3D11Texture2D> myEffectTexture;
	ComPtr<ID3D11ShaderResourceView> myEffectSRV;
	ComPtr<ID3D11RenderTargetView> myEffectRTV;
	ComPtr<ID3D11Buffer> myEffectBuffer;
	D3D11_VIEWPORT* myViewport;

public:

	void SetPixelSize(float aPixelSize);
	virtual bool Init(const char* aPixelShaderPath) override;
	virtual void Render() override;
	void Activate(Tga2D::DepthBuffer* aDepth = nullptr);
};
