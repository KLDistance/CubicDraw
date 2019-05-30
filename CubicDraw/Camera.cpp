#include "Camera.h"
#include "imgui.h"
#include "WinMath.h"

namespace dx = DirectX;

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	const auto pos = dx::XMVector3Transform(
		dx::XMVectorSet(0.0f, 0.0f, -this->r, 0.0f), 
		dx::XMMatrixRotationRollPitchYaw(sin(this->phi + PI / 2) * 0.2f, -sin(this->theta) * 0.2f, 0.0f)
	);
	return dx::XMMatrixLookAtLH(
		pos, dx::XMVectorZero(),
		dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
	) * dx::XMMatrixRotationRollPitchYaw(
		this->pitch, -this->yaw, this->roll
	);
}

void Camera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Camera"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("R", &this->r, 0.0f, 80.0f, "%.1f");
		ImGui::SliderAngle("Theta", &theta, -180.0f, 180.0f);
		ImGui::SliderAngle("Phi", &phi, -89.0f, 89.0f);
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void Camera::SetCameraIncrement(float dt) noexcept
{
	this->r = 20.0f;
	this->theta = this->theta + dt;
	this->phi = this->phi + dt;
	this->pitch = 0.0f;
	this->yaw = 0.0f;
	this->roll = 0.0f;
}

void Camera::Reset() noexcept
{
	this->r = 20.0f;
	this->theta = 0.0f;
	this->phi = 0.0f;
	this->pitch = 0.0f;
	this->yaw = 0.0f;
	this->roll = 0.0f;
}
