#include "pch.h"

DXGI_P_LOG_SYSTEM(DXGI_PROXY);

using namespace dxgi_proxy;

namespace {

std::unique_ptr<Config> config;
HRESULT(WINAPI *create_factory)(REFIID, void **) = nullptr;
HRESULT(WINAPI *create_factory1)(REFIID, void **) = nullptr;
HRESULT(WINAPI *create_factory2)(UINT, REFIID, void **) = nullptr;
HRESULT(WINAPI *declare_adapter_removal_support)() = nullptr;
HRESULT(WINAPI *get_debug_interface1)(UINT, REFIID, void **) = nullptr;

template<typename T>
[[nodiscard]] T getProcAddress(HMODULE m, const char *name)
{
	return reinterpret_cast<T>(GetProcAddress(m, name));
}

void initialize()
{
	static std::once_flag g_init_flag;

	std::call_once(g_init_flag, [] {
		config.reset(new Config{});
		initLog(*config);
		DXGI_P_LOGF("config:\n%s", config->toString().data());
		DXGI_P_LOG("initialization...");

		const auto &dxgi = config->dxgiSection();
		if(const auto m = LoadLibrary(dxgi.dll_path.data())) {
			create_factory = getProcAddress<decltype(create_factory)>(m, "CreateDXGIFactory");
			create_factory1 = getProcAddress<decltype(create_factory1)>(m, "CreateDXGIFactory1");
			create_factory2 = getProcAddress<decltype(create_factory2)>(m, "CreateDXGIFactory2");
			get_debug_interface1 = getProcAddress<decltype(get_debug_interface1)>(m, "DXGIGetDebugInterface1");
			declare_adapter_removal_support =
			    getProcAddress<decltype(declare_adapter_removal_support)>(m, "DXGIDeclareAdapterRemovalSupport");
		} else {
			DXGI_P_LOGF("ERROR: can't load proxied dxgi dll at path: %s", dxgi.dll_path.data());
		}

		DXGI_P_LOG("initialization complete:");
		DXGI_P_LOGF("\tCreateDXGIFactory: %p", create_factory);
		DXGI_P_LOGF("\tCreateDXGIFactory1: %p", create_factory1);
		DXGI_P_LOGF("\tCreateDXGIFactory2: %p", create_factory2);
		DXGI_P_LOGF("\tDXGIDeclareAdapterRemovalSupport: %p", declare_adapter_removal_support);
		DXGI_P_LOGF("\tDXGIGetDebugInterface1: %p", get_debug_interface1);
	});
}

}  // namespace

extern "C"
{

HRESULT WINAPI CreateDXGIFactory(REFIID riid, void **out)
{
	initialize();
	DXGI_P_LOGF("CreateDXGIFactory call with '%s' riid", format(riid).data());
	auto hr = create_factory ? create_factory(riid, out) : E_NOINTERFACE;
	if(out && hr == S_OK) {
		hr = Factory::wrap(*config, riid, *out, out);
	}
	DXGI_P_LOGF("CreateDXGIFactory call result: 0x%X", hr);
	return hr;
}

HRESULT WINAPI CreateDXGIFactory1(REFIID riid, void **out)
{
	initialize();
	DXGI_P_LOGF("CreateDXGIFactory1 call with '%s' riid", format(riid).data());
	auto hr = create_factory1 ? create_factory1(riid, out) : E_NOINTERFACE;
	if(out && hr == S_OK) {
		hr = Factory::wrap(*config, riid, *out, out);
	}
	DXGI_P_LOGF("CreateDXGIFactory1 call result: 0x%X", hr);
	return hr;
}

HRESULT WINAPI CreateDXGIFactory2(UINT flags, REFIID riid, void **out)
{
	initialize();
	DXGI_P_LOGF("CreateDXGIFactory2 call with '%s' riid", format(riid).data());
	auto hr = create_factory2 ? create_factory2(flags, riid, out) : E_NOINTERFACE;
	if(out && hr == S_OK) {
		hr = Factory::wrap(*config, riid, *out, out);
	}
	DXGI_P_LOGF("CreateDXGIFactory2 call result: 0x%X", hr);
	return hr;
}

HRESULT WINAPI DXGIDeclareAdapterRemovalSupport()
{
	initialize();
	DXGI_P_LOG("DXGIDeclareAdapterRemovalSupport call");
	const auto hr = declare_adapter_removal_support ? declare_adapter_removal_support() : E_NOINTERFACE;
	DXGI_P_LOGF("DXGIDeclareAdapterRemovalSupport call result: 0x%X", hr);
	return hr;
}

HRESULT WINAPI DXGIGetDebugInterface1(UINT flags, REFIID riid, void **out)
{
	initialize();
	DXGI_P_LOGF("DXGIGetDebugInterface1 call with 0x%X flags and '%s' riid", flags, format(riid).data());
	const auto hr = get_debug_interface1 ? get_debug_interface1(flags, riid, out) : E_NOINTERFACE;
	DXGI_P_LOGF("DXGIGetDebugInterface1 call result: 0x%X", hr);
	return hr;
}

}  // extern "C"