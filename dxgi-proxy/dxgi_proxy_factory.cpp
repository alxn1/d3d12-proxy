#include "pch.h"

DXGI_P_LOG_SYSTEM(FACTORY);

namespace dxgi_proxy {

HRESULT Factory::wrap(const Config &config, REFIID riid, void *original, void **out)
{
	if(!original) {
		DXGI_P_LOG("attempt to wrap null pointer");
		return E_NOINTERFACE;
	} else {
		ComPtr<IUnknown> unknown{static_cast<IUnknown *>(original)};
		const auto wrapper = makeObject<Factory>(config, std::move(unknown));
		DXGI_P_LOG("new wrapper created");
		return wrapper->QueryInterface(riid, out);
	}
}

Factory::Factory(const Config &config, ComPtr<IUnknown> unknown)
    : config{&config}
    , unknown{std::move(unknown)}
    , object{queryInterface<IDXGIObject>(this->unknown)}
    , factory{queryInterface<IDXGIFactory>(this->unknown)}
    , factory1{queryInterface<IDXGIFactory1>(this->unknown)}
    , factory2{queryInterface<IDXGIFactory2>(this->unknown)}
    , factory3{queryInterface<IDXGIFactory3>(this->unknown)}
    , factory4{queryInterface<IDXGIFactory4>(this->unknown)}
    , factory5{queryInterface<IDXGIFactory5>(this->unknown)}
    , factory6{queryInterface<IDXGIFactory6>(this->unknown)}
    , factory7{queryInterface<IDXGIFactory7>(this->unknown)}
{
	DXGI_P_LOG("factory is initialized");
	DXGI_P_LOGF("\tIUnknown: %p", this->unknown.get());
	DXGI_P_LOGF("\tIDXGIObject: %p", object.get());
	DXGI_P_LOGF("\tIDXGIFactory: %p", factory.get());
	DXGI_P_LOGF("\tIDXGIFactory1: %p", factory1.get());
	DXGI_P_LOGF("\tIDXGIFactory2: %p", factory2.get());
	DXGI_P_LOGF("\tIDXGIFactory3: %p", factory3.get());
	DXGI_P_LOGF("\tIDXGIFactory4: %p", factory4.get());
	DXGI_P_LOGF("\tIDXGIFactory5: %p", factory5.get());
	DXGI_P_LOGF("\tIDXGIFactory6: %p", factory6.get());
	DXGI_P_LOGF("\tIDXGIFactory7: %p", factory7.get());
}

// IUnknown methods

HRESULT DXGI_P_API Factory::QueryInterface(REFIID riid, void **out)
{
	DXGI_P_LOGF("QueryInterface call with '%s' riid", format(riid).data());
	if(!out) {
		DXGI_P_LOG("output pointer is nullptr");
		return E_POINTER;
	} else if(isWrapped(riid)) {
		*out = addRef(this).release();
		DXGI_P_LOG("return pointer to self");
		return S_OK;
	} else {
		DXGI_P_LOG("unknown/unsupported interface, call original object");
		const auto hr = unknown ? unknown->QueryInterface(riid, out) : E_NOINTERFACE;
		DXGI_P_LOGF("QueryInterface call result: 0x%X", hr);
		return hr;
	}
}

// IDXGIObject methods

HRESULT DXGI_P_API Factory::GetParent(REFIID riid, void **out)
{
	auto *o = getObject();
	return o ? o->GetParent(riid, out) : E_NOTIMPL;
}

HRESULT DXGI_P_API Factory::GetPrivateData(REFGUID riid, UINT *size, void *data)
{
	auto *o = getObject();
	return o ? o->GetPrivateData(riid, size, data) : E_NOTIMPL;
}

HRESULT DXGI_P_API Factory::SetPrivateData(REFGUID riid, UINT size, const void *data)
{
	auto *o = getObject();
	return o ? o->SetPrivateData(riid, size, data) : E_NOTIMPL;
}

HRESULT DXGI_P_API Factory::SetPrivateDataInterface(REFGUID riid, const IUnknown *obj)
{
	auto *o = getObject();
	return o ? o->SetPrivateDataInterface(riid, obj) : E_NOTIMPL;
}

// IDXGIFactory methods

HRESULT DXGI_P_API Factory::EnumAdapters(UINT adapter, IDXGIAdapter **out)
{
	DXGI_P_LOGF("EnumAdapters call with '%u' index", adapter);
	auto *f = getFactory();
	auto hr = f ? f->EnumAdapters(adapter, out) : E_NOTIMPL;
	if(out && hr == S_OK && config->dxgiSection().enable_adapter_proxy) {
		hr = Adapter::wrap(*config,
		                   addRef(static_cast<IUnknown *>(this)),
		                   __uuidof(IDXGIAdapter),
		                   static_cast<void *>(*out),
		                   reinterpret_cast<void **>(out));
	}
	DXGI_P_LOGF("EnumAdapters call result: 0x%X", hr);
	return hr;
}

HRESULT DXGI_P_API Factory::MakeWindowAssociation(HWND hwnd, UINT flags)
{
	auto *f = getFactory();
	return f ? f->MakeWindowAssociation(hwnd, flags) : E_NOTIMPL;
}

HRESULT DXGI_P_API Factory::GetWindowAssociation(HWND *out)
{
	auto *f = getFactory();
	return f ? f->GetWindowAssociation(out) : E_NOTIMPL;
}

HRESULT DXGI_P_API Factory::CreateSwapChain(IUnknown *device, DXGI_SWAP_CHAIN_DESC *desc, IDXGISwapChain **out)
{
	auto *f = getFactory();
	return f ? f->CreateSwapChain(device, desc, out) : E_NOTIMPL;
}

HRESULT DXGI_P_API Factory::CreateSoftwareAdapter(HMODULE m, IDXGIAdapter **out)
{
	auto *f = getFactory();
	return f ? f->CreateSoftwareAdapter(m, out) : E_NOTIMPL;
}

// IDXGIFactory1 methods

HRESULT DXGI_P_API Factory::EnumAdapters1(UINT adapter, IDXGIAdapter1 **out)
{
	DXGI_P_LOGF("EnumAdapters1 call with '%u' index", adapter);
	auto *f = getFactory1();
	auto hr = f ? f->EnumAdapters1(adapter, out) : E_NOTIMPL;
	if(out && hr == S_OK && config->dxgiSection().enable_adapter_proxy) {
		hr = Adapter::wrap(*config,
		                   addRef(static_cast<IUnknown *>(this)),
		                   __uuidof(IDXGIAdapter1),
		                   static_cast<void *>(*out),
		                   reinterpret_cast<void **>(out));
	}
	DXGI_P_LOGF("EnumAdapters1 call result: 0x%X", hr);
	return hr;
}

BOOL DXGI_P_API Factory::IsCurrent()
{
	auto *f = getFactory1();
	return f ? f->IsCurrent() : FALSE;
}

// IDXGIFactory2 methods

BOOL DXGI_P_API Factory::IsWindowedStereoEnabled()
{
	auto *f = getFactory2();
	return f ? f->IsWindowedStereoEnabled() : FALSE;
}

HRESULT DXGI_P_API Factory::GetSharedResourceAdapterLuid(HANDLE resource, LUID *out)
{
	auto *f = getFactory2();
	return f ? f->GetSharedResourceAdapterLuid(resource, out) : E_NOTIMPL;
}

HRESULT DXGI_P_API Factory::RegisterStereoStatusWindow(HWND hwnd, UINT msg, DWORD *cookie)
{
	auto *f = getFactory2();
	return f ? f->RegisterStereoStatusWindow(hwnd, msg, cookie) : E_NOTIMPL;
}

HRESULT DXGI_P_API Factory::RegisterStereoStatusEvent(HANDLE event, DWORD *cookie)
{
	auto *f = getFactory2();
	return f ? f->RegisterStereoStatusEvent(event, cookie) : E_NOTIMPL;
}

void DXGI_P_API Factory::UnregisterStereoStatus(DWORD cookie)
{
	if(auto *f = getFactory2()) {
		f->UnregisterStereoStatus(cookie);
	}
}

HRESULT DXGI_P_API Factory::RegisterOcclusionStatusWindow(HWND hwnd, UINT msg, DWORD *cookie)
{
	auto *f = getFactory2();
	return f ? f->RegisterOcclusionStatusWindow(hwnd, msg, cookie) : E_NOTIMPL;
}

HRESULT DXGI_P_API Factory::RegisterOcclusionStatusEvent(HANDLE event, DWORD *cookie)
{
	auto *f = getFactory2();
	return f ? f->RegisterOcclusionStatusEvent(event, cookie) : E_NOTIMPL;
}

void DXGI_P_API Factory::UnregisterOcclusionStatus(DWORD cookie)
{
	if(auto *f = getFactory2()) {
		f->UnregisterOcclusionStatus(cookie);
	}
}

HRESULT DXGI_P_API Factory::CreateSwapChainForHwnd(IUnknown *device,
                                                   HWND hwnd,
                                                   const DXGI_SWAP_CHAIN_DESC1 *desc,
                                                   const DXGI_SWAP_CHAIN_FULLSCREEN_DESC *fdesc,
                                                   IDXGIOutput *restrict_out,
                                                   IDXGISwapChain1 **out)
{
	auto *f = getFactory2();
	return f ? f->CreateSwapChainForHwnd(device, hwnd, desc, fdesc, restrict_out, out) : E_NOTIMPL;
}

HRESULT DXGI_P_API Factory::CreateSwapChainForCoreWindow(IUnknown *device,
                                                         IUnknown *window,
                                                         const DXGI_SWAP_CHAIN_DESC1 *desc,
                                                         IDXGIOutput *restrict_out,
                                                         IDXGISwapChain1 **out)
{
	auto *f = getFactory2();
	return f ? f->CreateSwapChainForCoreWindow(device, window, desc, restrict_out, out) : E_NOTIMPL;
}

HRESULT DXGI_P_API Factory::CreateSwapChainForComposition(IUnknown *device,
                                                          const DXGI_SWAP_CHAIN_DESC1 *desc,
                                                          IDXGIOutput *restrict_out,
                                                          IDXGISwapChain1 **out)
{
	auto *f = getFactory2();
	return f ? f->CreateSwapChainForComposition(device, desc, restrict_out, out) : E_NOTIMPL;
}

// IDXGIFactory3 methods

UINT DXGI_P_API Factory::GetCreationFlags()
{
	auto *f = getFactory3();
	return f ? f->GetCreationFlags() : 0;
}

// IDXGIFactory4 methods

HRESULT DXGI_P_API Factory::EnumAdapterByLuid(LUID luid, REFIID riid, void **out)
{
	DXGI_P_LOGF("EnumAdapterByLuid call with '%s' riid", format(riid).data());
	auto *f = getFactory4();
	auto hr = f ? f->EnumAdapterByLuid(luid, riid, out) : E_NOTIMPL;
	if(out && hr == S_OK && config->dxgiSection().enable_adapter_proxy) {
		hr = Adapter::wrap(*config, addRef(static_cast<IUnknown *>(this)), riid, *out, out);
	}
	DXGI_P_LOGF("EnumAdapterByLuid call result: 0x%X", hr);
	return hr;
}

HRESULT DXGI_P_API Factory::EnumWarpAdapter(REFIID riid, void **out)
{
	auto *f = getFactory4();
	return f ? f->EnumWarpAdapter(riid, out) : E_NOTIMPL;
}

// IDXGIFactory5 methods

HRESULT DXGI_P_API Factory::CheckFeatureSupport(DXGI_FEATURE feature, void *data, UINT size)
{
	auto *f = getFactory5();
	return f ? f->CheckFeatureSupport(feature, data, size) : E_NOTIMPL;
}

// IDXGIFactory6 methods

HRESULT DXGI_P_API Factory::EnumAdapterByGpuPreference(UINT adapter, DXGI_GPU_PREFERENCE preference, REFIID riid, void **out)
{
	DXGI_P_LOGF("EnumAdapterByGpuPreference call with '%s' riid", format(riid).data());
	auto *f = getFactory6();
	auto hr = f ? f->EnumAdapterByGpuPreference(adapter, preference, riid, out) : E_NOTIMPL;
	if(out && hr == S_OK && config->dxgiSection().enable_adapter_proxy) {
		hr = Adapter::wrap(*config, addRef(static_cast<IUnknown *>(this)), riid, *out, out);
	}
	DXGI_P_LOGF("EnumAdapterByGpuPreference call result: 0x%X", hr);
	return hr;
}

// IDXGIFactory7 methods

HRESULT DXGI_P_API Factory::RegisterAdaptersChangedEvent(HANDLE event, DWORD *cookie)
{
	auto *f = getFactory7();
	return f ? f->RegisterAdaptersChangedEvent(event, cookie) : E_NOTIMPL;
}

HRESULT DXGI_P_API Factory::UnregisterAdaptersChangedEvent(DWORD cookie)
{
	auto *f = getFactory7();
	return f ? f->UnregisterAdaptersChangedEvent(cookie) : E_NOTIMPL;
}

// utilities

bool Factory::isWrapped(REFIID riid) const noexcept
{
	if(riid == __uuidof(IUnknown)) {
		return static_cast<bool>(unknown);
	} else if(riid == __uuidof(IDXGIObject)) {
		return getObject();
	} else if(riid == __uuidof(IDXGIFactory)) {
		return getFactory();
	} else if(riid == __uuidof(IDXGIFactory1)) {
		return getFactory1();
	} else if(riid == __uuidof(IDXGIFactory2)) {
		return getFactory2();
	} else if(riid == __uuidof(IDXGIFactory3)) {
		return getFactory3();
	} else if(riid == __uuidof(IDXGIFactory4)) {
		return getFactory4();
	} else if(riid == __uuidof(IDXGIFactory5)) {
		return getFactory5();
	} else if(riid == __uuidof(IDXGIFactory6)) {
		return getFactory6();
	} else if(riid == __uuidof(IDXGIFactory7)) {
		return getFactory7();
	} else {
		return false;
	}
}

}  // namespace dxgi_proxy
