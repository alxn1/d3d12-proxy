#pragma once

namespace dxgi_proxy {
namespace impl {

template<typename T>
struct SafeRelease final
{
	void operator()(T *obj) noexcept
	{
		if(obj) {
			obj->Release();
		}
	}
};

}  // namespace impl

template<typename T>
using ComPtr = std::unique_ptr<T, impl::SafeRelease<T>>;

template<typename T, typename U>
[[nodiscard]] auto queryInterface(const ComPtr<U> &obj) noexcept
{
	void *out = nullptr;
	if(obj && obj->QueryInterface(__uuidof(T), &out) != S_OK) {
		out = nullptr;
	}
	return ComPtr<T>(static_cast<T *>(out));
}

template<typename T>
[[nodiscard]] auto addRef(const ComPtr<T> &obj) noexcept
{
	if(obj) {
		obj->AddRef();
	}
	return ComPtr<T>{obj.get()};
}

template<typename T>
[[nodiscard]] auto addRef(T *obj) noexcept
{
	if(obj) {
		obj->AddRef();
	}
	return ComPtr<T>{obj};
}

template<typename T, typename... P>
[[nodiscard]] auto makeComObject(P &&...p)
{
	return addRef(new T{std::forward<P>(p)...});
}

}  // namespace dxgi_proxy
