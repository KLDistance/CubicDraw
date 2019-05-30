#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"

template <typename C>
class ConstantBuffer : public Bindable
{
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	UINT slot;
public:
	void Update(Graphics &gfx, const C &consts)
	{
		INFOMAN(gfx);
		
		D3D11_MAPPED_SUBRESOURCE msr;
		GFX_THROW_INFO(
			this->GetContext(gfx)->Map(
				this->pConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD,
				0u, &msr
			)
		);
		memcpy(msr.pData, &consts, sizeof(consts));
		this->GetContext(gfx)->Unmap(this->pConstantBuffer.Get(), 0u);
	}
	ConstantBuffer(Graphics &gfx, const C &consts, UINT slot = 0u)
		: slot(slot)
	{
		INFOMAN(gfx);
		
		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(consts);
		cbd.StructureByteStride = 0u;

		D3D11_SUBRESOURCE_DATA csd = {};
		csd.pSysMem = &consts;
		GFX_THROW_INFO(this->GetDevice(gfx)->CreateBuffer(&cbd, &csd, &this->pConstantBuffer));
	}
	ConstantBuffer(Graphics &gfx, UINT slot = 0u)
		: slot(slot)
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(C);
		cbd.StructureByteStride = 0u;
		GFX_THROW_INFO(this->GetDevice(gfx)->CreateBuffer(&cbd, nullptr, &this->pConstantBuffer));
	}
};

template <typename C>
class VertexConstantBuffer : public ConstantBuffer<C>
{
	using ConstantBuffer<C>::pConstantBuffer;
	using ConstantBuffer<C>::slot;
	using Bindable::GetContext;
public:
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind(Graphics &gfx) override
	{
		this->GetContext(gfx)->VSSetConstantBuffers(this->slot, 1u, this->pConstantBuffer.GetAddressOf());
	}
};

template <typename C>
class PixelConstantBuffer : public ConstantBuffer<C>
{
	using ConstantBuffer<C>::pConstantBuffer;
	using ConstantBuffer<C>::slot;
	using Bindable::GetContext;
public:
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind(Graphics &gfx) override
	{
		this->GetContext(gfx)->PSSetConstantBuffers(this->slot, 1u, this->pConstantBuffer.GetAddressOf());
	}
};