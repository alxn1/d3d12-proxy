#pragma once

namespace dxgi_proxy {

class Proxy final
{
public:
	[[nodiscard]] static const Proxy &instance();

	Proxy(const Proxy &) = delete;
	Proxy &operator=(const Proxy &) = delete;

	[[nodiscard]] HRESULT WINAPI create_factory(REFIID riid, void **out) const;
	[[nodiscard]] HRESULT WINAPI create_factory1(REFIID riid, void **out) const;
	[[nodiscard]] HRESULT WINAPI create_factory2(UINT flags, REFIID riid, void **out) const;
	[[nodiscard]] HRESULT WINAPI declare_adapter_removal_support() const;
	[[nodiscard]] HRESULT WINAPI get_debug_interface1(UINT flags, REFIID riid, void **out) const;

private:
	const Config config;
	HRESULT(WINAPI *create_factory_fn)(REFIID, void **) = nullptr;
	HRESULT(WINAPI *create_factory1_fn)(REFIID, void **) = nullptr;
	HRESULT(WINAPI *create_factory2_fn)(UINT, REFIID, void **) = nullptr;
	HRESULT(WINAPI *declare_adapter_removal_support_fn)() = nullptr;
	HRESULT(WINAPI *get_debug_interface1_fn)(UINT, REFIID, void **) = nullptr;

	[[nodiscard]] HRESULT wrapFactory(REFIID riid, void **in_out) const;

	Proxy();
};

}  // namespace dxgi_proxy
