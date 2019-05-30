#include "InputLayout.h"
#include "GraphicsThrowMacros.h"

InputLayout::InputLayout(Graphics &gfx, const std::vector<D3D11_INPUT_ELEMENT_DESC> &layout, ID3DBlob *pVertexShaderBytecode)
{
	INFOMAN(gfx);

	GFX_THROW_INFO(
		this->GetDevice(gfx)->CreateInputLayout(
			layout.data(), (UINT)layout.size(),
			pVertexShaderBytecode->GetBufferPointer(),
			pVertexShaderBytecode->GetBufferSize(),
			&this->pInputLayout
		)
	);
}

void InputLayout::Bind(Graphics &gfx)
{
	this->GetContext(gfx)->IASetInputLayout(this->pInputLayout.Get());
}