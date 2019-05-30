#include "VertexBuffer.h"

void VertexBuffer::Bind(Graphics &gfx)
{
	const UINT offset = 0u;
	this->GetContext(gfx)->IASetVertexBuffers(0u, 1u, this->pVertexBuffer.GetAddressOf(), &this->stride, &offset);
}