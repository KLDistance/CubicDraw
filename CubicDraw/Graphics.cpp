#include "Graphics.h"
#include "GraphicsThrowMacros.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

Graphics::Graphics(HWND hwnd)
{
	// swap chain descriptor
	DXGI_SWAP_CHAIN_DESC sd = { 0 };
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hwnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	// macros defined swap create flages
	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// create device, front/back buffers, swap chain and rendering context
	HRESULT hr;
	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&this->pSwap,
		&this->pDevice,
		nullptr,
		&this->pContext
	));
	// gain access to texture subresource in swapchain (back buffer)
	Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;
	GFX_THROW_INFO(this->pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	// create render target view
	GFX_THROW_INFO(this->pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &this->pTarget));
	
	// create depth stencil state
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDSState;
	GFX_THROW_INFO(this->pDevice->CreateDepthStencilState(&dsDesc, &pDSState));

	// bind depth state
	this->pContext->OMSetDepthStencilState(pDSState.Get(), 1u);

	// create depth stencil texture
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil;
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = DISPLAY_SCREENWIDTH;
	descDepth.Height = DISPLAY_SCREENHEIGHT;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	GFX_THROW_INFO(this->pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil));

	// create view of depth stencil texture
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;
	GFX_THROW_INFO(this->pDevice->CreateDepthStencilView(pDepthStencil.Get(), &descDSV, &this->pDSV));

	// bind depth stencil view to OM
	this->pContext->OMSetRenderTargets(1u, this->pTarget.GetAddressOf(), this->pDSV.Get());

	// configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = (float)DISPLAY_SCREENWIDTH;
	vp.Height = (float)DISPLAY_SCREENHEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	this->pContext->RSSetViewports(1u, &vp);

	// init imgui d3d impl
	ImGui_ImplDX11_Init(this->pDevice.Get(), this->pContext.Get());
}

void Graphics::EndFrame()
{
	// imgui frame end
	if (this->imguiEnabled)
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	HRESULT hr;
#ifndef NDEBUG
	this->infoManager.Set();
#endif
	if (FAILED(hr = this->pSwap->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED) throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		else GFX_THROW_INFO(hr);
	}
}

void Graphics::BeginFrame(float red, float green, float blue) noexcept
{
	// imgui begin frame
	if (this->imguiEnabled)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	const float color[] = { red, green, blue, 1.0f };
	this->pContext->ClearRenderTargetView(this->pTarget.Get(), color);
	this->pContext->ClearDepthStencilView(this->pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
	const float color[] = { red, green, blue, 1.0f };
	this->pContext->ClearRenderTargetView(this->pTarget.Get(), color);
	this->pContext->ClearDepthStencilView(this->pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void Graphics::EnableImgui() noexcept
{
	this->imguiEnabled = true;
}

void Graphics::DisableImgui() noexcept
{
	this->imguiEnabled = false;
}

bool Graphics::IsImguiEnabled() const noexcept
{
	return this->imguiEnabled;
}

// test function for graphics of triangle
void Graphics::DrawTestTriangle(float angle, float x, float z)
{
	struct Vertex
	{
		struct
		{
			float x, y, z;
		} pos;
	};

	// create vertex buffer (1,2d triangle at center of screen
	/*
	Vertex vertices[] = 
	{
		{0.0f, 0.5f, 255u, 0u, 0u, 0u},
		{0.5f, -0.5f, 0u, 255u, 0u, 0u},
		{-0.5f, -0.5f, 0u, 0u, 255u, 0u},
		{-0.3f, 0.3f, 0u, 255u, 0u, 0u},
		{0.3f, 0.3f, 0u, 0u, 255u, 0u},
		{0.0f, -1.0f, 255u, 0u, 0u, 0u}
	};
	*/
	Vertex vertices[] = 
	{
		{-1.0f, -1.0f,-1.0f},
		{1.0f, -1.0f, -1.0f},
		{-1.0f, 1.0f, -1.0f},
		{1.0f, 1.0f, -1.0f},
		{-1.0f, -1.0f, 1.0f},
		{1.0f, -1.0f, 1.0f},
		{-1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f}
	};

	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	D3D11_BUFFER_DESC bd = { 0 };
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeof(vertices);
	bd.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA sd = { 0 };
	sd.pSysMem = vertices;

	GFX_THROW_INFO(this->pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));
	
	// indexed buffer
	/*
	const unsigned short indices[] = 
	{
		0, 1, 2,
		0, 2, 3,
		0, 4, 1,
		2, 1, 5
	};
	*/
	const unsigned short indices[] =
	{
		0,2,1,2,3,1,
		1,3,5,3,7,5,
		2,6,3,3,6,7,
		4,5,7,4,7,6,
		0,4,2,2,4,6,
		0,1,4,1,5,4
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = sizeof(indices);
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices;
	GFX_THROW_INFO(this->pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer));
	
	// bind vertex buffer to pipeline
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	this->pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);
	
	// bind index buffer to pipeline
	this->pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

	// create constant buffer for transformaion matrix
	struct ConstantBuffer
	{
		DirectX::XMMATRIX transform;
	};

	const ConstantBuffer cb =
	{
		{
			DirectX::XMMatrixTranspose(
				DirectX::XMMatrixRotationZ(angle) *
				DirectX::XMMatrixRotationX(angle) *
				DirectX::XMMatrixTranslation(x, 0, z * 4 + 4.0f) *
				DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 10.0f)
			)
		}
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(cb);
	cbd.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd = {};
	csd.pSysMem = &cb;
	GFX_THROW_INFO(this->pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer));

	// bind constant buffer to vertex shader
	this->pContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());

	struct ConstantBuffer2
	{
		struct
		{
			float r, g, b, a;
		} face_colors[6];
	};
	const ConstantBuffer2 cb2 = 
	{
		{
			{1.0f, 0.0f, 1.0f},
			{1.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f},
			{0.0f, 0.0f, 1.0f},
			{1.0f, 1.0f, 0.0f},
			{0.0f, 1.0f, 1.0f}
		}
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer2;
	D3D11_BUFFER_DESC cbd2;
	cbd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd2.Usage = D3D11_USAGE_DEFAULT;
	cbd2.CPUAccessFlags = 0u;
	cbd2.MiscFlags = 0u;
	cbd2.ByteWidth = sizeof(cb2);
	cbd2.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd2 = {};
	csd2.pSysMem = &cb2;
	GFX_THROW_INFO(this->pDevice->CreateBuffer(&cbd2, &csd2, &pConstantBuffer2));

	// bind constant buffer to pixel shader
	this->pContext->PSSetConstantBuffers(0u, 1u, pConstantBuffer2.GetAddressOf());

	// input (vertex) layout (2d position only)
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;

	// create pixel shader
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &pBlob));
	GFX_THROW_INFO(this->pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));

	// bind pixel shader
	this->pContext->PSSetShader(pPixelShader.Get(), 0, 0);

	// create vertex shader
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &pBlob));
	GFX_THROW_INFO(this->pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));

	// bind vertex shader
	this->pContext->VSSetShader(pVertexShader.Get(), 0, 0);

	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	GFX_THROW_INFO(
		pDevice->CreateInputLayout(
			ied, (UINT)std::size(ied), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout
			)
	);

	// bind vertex layout
	this->pContext->IASetInputLayout(pInputLayout.Get());

	// bind render target (done in constructor)
	//this->pContext->OMSetRenderTargets(1u, this->pTarget.GetAddressOf(), nullptr);

	// Set primitive topology to triangle list (groups of 3 vertices)
	this->pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = 800;
	vp.Height = 600;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	this->pContext->RSSetViewports(1u, &vp);

	GFX_THROW_INFO_ONLY(this->pContext->DrawIndexed(std::size(indices), 0u, 0u));
}

