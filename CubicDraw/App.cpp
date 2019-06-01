#include "App.h"
#include "Box.h"
#include "WinMath.h"
#include "Surface.h"
#include "imgui.h"
#include "GDIPlusManager.h"
#include "Cylinder.h"
#include "Sheet.h"

namespace dx = DirectX;

GDIPlusManager gdipm;

class Factory
{
private:
	Graphics &gfx;
	std::mt19937 rng{ std::random_device{}() };
	std::uniform_int_distribution<int> sdist{ 0, 2 };
	std::uniform_real_distribution<float> adist{ 0.0f, PI * 2.0f };
	std::uniform_real_distribution<float> ddist{ 0.0f, PI * 0.5f };
	std::uniform_real_distribution<float> odist{ 0.0f, PI * 0.08f };
	std::uniform_real_distribution<float> rdist{ 6.0f, 20.0f };
	std::uniform_real_distribution<float> bdist{ 0.4f, 3.0f };
	std::uniform_real_distribution<float> cdist{ 0.0f, 1.0f };
	std::uniform_int_distribution<int> tdist{ 0, 3 };
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
		case 2:
			return std::make_unique<Sheet>(
				this->gfx, rng, adist, ddist, odist, rdist
				);
		default:
			assert(false && "impossible drawable option in factory");
			return {};
		}
	}
};

App::App() : wnd(DISPLAY_SCREENWIDTH, DISPLAY_SCREENHEIGHT, "KLDistance Cubic Mushrooms"), light(wnd.Gfx())
{	
	// load logo1
	D3DX11CreateShaderResourceViewFromFile(this->wnd.Gfx().pDevice.Get(), "Logo\\logo1.png",
		nullptr, nullptr, &this->pLogo1ShaderResourceView, nullptr);
	// load logo2
	D3DX11CreateShaderResourceViewFromFile(this->wnd.Gfx().pDevice.Get(), "Logo\\logo2.png",
		nullptr, nullptr, &this->pLogo2ShaderResourceView, nullptr);
	// load logo3
	D3DX11CreateShaderResourceViewFromFile(this->wnd.Gfx().pDevice.Get(), "Logo\\logo3.png",
		nullptr, nullptr, &this->pLogo3ShaderResourceView, nullptr);
	// load sound
	this->hAudioThread = ::CreateThread(0, 0, App::PlayAudio, this, 0, 0);
}

