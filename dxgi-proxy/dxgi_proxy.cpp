#include "pch.h"

DXGI_P_LOG_SYSTEM(PROXY);

namespace dxgi_proxy {

const Proxy &Proxy::instance()
{
	static const Proxy sProxy;
	return sProxy;
}

HRESULT WINAPI Proxy::create_factory(REFIID riid, void **out) const
{
	DXGI_P_LOGF("CreateDXGIFactory call with '%s' riid", log::format(riid).data());
	auto hr = create_factory_fn ? create_factory_fn(riid, out) : E_NOINTERFACE;
	if(out && hr == S_OK) {
		hr = wrapFactory(riid, out);
	}
	DXGI_P_LOGF("CreateDXGIFactory call result: 0x%X", hr);
	return hr;
}

HRESULT WINAPI Proxy::create_factory1(REFIID riid, void **out) const
{
	DXGI_P_LOGF("CreateDXGIFactory1 call with '%s' riid", log::format(riid).data());
	auto hr = create_factory1_fn ? create_factory1_fn(riid, out) : E_NOINTERFACE;
	if(out && hr == S_OK) {
		hr = wrapFactory(riid, out);
	}
	DXGI_P_LOGF("CreateDXGIFactory1 call result: 0x%X", hr);
	return hr;
}

HRESULT WINAPI Proxy::create_factory2(UINT flags, REFIID riid, void **out) const
{
	DXGI_P_LOGF("CreateDXGIFactory2 call with '%s' riid", log::format(riid).data());
	auto hr = create_factory2_fn ? create_factory2_fn(flags, riid, out) : E_NOINTERFACE;
	if(out && hr == S_OK) {
		hr = wrapFactory(riid, out);
	}
	DXGI_P_LOGF("CreateDXGIFactory2 call result: 0x%X", hr);
	return hr;
}

HRESULT WINAPI Proxy::declare_adapter_removal_support() const
{
	DXGI_P_LOG("DXGIDeclareAdapterRemovalSupport call");
	const auto hr = declare_adapter_removal_support_fn ? declare_adapter_removal_support_fn() : E_NOINTERFACE;
	DXGI_P_LOGF("DXGIDeclareAdapterRemovalSupport call result: 0x%X", hr);
	return hr;
}

HRESULT WINAPI Proxy::get_debug_interface1(UINT flags, REFIID riid, void **out) const
{
	DXGI_P_LOGF("DXGIGetDebugInterface1 call with 0x%X flags and '%s' riid", flags, log::format(riid).data());
	const auto hr = get_debug_interface1_fn ? get_debug_interface1_fn(flags, riid, out) : E_NOINTERFACE;
	DXGI_P_LOGF("DXGIGetDebugInterface1 call result: 0x%X", hr);
	return hr;
}

HRESULT Proxy::wrapFactory(REFIID riid, void **in_out) const
{
	if(config.dxgiSection().enable_factory_proxy) {
		return makeComObject<Factory>(config, ComPtr<IUnknown>{static_cast<IUnknown *>(*in_out)})->QueryInterface(riid, in_out);
	}
	return S_OK;
}

Proxy::Proxy()
{
	static const auto get_proc_addr = [](HMODULE m, const char *name, auto &out) {
		out = reinterpret_cast<std::decay_t<decltype(out)>>(GetProcAddress(m, name));
		DXGI_P_LOGF("\t%s: 0x%p", name, out);
	};

	log::init(config);
	DXGI_P_LOGF("config:\n%s\n", config.toString().data());

	const auto &dxgi = config.dxgiSection();
	if(const auto m = LoadLibrary(dxgi.dll_path.data())) {
		get_proc_addr(m, "CreateDXGIFactory", create_factory_fn);
		get_proc_addr(m, "CreateDXGIFactory1", create_factory1_fn);
		get_proc_addr(m, "CreateDXGIFactory2", create_factory2_fn);
		get_proc_addr(m, "DXGIGetDebugInterface1", get_debug_interface1_fn);
		get_proc_addr(m, "DXGIDeclareAdapterRemovalSupport", declare_adapter_removal_support_fn);
	} else {
		DXGI_P_LOGF("ERROR: can't load proxied dxgi dll at path: %s", dxgi.dll_path.data());
	}
}

}  // namespace dxgi_proxy
