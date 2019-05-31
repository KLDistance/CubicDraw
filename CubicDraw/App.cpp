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
		std::uniform_int_distribution<int> tdist{ 4, 8 };
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
	// start sub thread for loading resources
	this->hLoadingThread = ::CreateThread(0, 0, App::LoadingThread, this, 0, 0);
	MSG msg = { 0 };
	// intro loop
	while (true)
	{
		if (const auto ecode = Window::ProcessMessages()) return *ecode;
		if(this->Intro()) break;
	}
	// thread joint
	::WaitForSingleObject(this->hLoadingThread, INFINITE);
	// lottery loop
	while (true)
	{
		if (const auto ecode = Window::ProcessMessages()) return *ecode;
		this->Lottery();
	}
	return 0;
}

void App::SetKeyState(int input_state)
{
	this->state = input_state;
}

int App::Intro()
{
	return 1;
}

void App::Lottery()
{
	const auto dt = timer.Mark() * this->speed_factor;
	this->wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);

	// imgui window to control simulation speed
	switch (this->state)
	{
	case 0:
	{
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
		break;
	}
	case 1:
	{
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
		break;
	}
	case 2:
	{
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
		if (wnd.Gfx().IsImguiEnabled())
		{
			this->expandingImGui->TriggerCenterImGui(this->nameList[0]);
		}
		break;
	}
	}

	wnd.Gfx().EndFrame();
}

DWORD __stdcall App::LoadingThread(LPVOID lpParameters)
{
	App *pApp = (App*)lpParameters;
	// loading fonts
	pApp->expandingImGui = new ExpandingImGui();
	// loading staff and students
	OpenXLSX::XLDocument xlsxDoc;
	xlsxDoc.OpenDocument("WorkBooks\\NameTest.xlsx");
	auto sheet = xlsxDoc.Workbook().Worksheet("Sheet1");
	size_t rowNum = sheet.RowCount();

	/*
	// create new work book for sequence storage in case of interrupts
	OpenXLSX::XLDocument xlsxDocOutCache;
	xlsxDocOutCache.CreateDocument("NameTest_Cache.xlsx");
	auto sheetCache = xlsxDocOutCache.Workbook().Worksheet("Sheet1");
	*/
	pApp->nameList.resize(rowNum);
	// load sequence
	for (size_t i = 0; i < rowNum; i++)
	{
		std::ostringstream oss;
		oss << "A" << i + 1;
		pApp->nameList[i] = sheet.Cell(oss.str()).Value().Get<std::string>();
	}
	// generate random sequence
	srand(time(nullptr));
	for (int i = rowNum - 1; i >= 0; i--)
	{
		std::swap(pApp->nameList[rand() % (i + 1)], pApp->nameList[i]);
	}
	return 0;
}
