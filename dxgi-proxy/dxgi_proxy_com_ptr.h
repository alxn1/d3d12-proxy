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
	if(obj->QueryInterface(__uuidof(T), &out) != S_OK) {
		out = nullptr;
	}
	return ComPtr<T>(static_cast<T *>(out));
}

template<typename T>
[[nodiscard]] auto addRef(ComPtr<T> &obj) noexcept
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

template<typename T, typename... A>
[[nodiscard]] auto makeObject(A &&...a)
{
	return addRef(new T{std::forward<A>(a)...});
}

template<typename T, typename A1, typename... A>
[[nodiscard]] T *firstOf(const ComPtr<A1> &a1, const ComPtr<A> &...a) noexcept
{
	if constexpr(!sizeof...(A)) {
		return a1.get();
	} else if(a1.get()) {
		return a1.get();
	} else {
		return firstOf<T>(a...);
	}
}

}  // namespace dxgi_proxy
