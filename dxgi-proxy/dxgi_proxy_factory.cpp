#include "pch.h"

DXGI_P_LOG_SYSTEM(FACTORY);

namespace dxgi_proxy {
namespace {

class FactoryStub final : public Object<IDXGIFactory7>
{
public:
	[[nodiscard]] static FactoryStub &instance()
	{
		static FactoryStub sInstance;
		return sInstance;
	}

	HRESULT DXGI_P_API QueryInterface(REFIID riid, void **out) override { return E_NOINTERFACE; }
	HRESULT DXGI_P_API GetParent(REFIID riid, void **out) override { return E_NOINTERFACE; }
	HRESULT DXGI_P_API GetPrivateData(REFGUID riid, UINT *size, void *data) override { return E_NOTIMPL; }
	HRESULT DXGI_P_API SetPrivateData(REFGUID riid, UINT size, const void *data) override { return E_NOTIMPL; }
	HRESULT DXGI_P_API SetPrivateDataInterface(REFGUID riid, const IUnknown *obj) override { return E_NOTIMPL; }
	HRESULT DXGI_P_API EnumAdapters(UINT adapter, IDXGIAdapter **out) override { return E_NOTIMPL; }
	HRESULT DXGI_P_API MakeWindowAssociation(HWND hwnd, UINT flags) override { return E_NOTIMPL; }
	HRESULT DXGI_P_API GetWindowAssociation(HWND *out) override { return E_NOTIMPL; }
	HRESULT DXGI_P_API CreateSwapChain(IUnknown *device, DXGI_SWAP_CHAIN_DESC *desc, IDXGISwapChain **out) override { return E_NOTIMPL; }
	HRESULT DXGI_P_API CreateSoftwareAdapter(HMODULE m, IDXGIAdapter **out) override { return E_NOTIMPL; }
	HRESULT DXGI_P_API EnumAdapters1(UINT adapter, IDXGIAdapter1 **out) override { return E_NOTIMPL; }
	BOOL DXGI_P_API IsCurrent() override { return FALSE; }
	BOOL DXGI_P_API IsWindowedStereoEnabled() override { return FALSE; }
	HRESULT DXGI_P_API GetSharedResourceAdapterLuid(HANDLE resource, LUID *out) override { return E_NOTIMPL; }
	HRESULT DXGI_P_API RegisterStereoStatusWindow(HWND hwnd, UINT msg, DWORD *cookie) override { return E_NOTIMPL; }
	HRESULT DXGI_P_API RegisterStereoStatusEvent(HANDLE event, DWORD *cookie) override { return E_NOTIMPL; }
	void DXGI_P_API UnregisterStereoStatus(DWORD cookie) override {}
	HRESULT DXGI_P_API RegisterOcclusionStatusWindow(HWND hwnd, UINT msg, DWORD *cookie) override { return E_NOTIMPL; }
	HRESULT DXGI_P_API RegisterOcclusionStatusEvent(HANDLE event, DWORD *cookie) override { return E_NOTIMPL; }
	void DXGI_P_API UnregisterOcclusionStatus(DWORD cookie) override {}
	UINT DXGI_P_API GetCreationFlags() override { return 0; }
	HRESULT DXGI_P_API EnumAdapterByLuid(LUID luid, REFIID riid, void **out) override { return E_NOTIMPL; }
	HRESULT DXGI_P_API EnumWarpAdapter(REFIID riid, void **out) override { return E_NOTIMPL; }
	HRESULT DXGI_P_API CheckFeatureSupport(DXGI_FEATURE feature, void *data, UINT size) override { return E_NOTIMPL; }
	HRESULT DXGI_P_API EnumAdapterByGpuPreference(UINT adapter, DXGI_GPU_PREFERENCE preference, REFIID riid, void **out) override { return E_NOTIMPL; }
	HRESULT DXGI_P_API RegisterAdaptersChangedEvent(HANDLE event, DWORD *cookie) override { return E_NOTIMPL; }
	HRESULT DXGI_P_API UnregisterAdaptersChangedEvent(DWORD cookie) override { return E_NOTIMPL; }

	HRESULT DXGI_P_API CreateSwapChainForHwnd(IUnknown *device,
	                                          HWND hwnd,
	                                          const DXGI_SWAP_CHAIN_DESC1 *desc,
	                                          const DXGI_SWAP_CHAIN_FULLSCREEN_DESC *fdesc,
	                                          IDXGIOutput *restrict_out,
	                                          IDXGISwapChain1 **out) override
	{
		return E_NOTIMPL;
	}

	HRESULT DXGI_P_API CreateSwapChainForCoreWindow(IUnknown *device,
	                                                IUnknown *window,
	                                                const DXGI_SWAP_CHAIN_DESC1 *desc,
	                                                IDXGIOutput *restrict_out,
	                                                IDXGISwapChain1 **out) override
	{
		return E_NOTIMPL;
	}

