#pragma once
#include "Dependencies.h"
#include "WinException.h"
#include "DxgiInfoManager.h"
#include <d2d1.h>

class Graphics
{
	friend class Bindable;
public:
	class Exception : public WinException
	{
		using WinException::WinException;
	};

	class HrException : public Exception
	{
	public:
		HrException(int line, const char *file, HRESULT hr, std::vector<std::string> infoMsg = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorInfo() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT hr;
		std::string info;
	};

	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	private:
		std::string reason;
	};

	class InfoException : public Exception
	{
	public:
		InfoException(int line, const char *file, std::vector<std::string> infoMsgs) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info;
	};

	Graphics(HWND hwnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics() = default;

	void EndFrame();
	void BeginFrame(float red, float green, float blue) noexcept;
	void ClearBuffer(float red, float green, float blue) noexcept;

	// imgui configurations
	void EnableImgui() noexcept;
	void DisableImgui() noexcept;
	bool IsImguiEnabled() const noexcept;

	// draw actual graphics on screen
	void DrawTestTriangle(float angle, float x, float z);

	// draw with interface design
	void DrawIndexed(UINT count);
	void SetProjection(DirectX::FXMMATRIX proj) noexcept;
	void SetCamera(DirectX::FXMMATRIX cam) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	DirectX::XMMATRIX GetCamera() const noexcept;
private:
	DirectX::XMMATRIX projection;
	DirectX::XMMATRIX camera;
	bool imguiEnabled = true;
#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif
public:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;
};