void Graphics::DrawIndexed(UINT count)
{
	GFX_THROW_INFO_ONLY(this->pContext->DrawIndexed(count, 0u, 0u));
}

void Graphics::SetProjection(DirectX::FXMMATRIX proj) noexcept
{
	this->projection = proj;
}

void Graphics::SetCamera(DirectX::FXMMATRIX cam) noexcept
{
	this->camera = cam;
}

DirectX::XMMATRIX Graphics::GetProjection() const noexcept
{
	return this->projection;
}

DirectX::XMMATRIX Graphics::GetCamera() const noexcept
{
	return this->camera;
}

// graphics exception
Graphics::HrException::HrException(int line, const char *file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
	: Exception(line, file), hr(hr)
{
	// join all info messages with newlines into single string
	for (const auto& m : infoMsgs)
	{
		this->info += m;
		this->info.push_back('\n');
	}
	// remove final newline if exists
	if (!this->info.empty()) this->info.pop_back();
}

const char* Graphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << this->GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << this->GetErrorCode()
		<< std::dec << " (" << (unsigned long)this->GetErrorCode() << ")" << std::endl
		<< "[Error String] " << this->GetErrorString() << std::endl
		<< "[Description]" << this->GetErrorDescription() << std::endl;
	if (!this->info.empty()) oss << "\n[Error Info]\n" << this->GetErrorInfo() << std::endl << std::endl;
	oss << this->GetOriginString();
	this->whatBuffer = oss.str();
	return this->whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const noexcept
{
	return "KLDistance Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return this->hr;
}

std::string Graphics::HrException::GetErrorInfo() const noexcept
{
	return this->info;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
	return DXGetErrorString(this->hr);
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	char buf[512];
	DXGetErrorDescription(hr, buf, sizeof(buf));
	return buf;
}

const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "KLDistance Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}

Graphics::InfoException::InfoException(int line, const char *file, std::vector<std::string> infoMsgs) noexcept
	: Exception(line, file)
{
	// join all info messages with newlines into single string
	for (const auto& m : infoMsgs)
	{
		this->info += m;
		this->info.push_back('\n');
	}
	// remove final newline if exists
	if (!this->info.empty())
	{
		this->info.pop_back();
	}
}

const char* Graphics::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << this->GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo()
		<< std::endl << std::endl;
	oss << GetOriginString();
	this->whatBuffer = oss.str();
	return this->whatBuffer.c_str();
}

const char* Graphics::InfoException::GetType() const noexcept
{
	return "KLDistance Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
	return this->info;
}