	HRESULT DXGI_P_API CreateSwapChainForComposition(IUnknown *device,
	                                                 const DXGI_SWAP_CHAIN_DESC1 *desc,
	                                                 IDXGIOutput *restrict_out,
	                                                 IDXGISwapChain1 **out) override
	{
		return E_NOTIMPL;
	}
};

}  // namespace

HRESULT Factory::wrap(const Config &config, REFIID riid, void **in_out)
{
	ComPtr<IUnknown> unknown{static_cast<IUnknown *>(*in_out)};
	return makeComObject<Factory>(config, std::move(unknown))->QueryInterface(riid, in_out);
}

Factory::Factory(const Config &config, ComPtr<IUnknown> unknown)
    : config{&config}
    , chain{std::move(unknown)}
{
	DXGI_P_LOG("factory is initialized");
	chain.enumerate([](const auto *p) { DXGI_P_LOGF("\t%s: 0x%p", typeid(std::decay_t<decltype(*p)>).name(), p); });
}

// IUnknown methods

HRESULT DXGI_P_API Factory::QueryInterface(REFIID riid, void **out)
{
	DXGI_P_LOGF("QueryInterface call with '%s' riid", format(riid).data());
	const auto hr = chain.proxiedQueryInterface(this, riid, out);
	if(hr == S_OK && *out == this) {
		DXGI_P_LOG("return pointer to self");
	}
	DXGI_P_LOGF("QueryInterface call result: 0x%X", hr);
	return hr;
}

// IDXGIObject methods

HRESULT DXGI_P_API Factory::GetParent(REFIID riid, void **out)
{
	return get<IDXGIObject>().GetParent(riid, out);
}

HRESULT DXGI_P_API Factory::GetPrivateData(REFGUID riid, UINT *size, void *data)
{
	return get<IDXGIObject>().GetPrivateData(riid, size, data);
}

HRESULT DXGI_P_API Factory::SetPrivateData(REFGUID riid, UINT size, const void *data)
{
	return get<IDXGIObject>().SetPrivateData(riid, size, data);
}

HRESULT DXGI_P_API Factory::SetPrivateDataInterface(REFGUID riid, const IUnknown *obj)
{
	return get<IDXGIObject>().SetPrivateDataInterface(riid, obj);
}

// IDXGIFactory methods

HRESULT DXGI_P_API Factory::EnumAdapters(UINT adapter, IDXGIAdapter **out)
{
	DXGI_P_LOGF("EnumAdapters call with '%u' index", adapter);
	auto hr = get<IDXGIFactory>().EnumAdapters(adapter, out);
	if(out && hr == S_OK) {
		hr = wrapAdapter(__uuidof(IDXGIAdapter), reinterpret_cast<void **>(out));
	}
	DXGI_P_LOGF("EnumAdapters call result: 0x%X", hr);
	return hr;
}

HRESULT DXGI_P_API Factory::MakeWindowAssociation(HWND hwnd, UINT flags)
{
	return get<IDXGIFactory>().MakeWindowAssociation(hwnd, flags);
}

HRESULT DXGI_P_API Factory::GetWindowAssociation(HWND *out)
{
	return get<IDXGIFactory>().GetWindowAssociation(out);
}

HRESULT DXGI_P_API Factory::CreateSwapChain(IUnknown *device, DXGI_SWAP_CHAIN_DESC *desc, IDXGISwapChain **out)
{
	return get<IDXGIFactory>().CreateSwapChain(device, desc, out);
}

HRESULT DXGI_P_API Factory::CreateSoftwareAdapter(HMODULE m, IDXGIAdapter **out)
{
	return get<IDXGIFactory>().CreateSoftwareAdapter(m, out);
}

// IDXGIFactory1 methods

HRESULT DXGI_P_API Factory::EnumAdapters1(UINT adapter, IDXGIAdapter1 **out)
{
	DXGI_P_LOGF("EnumAdapters1 call with '%u' index", adapter);
	auto hr = get<IDXGIFactory1>().EnumAdapters1(adapter, out);
	if(out && hr == S_OK) {
		hr = wrapAdapter(__uuidof(IDXGIAdapter1), reinterpret_cast<void **>(out));
	}
	DXGI_P_LOGF("EnumAdapters1 call result: 0x%X", hr);
	return hr;
}

BOOL DXGI_P_API Factory::IsCurrent()
{
	return get<IDXGIFactory1>().IsCurrent();
}

// IDXGIFactory2 methods

BOOL DXGI_P_API Factory::IsWindowedStereoEnabled()
{
	return get<IDXGIFactory2>().IsWindowedStereoEnabled();
}

HRESULT DXGI_P_API Factory::GetSharedResourceAdapterLuid(HANDLE resource, LUID *out)
{
	return get<IDXGIFactory2>().GetSharedResourceAdapterLuid(resource, out);
}

HRESULT DXGI_P_API Factory::RegisterStereoStatusWindow(HWND hwnd, UINT msg, DWORD *cookie)
{
	return get<IDXGIFactory2>().RegisterStereoStatusWindow(hwnd, msg, cookie);
}

HRESULT DXGI_P_API Factory::RegisterStereoStatusEvent(HANDLE event, DWORD *cookie)
{
	return get<IDXGIFactory2>().RegisterStereoStatusEvent(event, cookie);
}

void DXGI_P_API Factory::UnregisterStereoStatus(DWORD cookie)
{
	get<IDXGIFactory2>().UnregisterStereoStatus(cookie);
}

HRESULT DXGI_P_API Factory::RegisterOcclusionStatusWindow(HWND hwnd, UINT msg, DWORD *cookie)
{
	return get<IDXGIFactory2>().RegisterOcclusionStatusWindow(hwnd, msg, cookie);
}

HRESULT DXGI_P_API Factory::RegisterOcclusionStatusEvent(HANDLE event, DWORD *cookie)
{
	return get<IDXGIFactory2>().RegisterOcclusionStatusEvent(event, cookie);
}

void DXGI_P_API Factory::UnregisterOcclusionStatus(DWORD cookie)
{
	return get<IDXGIFactory2>().UnregisterOcclusionStatus(cookie);
}

HRESULT DXGI_P_API Factory::CreateSwapChainForHwnd(IUnknown *device,
                                                   HWND hwnd,
                                                   const DXGI_SWAP_CHAIN_DESC1 *desc,
                                                   const DXGI_SWAP_CHAIN_FULLSCREEN_DESC *fdesc,
                                                   IDXGIOutput *restrict_out,
                                                   IDXGISwapChain1 **out)
{
	return get<IDXGIFactory2>().CreateSwapChainForHwnd(device, hwnd, desc, fdesc, restrict_out, out);
}

HRESULT DXGI_P_API Factory::CreateSwapChainForCoreWindow(IUnknown *device,
                                                         IUnknown *window,
                                                         const DXGI_SWAP_CHAIN_DESC1 *desc,
                                                         IDXGIOutput *restrict_out,
                                                         IDXGISwapChain1 **out)
{
	return get<IDXGIFactory2>().CreateSwapChainForCoreWindow(device, window, desc, restrict_out, out);
}

HRESULT DXGI_P_API Factory::CreateSwapChainForComposition(IUnknown *device,
                                                          const DXGI_SWAP_CHAIN_DESC1 *desc,
                                                          IDXGIOutput *restrict_out,
                                                          IDXGISwapChain1 **out)
{
	return get<IDXGIFactory2>().CreateSwapChainForComposition(device, desc, restrict_out, out);
}

// IDXGIFactory3 methods

UINT DXGI_P_API Factory::GetCreationFlags()
{
	return get<IDXGIFactory3>().GetCreationFlags();
}

// IDXGIFactory4 methods

HRESULT DXGI_P_API Factory::EnumAdapterByLuid(LUID luid, REFIID riid, void **out)
{
	DXGI_P_LOGF("EnumAdapterByLuid call with '%s' riid", format(riid).data());
	auto hr = get<IDXGIFactory4>().EnumAdapterByLuid(luid, riid, out);
	if(out && hr == S_OK) {
		hr = wrapAdapter(riid, out);
	}
	DXGI_P_LOGF("EnumAdapterByLuid call result: 0x%X", hr);
	return hr;
}

HRESULT DXGI_P_API Factory::EnumWarpAdapter(REFIID riid, void **out)
{
	return get<IDXGIFactory4>().EnumWarpAdapter(riid, out);
}

// IDXGIFactory5 methods

HRESULT DXGI_P_API Factory::CheckFeatureSupport(DXGI_FEATURE feature, void *data, UINT size)
{
	return get<IDXGIFactory5>().CheckFeatureSupport(feature, data, size);
}

// IDXGIFactory6 methods

HRESULT DXGI_P_API Factory::EnumAdapterByGpuPreference(UINT adapter, DXGI_GPU_PREFERENCE preference, REFIID riid, void **out)
{
	DXGI_P_LOGF("EnumAdapterByGpuPreference call with '%s' riid", format(riid).data());
	auto hr = get<IDXGIFactory6>().EnumAdapterByGpuPreference(adapter, preference, riid, out);
	if(out && hr == S_OK) {
		hr = wrapAdapter(riid, out);
	}
	DXGI_P_LOGF("EnumAdapterByGpuPreference call result: 0x%X", hr);
	return hr;
}

// IDXGIFactory7 methods

HRESULT DXGI_P_API Factory::RegisterAdaptersChangedEvent(HANDLE event, DWORD *cookie)
{
	return get<IDXGIFactory7>().RegisterAdaptersChangedEvent(event, cookie);
}

HRESULT DXGI_P_API Factory::UnregisterAdaptersChangedEvent(DWORD cookie)
{
	return get<IDXGIFactory7>().UnregisterAdaptersChangedEvent(cookie);
}

// utilities

template<typename T>
T &Factory::get() const noexcept
{
	return *chain.anyOf<T>(&FactoryStub::instance());
}

HRESULT Factory::wrapAdapter(REFIID riid, void **in_out)
{
	if(config->dxgiSection().enable_adapter_proxy) {
		return Adapter::wrap(*config, addRef(static_cast<IUnknown *>(this)), riid, in_out);
	}
	return S_OK;
}

}  // namespace dxgi_proxy
