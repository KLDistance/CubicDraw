#pragma once
#include "Graphics.h"
#include "SolidSphere.h"
#include "ConstantBuffers.h"

class PointLight
{
private:
	struct PointLightCBuf
	{
		alignas(16) DirectX::XMFLOAT3 pos;
		alignas(16) DirectX::XMFLOAT3 ambient;
		alignas(16) DirectX::XMFLOAT3 diffuseColor;
		float diffuseIntensity, attConst, attLin, attQuad;
	};
	PointLightCBuf cbData;
	mutable SolidSphere mesh;
	mutable PixelConstantBuffer<PointLightCBuf> cbuf;
	float tmpX = 0, tmpY = 0, tmpZ = 0;
public:
	PointLight(Graphics &gfx, float radius = 0.5f);
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void LightMotion(float dt) noexcept;
	void LightColor(float r, float g, float b) noexcept;
	void Draw(Graphics &gfx) const noexcept;
	void Bind(Graphics &gfx, DirectX::FXMMATRIX view) const noexcept;
};