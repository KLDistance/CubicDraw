#include "VertexShader.h"
#include "GraphicsThrowMacros.h"

VertexShader::VertexShader(Graphics &gfx, const std::wstring &path)
{
	INFOMAN(gfx);

	GFX_THROW_INFO(D3DReadFileToBlob(path.c_str(), &this->pBytecodeBlob));
	GFX_THROW_INFO(
		this->GetDevice(gfx)->CreateVertexShader(
			this->pBytecodeBlob->GetBufferPointer(),
			this->pBytecodeBlob->GetBufferSize(),
			nullptr, &this->pVertexShader
		)
	);
}

void VertexShader::Bind(Graphics &gfx)
{
	this->GetContext(gfx)->VSSetShader(this->pVertexShader.Get(), nullptr, 0u);
}

ID3DBlob* VertexShader::GetBytecode() const noexcept
{
	return this->pBytecodeBlob.Get();
}