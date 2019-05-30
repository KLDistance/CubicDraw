#pragma once
#include "Drawable.h"
#include "IndexBuffer.h"

template <class T>
class DrawableBase : public Drawable
{
private:
	static std::vector<std::unique_ptr<Bindable>> staticBinds;
protected:
	static bool IsStaticInitialized() noexcept
	{
		return !staticBinds.empty();
	}
	static void AddStaticBind(std::unique_ptr<Bindable> bind) noexcept
	{
		assert("Must use AddStaticIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
		staticBinds.push_back(std::move(bind));
	}
	void AddStaticIndexBuffer(std::unique_ptr<IndexBuffer> ibuf) noexcept
	{
		assert("Attempting to add index buffer a second time" && this->pIndexBuffer == nullptr);
		this->pIndexBuffer = ibuf.get();
		staticBinds.push_back(std::move(ibuf));
	}
	void SetIndexFromStatic() noexcept
	{
		assert("Attempting to add index buffer a second time" && this->pIndexBuffer == nullptr);
		for (const auto &b : staticBinds)
		{
			if (const auto p = dynamic_cast<IndexBuffer*>(b.get()))
			{
				this->pIndexBuffer = p;
				return;
			}
		}
		assert("Failed to find index buffer in static binds" && this->pIndexBuffer != nullptr);
	}
private:
	const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept override
	{
		return staticBinds;
	}
};

template <class T>
std::vector<std::unique_ptr<Bindable>> DrawableBase<T>::staticBinds;