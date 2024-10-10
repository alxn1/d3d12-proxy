#include "pch.h"

namespace dxgi_proxy {

HRESULT Factory::wrap(REFIID riid, void *original, void **out)
{
	if(!original) {
		return E_NOINTERFACE;
	} else {
		ComPtr<IUnknown> unknown{static_cast<IUnknown *>(original)};
		const auto wrapper = makeObject<Factory>(std::move(unknown));
		return wrapper->QueryInterface(riid, out);
	}
}

Factory::Factory(ComPtr<IUnknown> unknown)
    : unknown{std::move(unknown)}
    , object{queryInterface<IDXGIObject>(this->unknown)}
    , factory{queryInterface<IDXGIFactory>(this->unknown)}
    , factory1{queryInterface<IDXGIFactory1>(this->unknown)}
    , factory2{queryInterface<IDXGIFactory2>(this->unknown)}
    , factory3{queryInterface<IDXGIFactory3>(this->unknown)}
    , factory4{queryInterface<IDXGIFactory4>(this->unknown)}
    , factory5{queryInterface<IDXGIFactory5>(this->unknown)}
    , factory6{queryInterface<IDXGIFactory6>(this->unknown)}
    , factory7{queryInterface<IDXGIFactory7>(this->unknown)}
{}

// IUnknown methods

HRESULT DXGI_P_API Factory::QueryInterface(REFIID riid, void **out)
{
	if(!out) {
		return E_POINTER;
	} else if(isWrapped(riid)) {
		*out = addRef(this).release();
		log("Factory", "Wrapper created");
		return S_OK;
	} else {
		log("Factory", "Unknown/unsupported interface usage");
		return object ? object->QueryInterface(riid, out) : E_NOINTERFACE;
	}
}

// IDXGIObject methods

HRESULT DXGI_P_API Factory::GetParent(REFIID riid, void **out)
{
	return object ? object->GetParent(riid, out) : E_NOTIMPL;
}

HRESULT DXGI_P_API Factory::GetPrivateData(REFGUID riid, UINT *size, void *data)
{
	return object ? object->GetPrivateData(riid, size, data) : E_NOTIMPL;
}

HRESULT DXGI_P_API Factory::SetPrivateData(REFGUID riid, UINT size, const void *data)
{
	return object ? object->SetPrivateData(riid, size, data) : E_NOTIMPL;
}

HRESULT DXGI_P_API Factory::SetPrivateDataInterface(REFGUID riid, const IUnknown *obj)
{
	return object ? object->SetPrivateDataInterface(riid, obj) : E_NOTIMPL;
}

// IDXGIFactory methods

HRESULT DXGI_P_API Factory::EnumAdapters(UINT adapter, IDXGIAdapter **out)
{
	const auto hr = factory ? factory->EnumAdapters(adapter, out) : E_NOTIMPL;
	return hr == S_OK
	           ? Adapter::wrap(ComPtr<IUnknown>{this}, __uuidof(IDXGIAdapter), static_cast<void *>(*out), reinterpret_cast<void **>(out))
	           : hr;
}

HRESULT DXGI_P_API Factory::MakeWindowAssociation(HWND hwnd, UINT flags)
{
	return factory ? factory->MakeWindowAssociation(hwnd, flags) : E_NOTIMPL;
}

HRESULT DXGI_P_API Factory::GetWindowAssociation(HWND *out)
{
	return factory ? factory->GetWindowAssociation(out) : E_NOTIMPL;
}

HRESULT DXGI_P_API Factory::CreateSwapChain(IUnknown *device, DXGI_SWAP_CHAIN_DESC *desc, IDXGISwapChain **out)
{
	return factory ? factory->CreateSwapChain(device, desc, out) : E_NOTIMPL;
}

HRESULT DXGI_P_API Factory::CreateSoftwareAdapter(HMODULE m, IDXGIAdapter **out)
{
	return factory ? factory->CreateSoftwareAdapter(m, out) : E_NOTIMPL;
}

// IDXGIFactory1 methods

HRESULT DXGI_P_API Factory::EnumAdapters1(UINT adapter, IDXGIAdapter1 **out)
{
	const auto hr = factory1 ? factory1->EnumAdapters1(adapter, out) : E_NOTIMPL;
	return hr == S_OK
	           ? Adapter::wrap(ComPtr<IUnknown>{this}, __uuidof(IDXGIAdapter1), static_cast<void *>(*out), reinterpret_cast<void **>(out))
	           : hr;
}

BOOL DXGI_P_API Factory::IsCurrent()
{
	return factory1 ? factory1->IsCurrent() : FALSE;
}

// IDXGIFactory2 methods

BOOL DXGI_P_API Factory::IsWindowedStereoEnabled()
{
	return factory2 ? factory2->IsWindowedStereoEnabled() : FALSE;
}

HRESULT DXGI_P_API Factory::GetSharedResourceAdapterLuid(HANDLE resource, LUID *out)
{
	return factory2 ? factory2->GetSharedResourceAdapterLuid(resource, out) : E_NOTIMPL;
}

HRESULT DXGI_P_API Factory::RegisterStereoStatusWindow(HWND hwnd, UINT msg, DWORD *cookie)
{
	return factory2 ? factory2->RegisterStereoStatusWindow(hwnd, msg, cookie) : E_NOTIMPL;
}

HRESULT DXGI_P_API Factory::RegisterStereoStatusEvent(HANDLE event, DWORD *cookie)
{
	return factory2 ? factory2->RegisterStereoStatusEvent(event, cookie) : E_NOTIMPL;
}

void DXGI_P_API Factory::UnregisterStereoStatus(DWORD cookie)
{
	if(factory2) {
		factory2->UnregisterStereoStatus(cookie);
	}
}

HRESULT DXGI_P_API Factory::RegisterOcclusionStatusWindow(HWND hwnd, UINT msg, DWORD *cookie)
{
	return factory2 ? factory2->RegisterOcclusionStatusWindow(hwnd, msg, cookie) : E_NOTIMPL;
}

HRESULT DXGI_P_API Factory::RegisterOcclusionStatusEvent(HANDLE event, DWORD *cookie)
{
	return factory2 ? factory2->RegisterOcclusionStatusEvent(event, cookie) : E_NOTIMPL;
}

void DXGI_P_API Factory::UnregisterOcclusionStatus(DWORD cookie)
{
	if(factory2) {
		factory2->UnregisterOcclusionStatus(cookie);
	}
}

HRESULT DXGI_P_API Factory::CreateSwapChainForHwnd(IUnknown *device,
                                                   HWND hwnd,
                                                   const DXGI_SWAP_CHAIN_DESC1 *desc,
                                                   const DXGI_SWAP_CHAIN_FULLSCREEN_DESC *fdesc,
                                                   IDXGIOutput *restrict_out,
                                                   IDXGISwapChain1 **out)
{
	return factory2 ? factory2->CreateSwapChainForHwnd(device, hwnd, desc, fdesc, restrict_out, out) : E_NOTIMPL;
}

HRESULT DXGI_P_API Factory::CreateSwapChainForCoreWindow(IUnknown *device,
                                                         IUnknown *window,
                                                         const DXGI_SWAP_CHAIN_DESC1 *desc,
                                                         IDXGIOutput *restrict_out,
                                                         IDXGISwapChain1 **out)
{
	return factory2 ? factory2->CreateSwapChainForCoreWindow(device, window, desc, restrict_out, out) : E_NOTIMPL;
}

HRESULT DXGI_P_API Factory::CreateSwapChainForComposition(IUnknown *device,
                                                          const DXGI_SWAP_CHAIN_DESC1 *desc,
                                                          IDXGIOutput *restrict_out,
                                                          IDXGISwapChain1 **out)
{
	return factory2 ? factory2->CreateSwapChainForComposition(device, desc, restrict_out, out) : E_NOTIMPL;
}

// IDXGIFactory3 methods

UINT DXGI_P_API Factory::GetCreationFlags()
{
	return factory3 ? factory3->GetCreationFlags() : 0;
}

// IDXGIFactory4 methods

HRESULT DXGI_P_API Factory::EnumAdapterByLuid(LUID luid, REFIID riid, void **out)
{
	const auto hr = factory4 ? factory4->EnumAdapterByLuid(luid, riid, out) : E_NOTIMPL;
	return hr == S_OK ? Adapter::wrap(ComPtr<IUnknown>{this}, riid, *out, out) : hr;
}

HRESULT DXGI_P_API Factory::EnumWarpAdapter(REFIID riid, void **out)
{
	return factory4 ? factory4->EnumWarpAdapter(riid, out) : E_NOTIMPL;
}

// IDXGIFactory5 methods

HRESULT DXGI_P_API Factory::CheckFeatureSupport(DXGI_FEATURE feature, void *data, UINT size)
{
	return factory5 ? factory5->CheckFeatureSupport(feature, data, size) : E_NOTIMPL;
}

// IDXGIFactory6 methods

HRESULT DXGI_P_API Factory::EnumAdapterByGpuPreference(UINT adapter, DXGI_GPU_PREFERENCE preference, REFIID riid, void **out)
{
	const auto hr = factory6 ? factory6->EnumAdapterByGpuPreference(adapter, preference, riid, out) : E_NOTIMPL;
	return hr == S_OK ? Adapter::wrap(ComPtr<IUnknown>{this}, riid, *out, out) : hr;
}

// IDXGIFactory7 methods

HRESULT DXGI_P_API Factory::RegisterAdaptersChangedEvent(HANDLE event, DWORD *cookie)
{
	return factory7 ? factory7->RegisterAdaptersChangedEvent(event, cookie) : E_NOTIMPL;
}

HRESULT DXGI_P_API Factory::UnregisterAdaptersChangedEvent(DWORD cookie)
{
	return factory7 ? factory7->UnregisterAdaptersChangedEvent(cookie) : E_NOTIMPL;
}

// utilities

bool Factory::isWrapped(REFIID riid) const noexcept
{
	if(riid == __uuidof(IUnknown)) {
		return static_cast<bool>(unknown);
	} else if(riid == __uuidof(IDXGIObject)) {
		return static_cast<bool>(object);
	} else if(riid == __uuidof(IDXGIFactory)) {
		return static_cast<bool>(factory);
	} else if(riid == __uuidof(IDXGIFactory1)) {
		return static_cast<bool>(factory1);
	} else if(riid == __uuidof(IDXGIFactory2)) {
		return static_cast<bool>(factory2);
	} else if(riid == __uuidof(IDXGIFactory3)) {
		return static_cast<bool>(factory3);
	} else if(riid == __uuidof(IDXGIFactory4)) {
		return static_cast<bool>(factory4);
	} else if(riid == __uuidof(IDXGIFactory5)) {
		return static_cast<bool>(factory5);
	} else if(riid == __uuidof(IDXGIFactory6)) {
		return static_cast<bool>(factory6);
	} else if(riid == __uuidof(IDXGIFactory7)) {
		return static_cast<bool>(factory7);
	} else {
		return false;
	}
}

}  // namespace dxgi_proxy
