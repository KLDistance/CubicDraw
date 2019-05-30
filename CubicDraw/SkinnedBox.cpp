#include "SkinnedBox.h"
#include "BindableBase.h"
#include "GraphicsThrowMacros.h"
#include "Cube.h"
#include "Surface.h"
#include "Texture.h"

SkinnedBox::SkinnedBox(
	Graphics & gfx, std::mt19937 & rng, 
	std::uniform_real_distribution<float>& adist, 
	std::uniform_real_distribution<float>& ddist, 
	std::uniform_real_distribution<float>& odist, 
	std::uniform_real_distribution<float>& rdist
)
	:
	r(rdist(rng)), droll(ddist(rng)), dpitch(ddist(rng)), dyaw(ddist(rng)),
	dphi(odist(rng)), dtheta(odist(rng)), dchi(odist(rng)), 
	chi(adist(rng)), theta(adist(rng)), phi(adist(rng))
{
	namespace dx = DirectX;
	if (!IsStaticInitialized())
	{
		struct Vertex
		{
			dx::XMFLOAT3 pos;
			struct
			{
				float u, v;
			} tex;
		};
		const auto model = Cube::MakeSkinned<Vertex>();

		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices));
		AddStaticBind(std::make_unique<Texture>(gfx, Surface::FromFile("Image\\cube.png")));

		auto pvs = std::make_unique<VertexShader>(gfx, L"TextureVS.cso");
		auto pvsbc = pvs->GetBytecode();

		AddStaticBind(std::move(pvs));
		AddStaticBind(std::make_unique<PixelShader>(gfx, L"TexturePS.cso"));
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied = 
		{
			{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));
		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}
	AddBind(std::make_unique<TransformCbuf>(gfx, *this));
}

void SkinnedBox::Update(float dt) noexcept
{
	this->roll += this->droll * dt;
	this->pitch += this->dpitch * dt;
	this->yaw += this->dyaw * dt;
	this->theta += this->dtheta * dt;
	this->phi += this->dphi * dt;
	this->chi += this->dchi * dt;
}

DirectX::XMMATRIX SkinnedBox::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	return (
		dx::XMMatrixRotationRollPitchYaw(this->pitch, this->yaw, this->roll) *
		dx::XMMatrixTranslation(this->r, 0.0f, 0.0f) *
		dx::XMMatrixRotationRollPitchYaw(this->theta, this->phi, this->chi)
		);
}