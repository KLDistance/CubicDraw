#include "App.h"
#include "Box.h"
#include "WinMath.h"
#include "Surface.h"
#include "imgui.h"
#include "GDIPlusManager.h"
#include "Cylinder.h"

namespace dx = DirectX;

GDIPlusManager gdipm;

App::App() : wnd(DISPLAY_SCREENWIDTH, DISPLAY_SCREENHEIGHT, "KLDistance Cubic Mushrooms"), light(wnd.Gfx())
{
	class Factory
	{
	private:
		Graphics &gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_int_distribution<int> sdist{ 0,1 };
		std::uniform_real_distribution<float> adist{ 0.0f, PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f, PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f, PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f, 20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f, 3.0f };
		std::uniform_real_distribution<float> cdist{ 0.0f, 1.0f };
		std::uniform_int_distribution<int> tdist{ 4, 4 };
	public:
		Factory(Graphics &gfx) : gfx(gfx) {}
		std::unique_ptr<Drawable> operator()()
		{
			const DirectX::XMFLOAT3 mat = { cdist(rng), cdist(rng), cdist(rng) };
			// for artistic style, use cubes only
			switch (0)
			{
			case 0:
				return std::make_unique<Box>(
					this->gfx, this->rng, this->adist, this->ddist,
					this->odist, this->rdist, this->bdist, mat
					);
			case 1:
				return std::make_unique<Cylinder>(
					this->gfx, this->rng, this->adist, this->ddist, this->odist,
					this->rdist, this->bdist, this->tdist
					);
			default:
				assert(false && "impossible drawable option in factory");
				return {};
			}
		}
	};

	drawables.reserve(nDrawables);
	std::generate_n(std::back_inserter(drawables), nDrawables, Factory{ this->wnd.Gfx() });
	this->wnd.Gfx().SetProjection(dx::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
}

int App::Go()
{
	MSG msg = { 0 };
	while (true)
	{
		if (const auto ecode = Window::ProcessMessages()) return *ecode;
		this->DoFrame();
	}
	return 0;
}

void App::DoFrame()
{
	const auto dt = timer.Mark() * this->speed_factor;
	this->wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	this->cam.SetCameraIncrement(dt * 0.7f);
	this->wnd.Gfx().SetCamera(this->cam.GetMatrix());
	//this->light.LightColor(0.1f, 0.1f, 0.1f);
	this->light.LightMotion(dt);
	this->light.Bind(this->wnd.Gfx(), this->cam.GetMatrix());

	for (auto &d : drawables)
	{
		d->Update(dt);
		d->Draw(this->wnd.Gfx());
	}
	this->light.Draw(this->wnd.Gfx());

	// imgui window to control simulation speed
	if (wnd.Gfx().IsImguiEnabled() && ImGui::Begin("Simulation Speed"))
	{
		ImGui::SliderFloat("Speed Factor", &this->speed_factor, 0.0f, 6.0f, "%.4f", 3.2f);
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Status: %s", wnd.kbd.KeyIsPressed(VK_SPACE) ? "PAUSED" : "RUNNING (hold spacebar to pause)");
		ImGui::End();

		// imgui window to set camera and light
		this->cam.SpawnControlWindow();
		this->light.SpawnControlWindow();
	}


	wnd.Gfx().EndFrame();
}