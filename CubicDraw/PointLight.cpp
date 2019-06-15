#include "PointLight.h"
#include "imgui.h"

PointLight::PointLight(Graphics & gfx, float radius)
	: mesh(gfx, radius), cbuf(gfx)
{
	this->Reset();
}

void PointLight::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Light"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &this->cbData.pos.x, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Y", &this->cbData.pos.y, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Z", &this->cbData.pos.z, -60.0f, 60.0f, "%.1f");

		ImGui::Text("Intensity/Color");
		ImGui::SliderFloat("Intensity", &this->cbData.diffuseIntensity, 0.01f, 2.0f, "%.2f", 2);
		ImGui::ColorEdit3("Diffuse Color", &this->cbData.diffuseColor.x);
		ImGui::ColorEdit3("Ambient", &this->cbData.ambient.x);

		ImGui::Text("Falloff");
		ImGui::SliderFloat("Constant", &this->cbData.attConst, 0.05f, 10.0f, "%.2f", 4);
		ImGui::SliderFloat("Linear", &this->cbData.attLin, 0.0001f, 4.0f, "%.4f", 8);
		ImGui::SliderFloat("Quadratic", &this->cbData.attQuad, 0.0000001f, 10.0f, "%.7f", 10);

		if (ImGui::Button("Reset"))
		{
			this->Reset();
		}
	}
}
/*
this->cbData = {
		{ 0.0f,0.0f,0.0f },
		{ 0.05f,0.05f,0.05f },
		{ 1.0f,1.0f,1.0f },
		1.4f,
		0.9f,
		0.040f,
		0.0045f,
	};
*/
void PointLight::Reset() noexcept
{
	this->cbData = {
		{ 0.0f,0.0f,0.0f },
		{ 0.1f,0.1f,0.1f },
		{ 1.0f,1.0f,1.0f },
		1.4f,
		0.75f,
		0.030f,
		0.0035f,
	};
}

void PointLight::LightMotion(float dt) noexcept
{
	this->cbData.pos.x = sin(this->tmpX += dt) * 4.0f;
	this->cbData.pos.y = cos(this->tmpY += dt) * 3.0f;
}

void PointLight::LightColor(float r, float g, float b) noexcept
{
	this->mesh.SetColor(r, g, b);
}

void PointLight::Draw(Graphics & gfx) const noexcept
{
	this->mesh.SetPos(this->cbData.pos);
	this->mesh.Draw(gfx);
}

void PointLight::Bind(Graphics & gfx, DirectX::FXMMATRIX view) const noexcept
{
	auto dataCopy = this->cbData;
	const auto pos = DirectX::XMLoadFloat3(&this->cbData.pos);
	DirectX::XMStoreFloat3(&dataCopy.pos, DirectX::XMVector3Transform(pos, view));
	this->cbuf.Update(gfx, dataCopy);
	this->cbuf.Bind(gfx);
}
