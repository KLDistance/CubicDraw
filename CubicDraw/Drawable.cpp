#include "Drawable.h"
#include "GraphicsThrowMacros.h"
#include "IndexBuffer.h"
#include <cassert>
#include <typeinfo>

void Drawable::Draw(Graphics &gfx) const noexcept
{
	for (auto &b : binds)
	{
		b->Bind(gfx);
	}
	for (auto &b : GetStaticBinds())
	{
		b->Bind(gfx);
	}
	gfx.DrawIndexed(this->pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::unique_ptr<Bindable> bind) noexcept
{
	assert("Must use AddIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
	binds.push_back(std::move(bind));
}

void Drawable::AddIndexBuffer(std::unique_ptr<IndexBuffer> ibuf) noexcept
{
	assert("Attempting to add index buffer a second time" && this->pIndexBuffer == nullptr);
	this->pIndexBuffer = ibuf.get();
	binds.push_back(std::move(ibuf));
}