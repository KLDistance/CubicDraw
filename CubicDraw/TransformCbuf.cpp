#include "TransformCbuf.h"

std::unique_ptr<VertexConstantBuffer<TransformCbuf::Transforms>> TransformCbuf::pVcbuf;

TransformCbuf::TransformCbuf(Graphics & gfx, const Drawable & parent, UINT slot)
	: parent(parent)
{
	if (!this->pVcbuf)
	{
		this->pVcbuf = std::make_unique<VertexConstantBuffer<Transforms>>(gfx, slot);
	}
}

void TransformCbuf::Bind(Graphics & gfx)
{
	const auto modelView = parent.GetTransformXM() * gfx.GetCamera();
	const Transforms tf = 
	{
		DirectX::XMMatrixTranspose(modelView),
		DirectX::XMMatrixTranspose(
			modelView * gfx.GetProjection()
		)
	};
	this->pVcbuf->Update(gfx, tf);
	this->pVcbuf->Bind(gfx);
}

