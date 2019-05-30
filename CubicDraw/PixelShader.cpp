#include "PixelShader.h"
#include "GraphicsThrowMacros.h"

PixelShader::PixelShader(Graphics &gfx, const std::wstring &path)
{
	INFOMAN(gfx);

	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO(D3DReadFileToBlob(path.c_str(), &pBlob));
	GFX_THROW_INFO(this->GetDevice(gfx)->CreatePixelShader(
		pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &this->pPixelShader
	));
}

void PixelShader::Bind(Graphics &gfx)
{
	this->GetContext(gfx)->PSSetShader(this->pPixelShader.Get(), nullptr, 0u);
}