int App::Go()
{
	// start sub thread for loading resources
	this->hLoadingThread = ::CreateThread(0, 0, App::LoadingThread, this, 0, 0);
	MSG msg = { 0 };
	// refresh all the timers
	this->timer.Mark();
	this->runtimer.Mark();
	// intro loop
	while (true)
	{
		if (const auto ecode = Window::ProcessMessages()) return *ecode;
		if(this->IntroPart()) break;
	}
	// thread joint
	::WaitForSingleObject(this->hLoadingThread, INFINITE);

	// release textures
	this->pLogo1ShaderResourceView->Release();
	this->pLogo2ShaderResourceView->Release();
	this->pLogo3ShaderResourceView->Release();

	::SetEvent(this->hAudioNotifier);
	// refresh all the timers
	this->timer.Mark();
	this->runtimer.Mark();
	// set projection
	this->wnd.Gfx().SetProjection(dx::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
	// lottery loop
	while (true)
	{
		if (const auto ecode = Window::ProcessMessages()) return *ecode;
		this->LotteryPart();
	}
	return 0;
}

int App::IntroPart()
{
	int ret = 0;
	this->wnd.Gfx().BeginFrame(0.0f, 0.0f, 0.0f);
	switch (this->logoState)
	{
	case 0:
	{
		if (ImGui::Begin("", 0, 3))
		{
			ImGui::SetWindowPos({ 0, 0 });
			ImGui::SetWindowSize({ DISPLAY_SCREENWIDTH , DISPLAY_SCREENHEIGHT });
			ImGui::Image(this->pLogo1ShaderResourceView, { DISPLAY_SCREENWIDTH, DISPLAY_SCREENHEIGHT }, { 0, 0 },
				{ 1, 1 }, ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
			ImGui::End();
			if (this->runtimer.Peek() >= logoSuspensionTiming + 3.0f)
			{
				this->logoState = 1;
				this->runtimer.Mark();
			}
		}
		break;
	}
	case 1:
	{
		if (ImGui::Begin("", 0, 3))
		{
			ImGui::SetWindowPos({ 0, 0 });
			ImGui::SetWindowSize({ DISPLAY_SCREENWIDTH , DISPLAY_SCREENHEIGHT });
			ImGui::Image(this->pLogo2ShaderResourceView, { DISPLAY_SCREENWIDTH, DISPLAY_SCREENHEIGHT }, { 0, 0 },
				{ 1, 1 }, ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
			ImGui::End();
			if (this->runtimer.Peek() >= logoSuspensionTiming)
			{
				this->logoState = 2;
				this->runtimer.Mark();
				::SetEvent(this->hAudioNotifier);
			}
		}
		break;
	}
	case 2:
	{
		if (ImGui::Begin("", 0, 3))
		{
			ImGui::SetWindowPos({ 0, 0 });
			ImGui::SetWindowSize({ DISPLAY_SCREENWIDTH , DISPLAY_SCREENHEIGHT });
			ImGui::Image(this->pLogo3ShaderResourceView, { DISPLAY_SCREENWIDTH, DISPLAY_SCREENHEIGHT }, { 0, 0 },
				{ 1, 1 }, ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
			ImGui::End();
			if (this->runtimer.Peek() >= logoSuspensionTiming)
			{
				ret = 1;
				this->audioPick = 2;
			}
		}
		break;
	}
	}
	wnd.Gfx().EndFrame();
	Sleep(16u);
	return ret;
}

void App::LotteryPart()
{
	const auto dt = timer.Mark() * this->speed_factor;

	// switch music on / off
	if (this->wnd.kbd.KeyIsPressed(VK_SHIFT))
	{
		switch (this->audioPick)
		{
		case 0:
			this->audioPick = 2;
			break;
		case 2:
			this->audioPick = 0;
			break;
		}
		this->wnd.kbd.ClearKeyState();
		::SetEvent(this->hAudioNotifier);
	}

	// imgui window to control simulation speed
	switch (this->state)
	{
	case 0:
	{
		this->wnd.Gfx().BeginFrame(0.0f, 0.0f, 0.0f);
		if (this->runtimer.Peek() >= this->transitionBlankTiming)
		{
			this->state = 1;
			this->runtimer.Mark();
		}
		this->expandingImGui->TriggerCenterImGui("", 0);
	}
	case 1:
	{
		this->wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
		this->cam.SetCameraIncrement(dt * 0.7f);
		this->wnd.Gfx().SetCamera(this->cam.GetMatrix());
		this->light.LightMotion(dt);
		this->light.Bind(this->wnd.Gfx(), this->cam.GetMatrix());

		for (auto &d : drawables)
		{
			d->Update(dt);
			d->Draw(this->wnd.Gfx());
		}
		this->light.Draw(this->wnd.Gfx());
		if (this->wnd.kbd.KeyIsPressed(VK_SPACE))
		{
			this->state = 2;
			this->tmpName = this->lottMachine->GetNextName();
			this->wnd.kbd.ClearKeyState();
		}
		this->expandingImGui->TriggerCenterImGui("", 0);
		break;
	}
	case 2:
	{
		this->wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
		this->cam.SetCameraIncrement(dt * 0.7f * this->accelerate);
		this->wnd.Gfx().SetCamera(this->cam.GetMatrix());

		this->light.LightMotion(dt * this->accelerate);
		this->light.Bind(this->wnd.Gfx(), this->cam.GetMatrix());

		for (auto &d : drawables)
		{
			d->Update(dt * this->accelerate);
			d->Draw(this->wnd.Gfx());
		}
		this->light.Draw(this->wnd.Gfx());

		// increment beyond max
		if (this->accelerate > this->accelerateMax)
		{
			this->accelerate = this->accelerateMax + 0.001f; // tolerant error
			this->accelerateIncrement = 0.0f;
		}
		else
		{
			runtimer.Mark();
		}
		if (runtimer.Peek() >= this->ElapseMs)
		{
			this->accelerateIncrement = -this->accelerateIncrementConst;
		}
		// decrement beyond min
		if (this->accelerate <= this->accelerateMin)
		{
			this->accelerateIncrement = this->accelerateIncrementConst;
			this->state = 3;
		}
		this->accelerate += this->accelerateIncrement;
		this->expandingImGui->TriggerCenterImGui("", 0);
		break;
	}
	case 3:
	{
		this->wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
		this->cam.SetCameraIncrement(dt * 0.7f);
		this->wnd.Gfx().SetCamera(this->cam.GetMatrix());
		this->light.LightMotion(dt);
		this->light.Bind(this->wnd.Gfx(), this->cam.GetMatrix());

		for (auto &d : drawables)
		{
			d->Update(dt);
			d->Draw(this->wnd.Gfx());
		}
		this->light.Draw(this->wnd.Gfx());

		// font display
		this->expandingImGui->TriggerCenterImGui(this->tmpName, 1);

		// exit font display
		if (this->wnd.kbd.KeyIsPressed(VK_SPACE))
		{
			this->state = 0;
			this->wnd.kbd.ClearKeyState();
		}
		break;
	}
	}

	wnd.Gfx().EndFrame();
}

DWORD __stdcall App::LoadingThread(LPVOID lpParameters)
{
	App *pApp = (App*)lpParameters;

	// loading dx and objects
	pApp->drawables.reserve(nDrawables);
	std::generate_n(std::back_inserter(pApp->drawables), nDrawables, Factory{ pApp->wnd.Gfx() });
	pApp->wnd.kbd.DisableAutorepeat();
	// loading fonts
	pApp->expandingImGui = new ExpandingImGui();
	// loading lottery
	pApp->lottMachine = new Lottery("WorkBooks\\NameList.xlsx");
	
	return 0;
}

DWORD __stdcall App::PlayAudio(LPVOID lpParameters)
{
	auto pApp = (App*)lpParameters;
	while (true)
	{
		pApp->hAudioNotifier = ::CreateEvent(0, 0, 0, 0);
		::WaitForSingleObject(pApp->hAudioNotifier, INFINITE);
		switch (pApp->audioPick)
		{
		case 0:
		{
			PlaySound(NULL, NULL, SND_ASYNC);
			break;
		}
		case 1:
		{
			PlaySound("Audio\\Logo1.wav", NULL, SND_ASYNC);
			break;
		}
		case 2:
		{
			PlaySound("Audio\\Init1.wav", NULL, SND_ASYNC | SND_LOOP);
			break;
		}
		}
	}
	return 0;
}
